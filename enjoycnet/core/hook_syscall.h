#pragma once


#include "syscall.h"
#include "io_context.h"
#include <glog/logging.h>

namespace enjoyc
{
	namespace net
	{		
		int accept_hook(int fd, struct sockaddr* sockaddr, socklen_t* len)
		{
			auto co_event = ThreadContext::this_io_context()->get_coevent(fd);
			LOG(INFO) << __FUNCTION__ << " co_event " << co_event <<" with fd " << fd;
			co_event->wait_read();

			LOG(INFO) << __FUNCTION__ << " co_event " << co_event << " with fd " << fd << " can read ";
			return accept_syscall(fd, sockaddr, len);
		}
		
		int connect_hook(int fd, struct sockaddr* sockaddr, socklen_t len)
		{
			// must call connect_syscall first£¬ otherwise there's no wriable event can be polled
			int ret = 0;
			do
			{
				ret = connect_syscall(fd, sockaddr, len);
			} while (ret < 0 && CHECK_ERROR(sys_errno(), EINTR));

			if (ret < 0)
			{
				int err = sys_errno();
				while (CHECK_ERROR(err, EWOULDBLOCK) || CHECK_ERROR(err, EINPROGRESS))
				{
					auto co_event = ThreadContext::this_io_context()->get_coevent(fd);
					co_event->wait_write();
					err = get_socket_error_syscall(fd);
					LOG(INFO) << " err is " << err;
					if (err == 0)
						return err;
				}
			}

			return ret;

		}

		ssize_t read_hook(int fd, const char* data, uint32_t len)
		{

			auto co_event = ThreadContext::this_io_context()->get_coevent(fd);

			DLOG(INFO) << __FUNCTION__ << " co_event " << co_event << " with fd " << fd;
			co_event->wait_read();
			ssize_t read_size = 0;
			do
			{
				recv_syscall(fd, data, len, read_size);
			} while (read_size < 0 && (errno == EWOULDBLOCK || errno == EINTR));

			DLOG(INFO) << __FUNCTION__ << " co_event " << co_event << " with fd " << fd << " size is " << read_size;
			return read_size;
		}

		ssize_t write_hook(int fd, const char* data, uint32_t len)
		{
			ssize_t write_size = 0;

			//try to syswrite first
			do {
				send_syscall(fd, data, len, write_size);

			} while (write_size < 0 && (errno == EINTR));
			if (write_size >= 0)
			{
				return write_size;
			}

			//try co_event
			auto co_event = ThreadContext::this_io_context()->get_coevent(fd);
			DLOG(INFO) << __FUNCTION__ << " co_event " << co_event << " with fd " << fd;
			co_event->wait_write();

			do
			{
				send_syscall(fd, data, len, write_size);
			} while (write_size < 0 && (errno == EWOULDBLOCK || errno == EINTR));

			return write_size;
		}

	}
}
