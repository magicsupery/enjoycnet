#include "endpoint.h"
#include "error.h"
#include <boost/regex.hpp>
#include <arpa/inet.h>
#include <stdlib.h>

namespace enjoyc
{
	namespace net
	{

		Endpoint Endpoint::generate(std::string host, int port, boost_ec& ec)
		{
			if(host.empty())
			{
				ec = MakeNetworkErrorCode(ErrorCode::ec_url_parse_wrong);
				return Endpoint();
			}

			boost::asio::ip::address addr = boost::asio::ip::address::from_string(host, ec);
			if(ec)
			{
				// need dns 
				hostent* h = gethostbyname(host.c_str());
				if (!h || !*h->h_addr_list) {
					ec = MakeNetworkErrorCode(ErrorCode::ec_dns_not_found);
					return Endpoint();
				}

				char buf[128];
				host = inet_ntop(h->h_addrtype, *h->h_addr_list, buf, sizeof(buf));
				addr = ::boost::asio::ip::address::from_string(host, ec);
				if (ec)
					return Endpoint();
			}

			Endpoint ep(addr, port);
			return ep;
		}
	}//net
}// enjoyc



