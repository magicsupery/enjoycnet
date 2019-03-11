#include "endpoint.h"
#include "../error.h"
#include <boost/regex.hpp>
#include <arpa/inet.h>
#include <stdlib.h>

namespace enjoyc
{
	namespace net
	{
		char const* proto_type_2str[] = {
			"unkown",
			"tcp",
			"udp",
			"http",
		};

		ProtoType str2proto(std::string const& s)
		{
			static int n = sizeof(proto_type_2str) / sizeof(char const*);
			for (int i = 0; i < n; ++i)
				if (strcmp(s.c_str(), proto_type_2str[i]) == 0)
					return ProtoType(i);

			return ProtoType::unkown;
		}

		std::string proto2str(ProtoType proto)
		{
			static int n = sizeof(proto_type_2str) / sizeof(char const*);
			if ((int)proto >= n)
				return proto_type_2str[0];

			return proto_type_2str[(int)proto];
		}

		Endpoint Endpoint::from_string(std::string const& url, boost_ec& ec)
		{
			if(url.empty())
			{
				ec = MakeNetworkErrorCode(ErrorCode::ec_url_parse_wrong);
				return Endpoint();
			}

			static boost::regex re("((.*)://)?([^:/]+)(:(\\d+))?(/.*)?");
			boost::smatch result;
			bool ok = boost::regex_match(url, result, re);
			if(!ok)
			{
				ec = MakeNetworkErrorCode(ErrorCode::ec_url_parse_wrong);
				return Endpoint();
			}

			std::string host = result[3].str();	
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
			
			auto proto_type = str2proto(result[2].str());
			if(proto_type == ProtoType::unkown)
			{
				ec = MakeNetworkErrorCode(ErrorCode::ec_not_support_ptotocol);
				return Endpoint();
			}
			Endpoint ep(addr, atoi(result[5].str().c_str()));

			ep.set_proto_type(proto_type);
			ep.set_path(result[6].str());
			return ep;
		}

		std::string Endpoint::to_string(boost_ec &ec) const
		{
			std::string url;
			if (proto_type_ != ProtoType::unkown) {
				url += proto2str(proto_type_) + "://";
			}
			url += address().to_string(ec);
			if (ec) return "";

			url += ":";
			url += std::to_string(port());
			url += get_path();
			return url;
			return	"";
		}
	}//net
}// enjoyc



