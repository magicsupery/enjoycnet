#pragma once
#include "../abstract/server_interface.h"
#include "../abstract/endpoint.h"

namespace enjoyc
{
	namespace net
	{

		using ServerImpl = std::shared_ptr<ServerImplInterface>;
		template<typename ProtoT, typename OptionT>
		class Server
		{
			using OptionPtr = std::shared_ptr<OptionT>;
			public:
				Server(Endpoint ep):
					local_addr_(ep),
					option_ptr_(new OptionT),
					proto_(ProtoT::instance()){}

				~Server(){};

				boost_ec start()
				{
					impl_ = proto_->create_server_impl();
					boost_ec ec;
					ec = impl_->start(local_addr_, option_ptr_);
					return ec;
				}
				void shutdown();
				
				OptionPtr get_option();

			private:
				ServerImpl impl_;
				Endpoint local_addr_;
				OptionPtr option_ptr_;
				ProtoT* proto_;

		};
	}
}
