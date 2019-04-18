#pragma once

#include "socket.h"
#include "io_context.h"
#include <assert.h>
#include <glog/logging.h>

namespace enjoyc
{
	namespace net
	{

		constexpr int S_READ = 0x0001;
		constexpr int S_WRITE = 0x0002;
		constexpr int S_CLOSE = 0x0004;
		
		constexpr int READ_BUFFER_SIZE = 1024 * 128;
		constexpr int READ_BUFFER_SHRINK_SIZE = 1024 * 1024 * 2;
		constexpr int READ_BUFFER_MAX_SIZE_LIMIT = 1024 * 1024 * 8;

		template <typename Proto, typename Codec>
		class Connection : public std::enable_shared_from_this<Connection<Proto, Codec> >
		{

			typedef typename Codec::ReadCallback ReadCallback;

			public:
				Connection(IOContext* io_context, Socket<Proto> const& socket, ReadCallback&& read_callback)
				:io_context_(io_context),
				socket_(socket),
				codec_(std::forward<ReadCallback>(read_callback)),
				read_buffer_(1024),
				read_buffer_pos_(0),
				state_(0x0000)
				{
					DLOG(INFO) << __FUNCTION__ << " " << this;
				}
				
				~Connection()
				{
					DLOG(INFO) << __FUNCTION__ << " " << this;
				}

				bool read()
				{
					//close return
					if((state_ & S_CLOSE) != 0)
						return false;

					// read at one time at this thread!
					// the readcb can not invoke read
					assert((state_ & S_READ) == 0 and io_context_->is_in_create_thread());
					state_ |= S_READ;
					uint32_t read_buffer_size = read_buffer_.size();
					uint32_t n = socket_.read(&read_buffer_[read_buffer_pos_],
							 read_buffer_size - read_buffer_pos_);
					
					DLOG(INFO) << __FUNCTION__ << " " << this << "read from socket " << n;
					if(n <= 0)
					{
						DLOG(ERROR) << __FUNCTION__ << " " << this << " read error " << errno;
						close();
						return false;
					}

					uint32_t consume = codec_.parse_message(read_buffer_.data(), read_buffer_pos_ + n);
					read_buffer_pos_ = read_buffer_pos_ + n - consume;
					if(consume > 0)
					{
						memcmp(read_buffer_.data(),&read_buffer_[read_buffer_pos_],
								read_buffer_pos_);
					}	
					
					state_ &= ~S_READ;

					//extend	
					if(read_buffer_pos_ >= read_buffer_size)
					{
						if(read_buffer_size >= READ_BUFFER_MAX_SIZE_LIMIT)
						{
							LOG(ERROR) << __FUNCTION__ << " " << this << " buffer size overflow limit " << READ_BUFFER_MAX_SIZE_LIMIT;
							close();
							return false;
						}

						read_buffer_.resize(std::min(read_buffer_size * 2, (uint32_t)READ_BUFFER_MAX_SIZE_LIMIT));
					}
					//shrink
					else if(read_buffer_size > READ_BUFFER_SHRINK_SIZE * 1.5 and 
							read_buffer_pos_ < READ_BUFFER_SHRINK_SIZE * 0.5)
					{
						read_buffer_.resize(READ_BUFFER_SHRINK_SIZE);
						read_buffer_.shrink_to_fit();
					}

					return true;
				}

				void write(const char*data, uint32_t len)
				{
					if(io_context_->is_in_create_thread())
					{
						write_in_thread(data, len);
					}
					else
					{
						std::string tmp(data, len);

						auto sp = this->shared_from_this();
						io_context_->make_async_cb(
							[=]()
							{
								std::string real_data(tmp);
								sp->write_in_thread(real_data.data(), real_data.size());
							}
						);
					}
				}

				void close()
				{
					if(io_context_->is_in_create_thread())
					{
						close_in_thread();
					}
					else
					{
						auto sp = this->shared_from_this();
						io_context_->make_async_cb(
							[=]()
							{
								sp->close_in_thread();
							});
					}
				}

			protected:
				inline void write_in_thread(const char*data, uint32_t len)
				{
					if((state_ & S_CLOSE) != 0)
						return;

					uint32_t n = socket_.write(data, len);
				}

				inline void close_in_thread()
				{
					//only close once
					if((state_ & S_CLOSE) != 0)
						return;

					state_ |= S_CLOSE;
					socket_.close();
				}

			private:
				IOContext* io_context_;
				Socket<Proto> socket_;
				Codec codec_;

				std::vector<char> read_buffer_;
				uint32_t read_buffer_pos_;
				std::vector<char> write_buffer_;

				int state_;

		};
	}
}
