#pragma once
#ifdef __linux__
	#include <sys/socket.h>
#elif _WIN32
	#include <ws2tcpip.h>
#endif


namespace enjoyc
{
	namespace net
	{
		int get_socket(int family, int type, int protocol);
		int close_socket(int fd);
		int make_socket_nonblocking(int fd);
		int make_socket_resue(int fd);
	}//net
}//enjoyc



