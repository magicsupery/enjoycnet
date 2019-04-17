#pragma once

#include <thread>
#include <mutex>
#include <vector>
#include <map>
#include <unordered_set>

#include <ev++.h>
#include <enjoycco/coroutine.h>

#include "utils.h"
#include "co_event.h"


namespace enjoyc
{
	namespace net
	{
		
		class IOContext;
		class ThreadContext: public NonCopyable
		{
			public:
				thread_local static IOContext* io_context_;
				static inline IOContext* this_io_context()
				{
					return io_context_;
				}
				static void init();

		};

		using Function = std::function<void()>;
		class IOContext: public NonCopyable
		{
			public:
				using FdMap = std::map<int, CoEvent*>;
			public:
				IOContext()
					:context_id_(std::this_thread::get_id())
				{
					ev_loop_ = new ev::dynamic_loop();
					
					sign_watcher_.set(ev_loop_->raw_loop);
					sign_watcher_.set<IOContext, &IOContext::loop_async_cb>(this);
				}

				~IOContext()
				{
					delete ev_loop_;
				}

			public:
				inline void run()
				{
					ev_loop_->run();
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

					sign_watcher_.send();

				}

				void loop_async_cb(ev::async &w, int revents)
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
					return;
				}

				inline CoEvent* destroy_coevent(int fd)
				{
					auto it = fd_2_coevent_.find(fd);
					if(it == fd_2_coevent_.end())
						return nullptr;
					
					auto res = it->second;
					fd_2_coevent_.erase(it);

					delete res;
					return res;
				}
		
			public:
				co::Coroutine* generate_coroutine(Function&& f)
				{
					auto co = new co::Coroutine(std::move(f));
					coroutines_.insert(co);
					return co;
				}		

				void destroy_coroutine(co::Coroutine* co)
				{
					auto it = coroutines_.find(co);
					if(it != coroutines_.end())
					{
						delete *it;
					}
				}

			private:
				std::thread::id  context_id_;
				ev::dynamic_loop* ev_loop_;
				ev::async sign_watcher_;

				std::mutex  mutex_;
				std::vector<Function>  functions_;

				FdMap fd_2_coevent_;

				std::unordered_set<co::Coroutine*> coroutines_;

		};

		void GO(Function&& f);

		void START(co::Coroutine* co);
		/*	
			struct GO
			{
			void operator()(Function &&f)
			{
			auto co = ThreadContext::this_io_context()->generate_coroutine(std::move(f));
			if(co->start()
			== co::RetCode::ret_already_finished)
			{
			ThreadContext::this_io_context()->destroy_coroutine(co);
			}
			}
			};
			*/
	}// net
} // enjoyc
