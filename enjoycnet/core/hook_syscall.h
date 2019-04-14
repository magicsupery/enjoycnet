#pragma once

#include <arpa/inet.h>

#include "IOContext.h"

namespace enjoyc
{
	namespace net
	{

		int accept_hook(int fd, struct sockaddr* sockaddr, socklen_t* len)
		{
			auto co_event = ThreadContext::this_io_context()->get_coevent(fd);
			co_event->wait_write();
			return ::accept(fd_, sockaddr, len);
		}
	}
}
