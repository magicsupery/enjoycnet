#pragma once
#include "../abstract/server_interface.h"
#include "../abstract/endpoint.h"

namespace enjoyc
{
	namespace net
	{

		using ServerImpl = std::shared_ptr<ServerImplInterface>;
		using EndPointPtr = std::shared_ptr<Endpoint>;
		using OptionPtr = std::shared_ptr<Option>;
		class Server
		{
			public:
				Server();
				~Server();

				boost_ec start(std::string const& url, OptionPtr option_ptr);
				void shutdown();
				
				OptionPtr get_option();

			private:
				ServerImpl impl_;
				EndPointPtr local_addr_;
				OptionPtr option_ptr_;

		};
	}
}
