#include "proto.h"

namespace enjoyc
{
	namespace net
	{
		int Proto::type	() const
		{
			switch(proto_type_)
			{
				case ProtoType::tcp:
					return boost::asio::ip::tcp::v4().type();
				case ProtoType::udp:
					return boost::asio::ip::udp::v4().type();
				case ProtoType::http:
					break;
				case ProtoType::unkown:
					break;
			}
			return 0;
		}

		int Proto::protocol() const
		{
			switch(proto_type_)
			{
				case ProtoType::tcp:
					return boost::asio::ip::tcp::v4().protocol();
				case ProtoType::udp:
					return boost::asio::ip::udp::v4().protocol();
				case ProtoType::http:
					break;
				case ProtoType::unkown:
					break;
			}
			return 0;
		}


	} // net
} // enjoyc
