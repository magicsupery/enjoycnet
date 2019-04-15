#pragma once

#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include "endpoint.h"
#include "hook_syscall.h"
#include <iostream>


namespace enjoyc
{
	namespace net
	{
		class Tcp
		{
			public:
				int listen(Endpoint & ep, uint32_t backlog)
				{
					if(not create())
						return -1;

					host_addr_ = ep;
					if(::bind(fd_, ep.sockaddr(), ep.sockadr_size()) < 0)
						return -1;

					if(::listen(fd_, backlog) < 0)
						return -1;

					return 0;
				}

				int accept(Tcp& t)
				{
					auto len = t.remote_addr_.sockadr_size();
					int res_fd = accept_hook(fd_, t.remote_addr_.sockaddr(), &len);
					if(res_fd < 0)
					{
						return res_fd;
					}
					else
					{
						t.host_addr_ = host_addr_;
						t.fd_ = res_fd;
						t.remote_addr_.calc_ipport_from_addr();
						
					}

					return res_fd;
				}
			
			public:
				const Endpoint& host_addr() const
				{
					return host_addr_;
				}

				const Endpoint& remote_addr() const
				{
					return remote_addr_;
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
