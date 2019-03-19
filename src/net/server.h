#pragma once
#include "../abstract/server_interface.h"
#include "../abstract/endpoint.h"
#include "../abstract/session_handler.h"

namespace enjoyc
{
	namespace net
	{
		
		using ServerImpl = std::shared_ptr<ServerImplInterface>;
		using OptionPtr = std::shared_ptr<Option>;
		
		template<typename ProtoT, typename HandlerT>
		class Server
		{
			public:
				Server(Endpoint ep):
					local_addr_(ep),
					option_ptr_(new Option),
					proto_(ProtoT::instance()),
					handler_ptr_(std::static_pointer_cast<SessionHandler>(std::make_shared<HandlerT>())){}
				~Server(){};

				boost_ec start()
				{
					impl_ = proto_->create_server_impl();
					boost_ec ec;
					ec = impl_->start(local_addr_, option_ptr_, handler_ptr_);
					return ec;
				}
				void shutdown();
				
				OptionPtr get_option();

			private:
				ServerImpl impl_;
				Endpoint local_addr_;
				OptionPtr option_ptr_;
				ProtoT* proto_;
				SessionHandlerPtr handler_ptr_;
		};
	}
}
