#pragma once

#include "socket.h"
#include "io_context.h"

namespace enjoyc
{
	namespace net
	{
		template <typename Proto, typename Codec>
		class Connection
		{
			public:
				Connection(IOContext* io_context, Socket<Proto> socket)
				:io_context_(io_context),
				socket_(socket),
				sending_(false)
				{

				}

				uint32_t read()
				{
					uint32_t n = socket_.read();

					//uint32_t consume = codec_.parse_message(xxx);
					return n;
				}

				void write(const char*data, uint32_t len)
				{
					if(io_context_->is_in_create_thread())
						write_in_thread(data, len);
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

			protected:
				inline void write_in_thread(const char*data, uint32_t len)
				{

				}

				void write_in_thread(std::string& str)
				{

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
