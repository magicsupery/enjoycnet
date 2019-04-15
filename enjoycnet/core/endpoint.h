#pragma once
#include <string>
#include <arpa/inet.h>
#include <ostream>
#include <cstring>

namespace enjoyc
{
	namespace net
	{
		class Endpoint
		{
			public:
				Endpoint() = default;
				Endpoint(std::string const& ip, uint16_t port)
					:ip_(ip),
					port_(port)
				{
					addr_.sin_family = AF_INET;
					inet_pton(AF_INET, ip_.data(), &(addr_.sin_addr));
					addr_.sin_port = htons(port_);
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
				
				void calc_ipport_from_addr()
				{
					port_ = ntohs(addr_.sin_port);

					char str[INET_ADDRSTRLEN];
					inet_ntop(AF_INET, &addr_.sin_addr, str, sizeof(str));
					ip_ = str;
					
				}

				friend std::ostream& operator << (std::ostream& os, Endpoint const& ep)
				{
					os << "endpoint[ " << ep.ip_ << " , " << ep.port_ <<" ]";
					return os;
				}

			public:
				sockaddr_in addr_;

				std::string ip_;
				uint16_t port_;
		};
	};
}
