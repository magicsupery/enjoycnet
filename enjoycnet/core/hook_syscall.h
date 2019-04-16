#pragma once

#include <arpa/inet.h>

#include "io_context.h"
#include <glog/logging.h>

namespace enjoyc
{
	namespace net
	{		
		int accept_hook(int fd, struct sockaddr* sockaddr, socklen_t* len)
		{
			auto co_event = ThreadContext::this_io_context()->get_coevent(fd);
			DLOG(INFO) << __FUNCTION__ << " co_event " << co_event <<" with fd " << fd;
			co_event->wait_read();
			return ::accept(fd, sockaddr, len);
		}

		uint32_t read_hook(int fd, const char* data, uint32_t len)
		{

			auto co_event = ThreadContext::this_io_context()->get_coevent(fd);

			DLOG(INFO) << __FUNCTION__ << " co_event " << co_event <<" with fd " << fd;
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

			uint32_t write_size = 0;
			auto co_event = ThreadContext::this_io_context()->get_coevent(fd);
			DLOG(INFO) << __FUNCTION__ << " co_event " << co_event <<" with fd " << fd;
			co_event->wait_write();

			do
			{
				write_size= ::write(fd, (void*)data, len);
			}while(write_size < 0  and (errno == EWOULDBLOCK or errno == EINTR));

			return write_size;
		}

	}
}
