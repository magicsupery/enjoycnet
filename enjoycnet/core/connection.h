#pragma once

#include "socket.h"

namespace enjoyc
{
	namespace net
	{
		template <typename Proto>
		class Connection
		{
			public:
				Connection(Socket<Proto> socket)
				:socket_(socket)
				{

				}

			private:
				Socket<Proto> socket_;
		};
	}
}
