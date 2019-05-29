#pragma once

#include <algorithm>
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
		
		constexpr int READ_BUFFER_SIZE = 1024 * 8;
		constexpr int READ_BUFFER_SHRINK_SIZE = 1024 * 32;
		constexpr int READ_BUFFER_MAX_SIZE_LIMIT = 1024 * 128;

		
		constexpr int WRITE_BUFFER_SIZE = 1024 * 8;
		constexpr int WRITE_BUFFER_SHRINK_SIZE = 1024 * 32;
		constexpr int WRITE_BUFFER_MAX_SIZE_LIMIT = 1024 * 128;
		constexpr int WRITE_ONCE_MAX_SIZE = 1024;

		template <typename Proto, typename Codec>
		class Connection : public std::enable_shared_from_this<Connection<Proto, Codec> >
		{

			typedef typename Codec::ReadCallback ReadCallback;

			public:
				Connection(IOContext* io_context, Socket<Proto> const& socket, ReadCallback&& read_callback)
				:io_context_(io_context),
				socket_(socket),
				codec_(std::forward<ReadCallback>(read_callback)),
				read_buffer_(READ_BUFFER_SIZE),
				read_buffer_pos_(0),
				write_buffer_(WRITE_BUFFER_SIZE),
				write_buffer_pos_(0),
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
					assert((state_ & S_READ) == 0 && io_context_->is_in_create_thread());
					state_ |= S_READ;
					size_t read_buffer_size = read_buffer_.size();
					ssize_t n = socket_.read(&read_buffer_[read_buffer_pos_],
							 read_buffer_size - read_buffer_pos_);
					
					DLOG(INFO) << __FUNCTION__ << " " << this << "read from socket " << n;
					if(n <= 0)
					{
						DLOG(ERROR) << __FUNCTION__ << " " << this << " read error " << errno;
						close();
						return false;
					}

					size_t consume = codec_.parse_message(read_buffer_.data(), read_buffer_pos_ + n);
					read_buffer_pos_ = read_buffer_pos_ + n - consume;

					DLOG(INFO) << __FUNCTION__ << " " << this <<" 1";
					if(consume > 0)
					{
						memcmp(read_buffer_.data(),&read_buffer_[read_buffer_pos_],
								read_buffer_pos_);
					}	
					
					DLOG(INFO) << __FUNCTION__ << " " << this <<" 2";
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

						read_buffer_.resize(std::min(read_buffer_size * 2, (size_t)READ_BUFFER_MAX_SIZE_LIMIT));
					}
					//shrink
					else if(read_buffer_size > READ_BUFFER_SHRINK_SIZE * 1.5 && 
							read_buffer_pos_ < READ_BUFFER_SHRINK_SIZE * 0.5)
					{
						read_buffer_.resize(READ_BUFFER_SHRINK_SIZE);
						read_buffer_.shrink_to_fit();
					}

					return true;
				}

				void write(const char*data, size_t len)
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
				inline void write_in_thread(const char*data, size_t len)
				{
					if((state_ & S_CLOSE) != 0)
						return;
					
					if((state_ & S_WRITE) != 0)
					{
						if(!write_to_buffer(data, len))
						{
							close();
						}

						return;
					}

					state_ |= S_WRITE;

					// once send size 
					if(len > WRITE_ONCE_MAX_SIZE)
					{
						auto write_size = len - WRITE_ONCE_MAX_SIZE;
						if(!write_to_buffer(data + WRITE_ONCE_MAX_SIZE, write_size))
						{
							close();
							return;
						}

						len = WRITE_ONCE_MAX_SIZE;
					}
					
					ssize_t n = socket_.write(data, len);
					if(n <=0)
					{
						close();
						return;
					}

					//not write clean or write_buffer has data
					while(n != len || write_buffer_pos_ > 0)
					{
						size_t left_len = len - n;
						size_t buffer_len = std::min((size_t)WRITE_ONCE_MAX_SIZE - left_len, write_buffer_pos_);
					#ifdef _WIN32
						char* write_data = (char*)_alloca(left_len + buffer_len);

						(void)memcmp(write_data, data + n, left_len);
						read_from_buffer(write_data + left_len, buffer_len);

						len = left_len + buffer_len;
						n = socket_.write((const char*)write_data, len);
					#else

						char write_data[left_len + buffer_len];

						(void)memcmp(&write_data[0], data + n, left_len);
						read_from_buffer(&write_data[left_len], buffer_len);

						len = left_len + buffer_len;
						n = socket_.write((const char*)&write_data, len);
					#endif
					}
					
					state_ &= ~S_WRITE;
				}

				inline void close_in_thread()
				{
					//only close once
					if((state_ & S_CLOSE) != 0)
						return;

					state_ |= S_CLOSE;
					socket_.close();
				}

				inline bool write_to_buffer(const char* data, size_t len)
				{

					//write to the buffer
					auto buffer_len = write_buffer_.size();

					//extend
					if((buffer_len - write_buffer_pos_) < len)
					{
						//reach limit
						if(buffer_len >= WRITE_BUFFER_MAX_SIZE_LIMIT)
						{
							LOG(ERROR) << __FUNCTION__ << " " << this << 
								"reach write buffer limit " << WRITE_BUFFER_MAX_SIZE_LIMIT;
							return false;
						}

						write_buffer_.resize(std::min(buffer_len * 2, (size_t)WRITE_BUFFER_MAX_SIZE_LIMIT));
					}

					memcpy(&write_buffer_[write_buffer_pos_], data, len);
					
					write_buffer_pos_ += len;
					return true;
				}

				inline void read_from_buffer(char* data, size_t len)
				{
					memcpy(data, write_buffer_.data(), len);
					
					write_buffer_pos_ -= len;
					memcpy(write_buffer_.data(), write_buffer_.data() + len, write_buffer_pos_);
				}

			private:
				IOContext* io_context_;
				Socket<Proto> socket_;
				Codec codec_;

				std::vector<char> read_buffer_;
				size_t read_buffer_pos_;
				std::vector<char> write_buffer_;
				size_t write_buffer_pos_;

				int state_;

		};
	}
}
