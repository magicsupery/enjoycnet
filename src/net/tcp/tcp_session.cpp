#include "tcp_session.h"
#include "assert.h"
#include <glog/logging.h>
#include "../../option/parser/parser.h"


namespace enjoyc
{
	namespace net
	{

		tcp_io_service& get_tcp_io_service()
		{
			static tcp_io_service io_service;
			return	io_service;
		}


		TcpSession::TcpSession(TcpSocketPtr tcp_socket_ptr, OptionPtr option_ptr, 
				Endpoint const& local_addr)
			:socket_ptr_(tcp_socket_ptr),
			 local_addr_(local_addr),
			 remote_addr_(tcp_socket_ptr->remote_endpoint().address(),
					 tcp_socket_ptr->remote_endpoint().port(), 
					 local_addr.get_proto_type(), local_addr.get_path()),
			 option_session_ptr_(std::unique_ptr<
					 OptionSessionDefine>(new OptionSessionDefine(option_ptr->define_data_))),

			 msg_chan_(option_ptr->net_.msg_chan_buffer_num_),
			 max_read_buf_num_(option_ptr->net_.read_buffer_num_)
		{
			DLOG(INFO) << __FUNCTION__ << " " << this;
		}
		
		TcpSession::~TcpSession()
		{
			DLOG(INFO) << __FUNCTION__ << " " << this;
			assert(option_session_ptr_ == nullptr);
		}

		void TcpSession::start()
		{
			go [=]()
			{
				shared_from_this()->go_send();
			};

			go [=]()
			{
				shared_from_this()->go_receive();
			};

			send_closed_flag_.store(false);
			receive_closed_flag_.store(false);
			closed_flag_.store(false);
		}

		void TcpSession::send(Buffer&& buf)
		{
			if(closed_flag_ or send_closed_flag_)
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

			if(closed_flag_ or send_closed_flag_)
				return;

			auto buffer_ptr = std::make_shared<Buffer>(len);
			memcpy(buffer_ptr->data(), data, len);
			if(not msg_chan_.TryPush(buffer_ptr))
			{
				LOG(ERROR) << __FUNCTION__ << this << " msg chan full";
			}
		}

		void TcpSession::shutdown()
		{
			if(closed_flag_)
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
			for(;;)
			{

				//get buffers as much as possible
				while(true)
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
						LOG(ERROR) << __FUNCTION__ << " " << this << " " << "get shutdown msg";
						on_close_send();
						return;
					}

					buffers.emplace_back(boost::asio::buffer(buffer_ptr->data(), buffer_ptr->size()));
					send_once_size += buffer_ptr->size();
				}

				boost_ec ec;
				auto send_size = boost::asio::write(*socket_ptr_, buffers, ec);
				assert(send_size == send_once_size);

				buffers.clear();
				buffer_ptr = nullptr;
				send_once_size = 0;
				if(ec)
				{
					LOG(ERROR) << __FUNCTION__ << " " << this << " " << "write socket error" << ec.message();
					on_close_send();
					return;
				}
			}
		}

		void TcpSession::go_receive()
		{
			DLOG(INFO) << __FUNCTION__ ;

			boost_ec ec;
			char read_buf[max_read_buf_num_];
			for(;;)
			{
				size_t n = socket_ptr_->read_some(boost::asio::buffer(read_buf, max_read_buf_num_), ec);
				option_session_ptr_->get_parser_ptr()->parse_message(shared_from_this(), read_buf, n);
				if(ec)
				{
					LOG(ERROR) << __FUNCTION__ << " " << this << " " << "read socket error" << ec.message();
					on_close_receive();
					return;
				}

			}
		}


		void TcpSession::on_close_send()
		{
			send_closed_flag_ = true;
			boost_ec ec;
			socket_ptr_->shutdown(boost::asio::socket_base::shutdown_type::shutdown_send, ec);
			if(receive_closed_flag_)
				real_close();

		}

		void TcpSession::on_close_receive()
		{
			receive_closed_flag_ = true;

			boost_ec ec;
			socket_ptr_->shutdown(boost::asio::socket_base::shutdown_type::shutdown_receive, ec);
			if(send_closed_flag_)
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
			socket_ptr_ = nullptr;
			
			//2.clear msg_chan_
			//
			msg_chan_.Close();
			BufferPtr ptr;
			while(msg_chan_.TryPop(ptr));

			//3.cb
			
		}

	}

}



