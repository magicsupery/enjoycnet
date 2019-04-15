#pragma once

#include "endpoint.h"

namespace enjoyc
{
	namespace net
	{
		template<typename Proto>
		class Socket
		{
			public:
				Socket() = default;
				~Socket() = default;

				Socket(Socket const&) = default;
				Socket& operator=(Socket const&) = delete;

			public:
				int listen(Endpoint & ep, uint32_t backlog)
				{
					return proto_.listen(ep, backlog);
				}

				int accept(Socket<Proto>& socket)
				{
					return proto_.accept(socket.proto_);
				}

				const Endpoint& host_addr() const
				{
					return proto_.host_addr();
				}

				const Endpoint& remote_addr() const
				{
					return proto_.remote_addr();
				}

			private:
				Proto proto_;
		};
	}
}
