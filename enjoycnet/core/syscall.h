#pragma once

#ifdef __linux__
#include <sys/socket.h>
#define FD_DESC(handle) handle
#define FD_HANDLE(fd) fd
#define CHECK_ERROR(err, err_name) ((err) == err_name)
#elif _WIN32
#ifndef _WINSOCK2API_

// Prevent inclusion of winsock.h
#ifdef _WINSOCKAPI_
#error Header winsock.h is included unexpectedly.
#endif

// NOTE: If you use Windows Platform SDK, you should enable following definition:
// #define USING_WIN_PSDK

#if !defined(WIN32_LEAN_AND_MEAN) && (_WIN32_WINNT >= 0x0400) && !defined(USING_WIN_PSDK)
#include <windows.h>
#else
#include <winsock2.h>
#endif

#endif//_WINSOCK2API_

#include <io.h>
#include <WS2tcpip.h>
#include <stdint.h>
typedef SSIZE_T ssize_t;
#define FD_DESC(handle) _open_osfhandle ((handle), 0)
#define FD_HANDLE(fd) _get_osfhandle(fd)
#define CHECK_ERROR(err, err_name) ((err) == WSA##err_name)
#endif

#include <iostream>
namespace enjoyc
{
	namespace net
	{
		int make_socket_nonblocking(int fd);
		int make_socket_resue(int fd);
		int get_socket_error_syscall(int fd);

		inline int socket_syscall(int family, int type, int protocol)
		{
			return FD_DESC(socket(family, type, protocol));
		}
	
		inline int bind_syscall(int fd, struct sockaddr* addr, socklen_t addrlen)
		{
			return bind(FD_HANDLE(fd), addr, addrlen);
		}

		inline int listen_syscall(int fd, int backlog)
		{
			return listen(FD_HANDLE(fd), backlog);
		}

		inline int accept_syscall(int fd, struct sockaddr* addr, socklen_t *addren)
		{
			return FD_DESC(accept(FD_HANDLE(fd), addr, addren));
		}
		inline int connect_syscall(int fd, struct sockaddr* addr, socklen_t addrlen)
		{
			return  connect(FD_HANDLE(fd), addr, addrlen);
		}

		inline ssize_t send_syscall(int fd, const char* data, size_t len, ssize_t& write_size)
		{
#ifdef __linux__
			write_size = ::send(fd, (void*)data, len, MSG_NOSIGNAL);
#elif _WIN32
			write_size = ::send(fd, (char*)data, len, 0);
#endif
			return write_size;
		}
		inline ssize_t recv_syscall(int fd, const char* data, size_t len, ssize_t& read_size)
		{
#ifdef __linux__
			read_size = ::recv(fd, (void*)data, len, MSG_NOSIGNAL);
#elif _WIN32
			read_size = ::recv(fd, (char*)data, len, 0);
#endif

			return read_size;
		}

		inline int close_syscall(int fd)
		{
#ifdef _WIN32
			return closesocket(fd);
#else
			return close(fd);
#endif
		}

		inline int sys_errno()
		{
#ifdef _WIN32
			return WSAGetLastError();
#else
			return errno;
#endif
		}

	}//net
}//enjoyc



