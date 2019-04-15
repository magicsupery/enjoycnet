#pragma once

#include <arpa/inet.h>

#include "io_context.h"

namespace enjoyc
{
	namespace net
	{
		
		int socket_hook(int family, int type, int protocol)
		{
			int fd = socket(family, type, protocol);
			ThreadContext::this_io_context()->gen_coevent(fd);
			return fd;
		}

		int accept_hook(int fd, struct sockaddr* sockaddr, socklen_t* len)
		{
			auto co_event = ThreadContext::this_io_context()->get_coevent(fd);
			co_event->wait_write();
			return ::accept(fd, sockaddr, len);
		}
	}
}
