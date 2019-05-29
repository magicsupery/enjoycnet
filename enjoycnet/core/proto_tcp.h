#pragma once

#include "endpoint.h"
#include "hook_syscall.h"

namespace enjoyc
{
	namespace net
	{
		class Tcp
		{
			enum class TcpSocketState
			{
				INVALID = 0,
				ACTIVE = 1,
				CLOSED = 2
			};

			public:
				Tcp()
					:fd_(-1),
					state_(TcpSocketState::INVALID){}

				~Tcp()
				{
					unactive();
				}

				Tcp(Tcp const& ) = default;
				Tcp& operator = (Tcp const&) = delete;

			public:
				int listen(Endpoint & ep, uint32_t backlog)
				{
					if(create() < 0)
						return -1;

					host_addr_ = ep;
					if (bind_syscall(fd_, ep.sockaddr(), ep.sockadr_size()) < 0)
					{
						LOG(ERROR) << "can not bind " << fd_;
						return -1;
					}
						

					if (listen_syscall(fd_, backlog) < 0)
					{
						LOG(ERROR) << "can not listen" << fd_;
						return -1;
					}

					return 0;
				}
				
				int connect(Endpoint & ep)
				{
					if(create() < 0)
						return -1;
					
					remote_addr_ = ep;
					if(connect_hook(fd_, ep.sockaddr(), ep.sockadr_size()) < 0)
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
						t.remote_addr_.calc_ipport_from_addr();
						t.active(res_fd);
						make_socket_nonblocking(res_fd);
						
					}

					return res_fd;
				}
				
				ssize_t read(const char* data, uint32_t len)
				{
					return read_hook(fd_, data, len);
				}

				ssize_t write(const char* data, uint32_t len)
				{
					return write_hook(fd_, data, len);
				}

				void close()
				{
					if(state_!= TcpSocketState::CLOSED)
					{
						/* can not unactive when close
						 * reason:
						 * co_event's coroutine maybe waiting,
						 * should call close to trig the coroutine wakeup
						 * The unactive time is this socket owner dtor(destructor)
						 * 
						*/
						//::shutdown(fd_, SHUT_RDWR);
						close_syscall(fd_);
						state_ = TcpSocketState::CLOSED;
					}
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
					int fd = socket_syscall(AF_INET, SOCK_STREAM, 0);
					if(fd <= 0)
						return -1;
					
					// non-blocking
					if(make_socket_nonblocking(fd) < 0)
					{
						LOG(ERROR) << "nonblocking failed " << fd;
						close_syscall(fd);
						return -1;
					}

					if(make_socket_resue(fd) < 0)
					{
						LOG(ERROR) << "reuse failed " << fd;
						close_syscall(fd);
						return -1;
					}

					active(fd);
					return fd;
				}
			
			private:
				// active&unactive co_event
				void active(int fd)
				{
					fd_ = fd;
					state_ = TcpSocketState::ACTIVE;
					ThreadContext::this_io_context()->gen_coevent(fd_);
				}	

				void unactive()
				{
					//if the state is not CLOSED,
					//has no right to destroy_coevent
					if(state_ != TcpSocketState::CLOSED)
						return;
					
					ThreadContext::this_io_context()->destroy_coevent(fd_);

				}

			private:
				Endpoint host_addr_;
				Endpoint remote_addr_;
				int fd_;
				TcpSocketState state_;
		};
	}
}
