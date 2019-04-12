#pragma once
#include <memory>

#include "socket.h"
#include "endpoint.h"

namespace enjoyc
{
	namespace net
	{
		template <typename Proto>
		class Acceptor
		{

			using AcceptCallback = std::function<void (Socket<Proto> )>;
			public:
				Acceptor(Endpoint const& ep, AcceptCallback const& callback)
					:host_addr_(ep),
					callback_(callback)
				{
						
				}

				~Acceptor() = default;

			public:
				bool listen(int backlog)
				{
					if(not socket_.listen(host_addr_, backlog))
						return false;
					
					return true;
				}

				bool accept()
				{
					Socket<Proto> con_socket;
					if(not socket_.accept(con_socket))
						return false;

					callback_(con_socket);
					return true;
				}

			private:
				Endpoint host_addr_;
				Socket<Proto> socket_;
				AcceptCallback callback_;
		};
	};
}
