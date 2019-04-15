#pragma once

#include "socket.h"
#include "io_context.h"
#include <iostream>

namespace enjoyc
{
	namespace net
	{
		template <typename Proto, typename Codec>
		class Connection
		{
			typedef typename Codec::ReadCallback ReadCallback;

			public:
				Connection(IOContext* io_context, Socket<Proto> socket, ReadCallback&& read_callback)
				:io_context_(io_context),
				socket_(socket),
				codec_(std::forward<ReadCallback>(read_callback)),
				read_buffer_(1024),
				sending_(false)
				{
					
				}

				void read()
				{
					uint32_t n = socket_.read(read_buffer_.data(), 1024);
					std::cout << "connection read from socket " << n << std::endl;;		
					uint32_t consume = codec_.parse_message(read_buffer_.data(), n);
					
				}

				void write(const char*data, uint32_t len)
				{
					if(io_context_->is_in_create_thread())
					{
						std::cout << "wirte_in_thread" << std::endl;
						write_in_thread(data, len);
					}
					else
					{
						std::string tmp(data, len);
						io_context_->make_async_cb(
							[=]()
							{
								std::string real_data(tmp);
								write_in_thread(real_data.data(), real_data.size());
							}
						);
					}
				}

				void close()
				{
					
				}
			
			protected:
				inline void write_in_thread(const char*data, uint32_t len)
				{
					uint32_t n = socket_.write(data, len);
				}

			private:
				IOContext* io_context_;
				Socket<Proto> socket_;
				Codec codec_;

				std::vector<char> read_buffer_;
				std::vector<char> write_buffer_;

				bool sending_;

		};
	}
}
