#pragma once

#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include "endpoint.h"
#include "hook_syscall.h"


namespace enjoyc
{
	namespace net
	{
		class ProtoTcp
		{
			public:
				int listen(Endpoint const& ep, uint32_t backlog)
				{
					if(not create())
						return -1;

					host_addr_ = ep;
					if(::bind(fd_, host_addr_.sockaddr(), host_addr_.sockadr_size() < 0))
						return -1;

					if(::listen(fd_, backlog) < 0)
						return -1;

					return 0;
				}

				int accept(ProtoTcp& t)
				{
					auto len = t.remote_addr_.sockadr_size();
					int res_fd = accept_hook(fd_, t.remote_addr_.sockaddr(), &len);
					if(res_fd < 0)
						return -1;
					else
					{
						t.host_addr_ = host_addr_;
					}

					return 0;
				}

			private:
				int create()
				{
					fd_ = socket_hook(AF_INET, SOCK_STREAM, 0);
					if(fd_ <= 0)
						return -1;

					// non-blocking
					if(fcntl(fd_, F_SETFL, fcntl(fd_, F_GETFL) | O_NONBLOCK) < 0)
						return -1;

					return fd_;
				}

			private:
				Endpoint host_addr_;
				Endpoint remote_addr_;
				int fd_;
		};
	}
}
