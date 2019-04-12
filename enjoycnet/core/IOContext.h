#pragma once

#include "ev.h"
#include <thread>
#include <mutex>
#include <vector>

namespace enjoyc
{
	namespace net
	{
		class IOContext
		{
			public:
				IOContext()
				{
					ev_loop_ = EV_DEFAULT; 
					
				}

				~IOContext() = default;

				IOContext(IOContext const& other) = default;

			public:
				inline void run()
				{
					ev_run(ev_loop_);
				}

				inline void async_write(const char* data, uint32_t len)
				{

					std::lock_guard<std::mutex> lk(mutex_);
					buffer_.emplace_back(std::string(data, len));
				}

				void async_write_cb()
				{
					std::vector<std::string> write_buffer;
					{	
						std::lock_guard<std::mutex> lk(mutex_);
						write_buffer.swap(buffer_);
					}

					//handle connection
						
				}

				
			private:
				std::mutex  mutex_;
				struct ev_loop* ev_loop_;
				ev_async mysig;
				std::vector<std::string>  buffer_;
				std::thread::id  context_id_;
		};
	}


}
