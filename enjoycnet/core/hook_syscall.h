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
		
		int connect_hook(int fd, struct sockaddr* sockaddr, socklen_t len)
		{

			auto co_event = ThreadContext::this_io_context()->get_coevent(fd);
			DLOG(INFO) << __FUNCTION__ << " co_event " << co_event <<" with fd " << fd;
			co_event->wait_write();
			return ::connect(fd, sockaddr, len);
		}

		ssize_t read_hook(int fd, const char* data, uint32_t len)
		{

			auto co_event = ThreadContext::this_io_context()->get_coevent(fd);

			DLOG(INFO) << __FUNCTION__ << " co_event " << co_event <<" with fd " << fd;
			co_event->wait_read();
			ssize_t read_size = 0;
			do
			{
				read_size = ::recv(fd, (void*)data, len, MSG_NOSIGNAL);
			}while(read_size < 0  and (errno == EWOULDBLOCK or errno == EINTR));
			
			DLOG(INFO) << __FUNCTION__ << " co_event " << co_event <<" with fd " << fd << " size is " << read_size;
			return read_size;
		}

		ssize_t write_hook(int fd, const char* data, uint32_t len)
		{
			ssize_t write_size = 0;

			//try to syswrite first
			do{

				write_size= ::send(fd, (void*)data, len, MSG_NOSIGNAL);

			}while(write_size < 0  and (errno == EINTR));
			if(write_size >= 0)
			{
				return write_size;
			}

			//try co_event
			auto co_event = ThreadContext::this_io_context()->get_coevent(fd);
			DLOG(INFO) << __FUNCTION__ << " co_event " << co_event <<" with fd " << fd;
			co_event->wait_write();

			do
			{
				write_size= ::send(fd, (void*)data, len, MSG_NOSIGNAL);
			}while(write_size < 0  and (errno == EWOULDBLOCK or errno == EINTR));

			return write_size;
		}

	}
}
