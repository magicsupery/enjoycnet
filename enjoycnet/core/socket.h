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
				int listen(Endpoint const& ep, uint32_t backlog)
				{
					return proto_.listen(ep, backlog);
				}

				bool accept(Socket<Proto>& socket)
				{
					return proto_.accept(socket.proto_);
				}

			private:
				Proto proto_;
		};
	}
}
