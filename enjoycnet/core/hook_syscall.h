#pragma once

#include <arpa/inet.h>

#include "io_context.h"
#include <iostream>

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
			//std::cout << "get event " << co_event << " with fd " << fd << std::endl;
			co_event->wait_read();
			int res_fd = ::accept(fd, sockaddr, len);
			if(res_fd > 0)
				ThreadContext::this_io_context()->gen_coevent(res_fd);

			return res_fd;
		}

		uint32_t read_hook(int fd, const char* data, uint32_t len)
		{

			auto co_event = ThreadContext::this_io_context()->get_coevent(fd);
			co_event->wait_read();
			uint32_t read_size = 0;
			do
			{
				read_size = ::read(fd, (void*)data, len);
			}while(read_size < 0  and (errno == EWOULDBLOCK or errno == EINTR));

			return read_size;
		}

		uint32_t write_hook(int fd, const char* data, uint32_t len)
		{
			return 0;
		}
	}
}
