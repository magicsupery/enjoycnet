#include "IOContext.h"

namespace enjoyc
{
	namespace net
	{
		thread_local IOContext* IOContext::instance_ = nullptr;
	}
}
