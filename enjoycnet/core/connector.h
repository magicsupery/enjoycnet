#pragma once

#include <memory>
#include "utils.h"
#include "socket.h"

namespace enjoyc
{
	namespace net
	{
		template<typename Proto>
		class Connector: public NonCopyable
		{

			using ConnectCallback = std::function<void (Socket<Proto> const&)>;
			public:
				Connector(ConnectCallback && callback)
					:callback_(callback)
				{
						
				}

			public:
				bool connect(Endpoint & ep)
				{
					Socket<Proto> socket;
					socket.connect(ep);
					callback_(socket);
				}

			private:
				ConnectCallback callback_;

		};
	}//net
}//enjoyc
