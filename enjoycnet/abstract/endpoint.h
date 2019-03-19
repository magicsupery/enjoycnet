#pragma once
#include <boost/asio.hpp>
#include "proto.h"
namespace enjoyc
{
	namespace net
	{
		using namespace boost::asio;
		using BaseType = ip::basic_endpoint<Proto>;
		using boost_ec = boost::system::error_code;
		class Endpoint: public BaseType
		{
			public:
				Endpoint() = default;
				Endpoint(const Endpoint&) = default;
				Endpoint(Endpoint&&) = default;
				Endpoint& operator=(const Endpoint&) = default;
				Endpoint& operator=(Endpoint&&) = default;

			public:
				explicit Endpoint(boost::asio::ip::address const& addr, int host)
				:BaseType(addr, host){}

			public:
				operator ::boost::asio::ip::tcp::endpoint() const
				{
					return ::boost::asio::ip::tcp::endpoint(address(), port()); 
				}

				operator ::boost::asio::ip::udp::endpoint() const
				{
					return ::boost::asio::ip::udp::endpoint(address(), port()); 
				}

			public:
				static Endpoint generate(std::string host, int port, boost_ec& ec);
		};
	};
};

