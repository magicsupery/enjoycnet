#pragma once
#include <boost/context/all.hpp>
#include <iostream>

#include "../fcontext/stack_allocator.h"
#include "coroutine_context.h"

namespace enjoyc
{
	namespace co
	{
		using fcontext_t = boost::context::detail::fcontext_t;
		using transfer_t = boost::context::detail::transfer_t;
		using Function = std::function<void()>;
		using DefaultAllocator = StackAllocator<8 * 1024 * 1024, 4 * 1024 * 1024, 1 * 1024 * 1024>;


		class Coroutine: public NonCopyable
		{
			private:
				struct JumpData
				{
					Coroutine* from_co;
					Coroutine* to_co;
					void* data;
				};

			public:
				Coroutine(Function&& fn)
					:fn_(fn),
					from_t_(nullptr),
					to_t_(nullptr)
			{
				sp_ = stack_allocator_.allocate(stack_allocator_.default_stacksize());
				to_t_ = boost::context::detail::make_fcontext(sp_,
						stack_allocator_.default_stacksize(),
						&Coroutine::wrapper_function);
				state_ = Coroutine_S::READY;
			}

				~Coroutine()
				{
					if(state_ != Coroutine_S::FINISHED)
					{
						std::cout << "~Coroutine() " << this << " has not finished but dtor " << std::endl;
					}
					stack_allocator_.deallocate(sp_, stack_allocator_.default_stacksize());
				}

			public:
				RetCode start()
				{
					if(state_ == Coroutine_S::RUNNING)
					{
						return RetCode::ret_already_running;
					}
					else if(state_ == Coroutine_S::FINISHED)
					{
						return RetCode::ret_already_finished;
					}

					state_ = Coroutine_S::RUNNING;
					JumpData jump_data{CoroutineContext::this_coroutine(), this, nullptr};
					jump_to(to_t_, jump_data);
					
					if(state_ == Coroutine_S::FINISHED)
						return RetCode::ret_already_finished;

					return RetCode::ret_success;
				}

				void yield()
				{
					state_ = Coroutine_S::READY;
					auto back_transfer_t = boost::context::detail::jump_fcontext(from_t_, nullptr);
					set_from_context(back_transfer_t.fctx);
				}

				void finish()
				{
					state_ = Coroutine_S::FINISHED;
					boost::context::detail::jump_fcontext(from_t_, nullptr);
				}

				void run()
				{
					fn_();
				}

				void set_state(Coroutine_S s)
				{
					state_ = s;
				}

				void set_from_context(fcontext_t t)
				{

					from_t_ = t;
				}


			protected:
				static void wrapper_function(transfer_t t)
				{
					JumpData* jump_data = reinterpret_cast<JumpData*>(t.data);
					auto co_to = jump_data->to_co;
					co_to->set_from_context(t.fctx);

					co_to->run();

					co_to->set_state(Coroutine_S::FINISHED);
					co_to->finish();
				}

				void jump_to(fcontext_t& to, JumpData& jump_data)
				{
					CoroutineContext::set_current_coroutine(jump_data.to_co);
					auto back_transfer_t = boost::context::detail::jump_fcontext(to, &jump_data);
					to_t_ = back_transfer_t.fctx;
					CoroutineContext::set_current_coroutine(jump_data.from_co);

				}

			private:
				Function fn_;
				Coroutine_S state_;

				fcontext_t from_t_;
				fcontext_t to_t_;

				DefaultAllocator stack_allocator_;
				void* sp_;
		};


	}
}



