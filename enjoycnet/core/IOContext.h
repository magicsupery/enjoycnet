#pragma once

#include <ev.h>
#include <thread>
#include <mutex>
#include <vector>
#include <unordered_map>
#include "utils.h"
#include "co_event.h"


namespace enjoyc
{
	namespace net
	{
		class IOContext: public NonCopyable
		{
			using Function = std::function<void()>;
			using FdMap = std::unordered_map<int, CoEvent*>;
			public:
				IOContext()
					:context_id_(std::this_thread::get_id())
				{
					ev_loop_ = EV_DEFAULT;
					ev_init(&cb_sig_, std::bind(&IOContext::loop_async_cb, this, _1, _2);
					ev_async_start(ev_loop_, &cb_sig_);
				}

				~IOContext()
				{

				}

			public:
				inline void run()
				{
					ev_run(ev_loop_);
				}

				inline bool is_in_create_thread()
				{
					return context_id_ == std::this_thread::get_id();
				}

			public:
				inline void make_async_cb(Function&& function)
				{
					{
						std::lock_guard<std::mutex> lk(mutex_);
						functions_.emplace_back(function);
					}
					ev_async_send (EV_DEFAULT_ &cb_sig_);

				}

				void loop_async_cb(EV_P_ ev_async *w, int revents)
				{
					std::vector<Function> functions;
					{
						std::lock_guard<std::mutex> lk(mutex_);
						functions.swap(functions_);
					}

					for(auto& function : functions)
						function();
				}

			public:
				inline CoEvent* get_coevent(int fd)
				{
					return fd_2_coevent_[fd];
				}

				inline void gen_coevent(int fd)
				{
					auto co_event = new CoEvent(ev_loop_, fd);

					fd_2_coevent_[fd] = co_event;
					return co_event;
				}

				inline destroy_coevent(fd)
				{
					auto it = fd_2_coevent_.find(fd);
					if(it == fd_2_coevent_.end())
						return;

					fd_2_coevent_.erase(it);
				}
			private:
				std::thread::id  context_id_;
				struct ev_loop* ev_loop_;
				ev_async cb_sig_;

				std::mutex  mutex_;
				std::vector<Function>  functions_;

				FdMap fd_2_coevent_;

		};

		class ThreadContext: public NonCopyable
		{
			public:
				thread_local static IOContext* io_context_;
				inline IOContext* this_io_context()
				{
					return io_context_;
				}
				static void init();

		};
	}


}
