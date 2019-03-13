#include "server.h"
#include "../abstract/error.h"
#include "tcp/tcp.h"

namespace enjoyc
{
	namespace net
	{

		Server::Server()
			:local_addr_(nullptr),
			option_ptr_(new Option)
		{

		}

		Server::~Server()
		{
			shutdown();
		}

		boost_ec Server::start(std::string const& url, OptionPtr option_ptr)
		{
			boost_ec ec;
			local_addr_ = std::make_shared<Endpoint>(Endpoint::from_string(url, ec));
			
			Proto* proto_ptr = nullptr;	
			if(local_addr_->get_proto_type() == ProtoType::tcp)
			{
				proto_ptr = Tcp::instance();
			}
			else
			{
				ec = MakeNetworkErrorCode(ErrorCode::ec_not_support_ptotocol);
				return ec;
			}
			
			option_ptr_ = option_ptr;
			
			impl_ = proto_ptr->create_server_impl();
			
			ec = impl_->start(*local_addr_, option_ptr_);
			return ec;
		}

		void Server::shutdown()
		{
			if(impl_)
				impl_->shutdown();
		}

		OptionPtr Server::get_option()
		{
			return	option_ptr_;
		}


	}
}
