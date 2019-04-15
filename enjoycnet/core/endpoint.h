#pragma once
#include <string>
#include <arpa/inet.h>

namespace enjoyc
{
	namespace net
	{
		struct Endpoint
		{
			public:
				Endpoint(std::string const& addr, uint16_t port)
				{
					addr_.sin_family = AF_INET;
					inet_pton(AF_INET, addr.data(), &(addr_.sin_addr));
					addr_.sin_port = port;
				}

			public:
				struct sockaddr* sockaddr()
				{
					return (struct sockaddr*)&addr_;
				}	
				
				
				socklen_t sockadr_size()
				{
					return sizeof(addr_);
				}		

				sockaddr_in addr_;
		};
	};
}
