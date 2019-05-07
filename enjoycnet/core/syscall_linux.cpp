#include "syscall.h"

#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>

namespace enjoyc
{
	namespace net
	{
		int get_socket(int family, int type, int protocol)
		{
			return ::socket(family, type, protocol);
		}

		int close_socket(int fd)
		{
			return ::close(fd);
		}

		int make_socket_nonblocking(int fd)
		{
			return fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
		}

		int make_socket_resue(int fd)
		{
			int option = 1;
			return ::setsockopt(fd, SOL_SOCKET, (SO_REUSEADDR | SO_REUSEPORT), 
						(char*)&option, sizeof(option));
		}

	}//net
}//enjoyc




