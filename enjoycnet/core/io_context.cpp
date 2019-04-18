#include "io_context.h"
#include <glog/logging.h>

namespace enjoyc
{
	namespace net
	{
		thread_local IOContext* ThreadContext::io_context_= nullptr;

		void ThreadContext::init()
		{
			io_context_ = new IOContext();
		}


		void GO(Function&& f)
		{

			START(ThreadContext::this_io_context()->generate_coroutine(std::move(f)));
		}	

		void START(co::Coroutine* co)
		{
			DLOG(INFO) << "start co " << co;
			if(co->start()
					== co::RetCode::ret_already_finished)
			{
				ThreadContext::this_io_context()->destroy_coroutine(co);
			}
		}

	}
}
