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
				:BaseType(addr, host),
				 proto_type_(ProtoType::unkown),
				 path_(""){}

				explicit Endpoint(boost::asio::ip::address const& addr, int host,
						ProtoType proto_type, std::string const& path)
				:BaseType(addr, host),
				 proto_type_(proto_type),
				 path_(path){}
				 
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
				ProtoType get_proto_type() const
				{
					return proto_type_;
				}

				void set_proto_type(ProtoType proto)
				{
					proto_type_ = proto;
				}

				std::string const& get_path() const
				{
					return path_;
				}

				void set_path(std::string const& path)
				{
					path_ = path;
				}
			
			public:
				std::string to_string(boost_ec& ec) const;
				static Endpoint from_string(std::string const& url, boost_ec& ec);
			private:
				ProtoType proto_type_;
				std::string path_;

		};
	};
};

