#include "io_context.h"

namespace enjoyc
{
	namespace net
	{
		thread_local IOContext* ThreadContext::io_context_= nullptr;

		void ThreadContext::init()
		{
			io_context_ = new IOContext();
		}
	}
}
