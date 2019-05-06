#pragma once
#include <memory>

#include "socket.h"
#include "endpoint.h"
#include "utils.h"

namespace enjoyc
{
	namespace net
	{
		template <typename Proto>
		class Acceptor: public NonCopyable
		{

			using AcceptCallback = std::function<void (Socket<Proto> const&)>;
			public:
				Acceptor(AcceptCallback && callback)
					:callback_(callback)
				{
						
				}

				~Acceptor() = default;

			public:
				bool listen(Endpoint& ep, int backlog)
				{
					if(socket_.listen(ep, backlog) < 0)
						return false;
					
					return true;
				}

				bool accept()
				{
					Socket<Proto> con_socket;
					if(socket_.accept(con_socket) < 0)
						return false;

					callback_(con_socket);
					return true;
				}

			private:
				Socket<Proto> socket_;
				AcceptCallback callback_;
		};
	};
}
