#include "tcp_session.h"
#include "assert.h"
#include <glog/logging.h>
#include <random>

namespace enjoyc
{
	namespace net
	{

		tcp_io_service& get_tcp_io_service()
		{
			static tcp_io_service io_service;
			return	io_service;
		}


		TcpSession::TcpSession(Scheduler* sche, TcpSocketPtr tcp_socket_ptr, 
				OptionPtr option_ptr, SessionHandlerPtr handler_ptr,
				Endpoint const& local_addr)
			:sche_(sche),
			 socket_ptr_(tcp_socket_ptr),
			 option_ptr_(option_ptr),
			 handler_ptr_(handler_ptr),
			 local_addr_(local_addr),
			 remote_addr_(tcp_socket_ptr->remote_endpoint().address(),
					 tcp_socket_ptr->remote_endpoint().port()),
			 msg_chan_(option_ptr->net_.msg_chan_buffer_num_),
			 max_read_buf_size_(option_ptr->net_.read_buffer_size_),
			 max_read_buf_size_limit_(option_ptr->net_.max_read_buf_size_limit_),
			 max_read_buf_shrik_size_(option_ptr->net_.max_read_buf_shrik_size_),
			 read_buf_(max_read_buf_size_),
			 current_read_pos_(0)
		{
			DLOG(INFO) << __FUNCTION__ << " " << this;
		}

		TcpSession::~TcpSession()
		{
			DLOG(INFO) << __FUNCTION__ << " " << this;
		}

		void TcpSession::start()
		{
			go co_scheduler(sche_)[=]()
			{
				shared_from_this()->go_send();
			};

			go co_scheduler(sche_)[=]()
			{
				shared_from_this()->go_receive();
			};

			send_closed_flag_.store(false);
			receive_closed_flag_.store(false);
			closed_flag_.store(false);
		}

		void TcpSession::send(Buffer&& buf)
		{
			if(closed_flag_.load() or send_closed_flag_.load())
				return;

			auto buffer_ptr = std::make_shared<Buffer>();
			buffer_ptr->swap(buf);
			if(not msg_chan_.TryPush(buffer_ptr))
			{
				LOG(ERROR) << __FUNCTION__ << this << " msg chan full";
			}
		}

		void TcpSession::send(const char* data, size_t len)
		{
			if(closed_flag_.load() or send_closed_flag_.load())
				return;
			auto buffer_ptr = std::make_shared<Buffer>(len);
			memcpy(buffer_ptr->data(), data, len);
			if(not msg_chan_.TryPush(buffer_ptr))
			{
				LOG(ERROR) << __FUNCTION__ << this << " msg chan full";
			}
		}

		void TcpSession::send_no_delay(Buffer&& buf)
		{
			send_no_delay(buf.data(), buf.size());

		}

		void TcpSession::send_no_delay(const char* data, size_t len)
		{
			if(closed_flag_)
				return;

			boost_ec ec;
			boost::asio::write(*socket_ptr_, boost::asio::buffer(data, len), ec);
			if(ec)
			{
				on_close_send();
			}
		}

		void TcpSession::shutdown()
		{
			DLOG(INFO) << __FUNCTION__ ;
			if(closed_flag_.load())
				return;

			boost_ec ec;
			socket_ptr_->shutdown(boost::asio::socket_base::shutdown_type::shutdown_both, ec);
			msg_chan_.TryPush(nullptr);

		}

		Endpoint TcpSession::local_addr()
		{
			return	local_addr_;
		}

		Endpoint TcpSession::remote_addr()
		{
			return	remote_addr_;
		}

		//------------------------------private method----------------------------------
		void TcpSession::go_send()
		{
			DLOG(INFO) << __FUNCTION__ ;

			std::vector<boost::asio::const_buffer> buffers;
			BufferPtr buffer_ptr = nullptr;
			size_t send_once_size = 0;
			size_t cur_num = 0;
			size_t max_once_send_num = std::min<int>(64, boost::asio::detail::max_iov_len);
			std::vector<BufferPtr> send_buffer_ptrs(max_once_send_num);
			bool msg_shutdown = false;
			for(;;)
			{
				if(msg_shutdown)
				{
					on_close_send();
					return;
				}

				//get buffers as much as possible
				while(cur_num < max_once_send_num)
				{
					if(!msg_chan_.TryPop(buffer_ptr))
					{
						if(buffers.size() != 0)
							break;

						//no msg at all, wait until has one
						msg_chan_ >> buffer_ptr;
					}

					if(buffer_ptr == nullptr)
					{
						//LOG(ERROR) << __FUNCTION__ << " " << this << " " << "get shutdown msg";
						msg_shutdown = true;
						break;
					}

					buffers.emplace_back(boost::asio::buffer(buffer_ptr->data(), buffer_ptr->size()));
					send_buffer_ptrs.emplace_back(buffer_ptr);
					send_once_size += buffer_ptr->size();
					cur_num ++;
				}

				boost_ec ec;
				auto send_size = boost::asio::write(*socket_ptr_, buffers, ec);

				buffer_ptr = nullptr;
				buffers.clear();
				send_buffer_ptrs.clear();
				cur_num = 0;
				if(ec)
				{
					//LOG(ERROR) << __FUNCTION__ << " " << this << " " << "write socket error" << ec.message();
					on_close_send();
					return;
				}
				else
				{
					assert(send_size == send_once_size);
				}

				send_once_size = 0;
			}
		}

		void TcpSession::go_receive()
		{
			DLOG(INFO) << __FUNCTION__ ;

			boost_ec ec;

			auto shrik_size = max_read_buf_shrik_size_ + max_read_buf_shrik_size_ / 2;
			auto shrik_size_half = max_read_buf_shrik_size_ / 2;
			for(;;)
			{

				size_t n = socket_ptr_->read_some(
						boost::asio::buffer(&read_buf_[current_read_pos_],
							read_buf_.size() - current_read_pos_), ec);
				if(ec)
				{
					//LOG(ERROR) << __FUNCTION__ << " " << this << " " << "read socket error" << ec.message();
					on_close_receive();
					return;
				}
				auto consume = handler_ptr_->parse_message(shared_from_this(), 
						&read_buf_[0], n + current_read_pos_);

				current_read_pos_ = current_read_pos_ + n - consume;
				if(consume > 0 and current_read_pos_ > 0)
				{
					memcpy(&read_buf_[0], &read_buf_[consume], current_read_pos_);
				}

				if(read_buf_.size() > shrik_size && current_read_pos_ < shrik_size_half)
				{
					read_buf_.resize(max_read_buf_shrik_size_);
					read_buf_.shrink_to_fit();
				}

			}
		}


		void TcpSession::on_close_send()
		{
			send_closed_flag_.store(true);

			boost_ec ec;
			socket_ptr_->shutdown(boost::asio::socket_base::shutdown_type::shutdown_send, ec);
			if(receive_closed_flag_.load())
				real_close();

		}

		void TcpSession::on_close_receive()
		{
			receive_closed_flag_.store(true);

			boost_ec ec;
			socket_ptr_->shutdown(boost::asio::socket_base::shutdown_type::shutdown_receive, ec);
			if(send_closed_flag_.load())
			{
				real_close();
			}
			else
			{
				msg_chan_ << nullptr;
				socket_ptr_->shutdown(boost::asio::socket_base::shutdown_type::shutdown_send, ec);
			}
		}

		void TcpSession::real_close()
		{
			bool expected = false;

			if(!closed_flag_.compare_exchange_strong(expected, true,
						std::memory_order_acq_rel, std::memory_order_relaxed))
				return;

			// real close thing

			//1.clear socket	
			if(socket_ptr_->is_open())
			{
				socket_ptr_->close();
			}			

			//2.clear msg_chan_
			//
			msg_chan_.Close();
			BufferPtr ptr;
			while(msg_chan_.TryPop(ptr));

			//3.cb
			option_ptr_->cb_.dis_cb_(shared_from_this());
		}

	}

}




