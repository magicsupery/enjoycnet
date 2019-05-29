#include "syscall.h"
namespace enjoyc
{
	namespace net
	{
		int make_socket_nonblocking(int fd)
		{
			unsigned long on = 1;
			return ioctlsocket(FD_HANDLE(fd), FIONBIO, &on);
		}

		int make_socket_resue(int fd)
		{
			int on = 1;
			return ::setsockopt(FD_HANDLE(fd), SOL_SOCKET, (SO_REUSEADDR), 
						(char*)&on, sizeof(on));
		}

		int get_socket_error_syscall(int fd)
		{
			int error;
			socklen_t errorsz = sizeof(int);
			getsockopt(FD_HANDLE(fd), SOL_SOCKET, SO_ERROR, (char*)&error, &errorsz);
			return error;
		}

	}//net
}//enjoyc




