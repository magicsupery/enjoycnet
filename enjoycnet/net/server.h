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
				Server(Endpoint ep, Scheduler* sche, Option* option):
					local_addr_(ep),
					sche_(sche),
					option_ptr_(option),
					proto_(ProtoT::instance()),
					handler_ptr_(std::static_pointer_cast<SessionHandler>(std::make_shared<HandlerT>())){}
				~Server(){};

				boost_ec start()
				{
					impl_ = proto_->create_server_impl();
					boost_ec ec;
					ec = impl_->start(local_addr_, sche_, option_ptr_, handler_ptr_);
					return ec;
				}

				SessionHandlerPtr get_handler()
				{
					return handler_ptr_;
				}

			private:
				ServerImpl impl_;
				Endpoint local_addr_;
				Scheduler* sche_;
				OptionPtr option_ptr_;
				ProtoT* proto_;
				SessionHandlerPtr handler_ptr_;
		};
	}
}
