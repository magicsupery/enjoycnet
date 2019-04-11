#pragma once

#include "../abstract/client_interface.h"
#include "../abstract/endpoint.h"
#include "../abstract/session_handler.h"

namespace enjoyc
{
	namespace net
	{

		using ClientImpl = std::shared_ptr<ClientImplInterface>;
		using OptionPtr = std::shared_ptr<Option>;

		template<typename ProtoT, typename HandlerT>
			class Client
			{

				public:
					Client(Endpoint ep, Scheduler* sche, Option* option):
						remote_addr_(ep),
						sche_(sche),
						option_ptr_(option),
						proto_(ProtoT::instance()),
						handler_ptr_(std::static_pointer_cast<SessionHandler>(std::make_shared<HandlerT>())){}
					~Client(){};

					boost_ec start()
					{
						impl_ = proto_->create_client_impl();
						boost_ec ec;
						ec = impl_->connect(remote_addr_, sche_, option_ptr_, handler_ptr_);
						return ec;
					}
					void shutdown();

					OptionPtr get_option();

					SessionHandlerPtr get_handler()
					{
						return handler_ptr_;
					}

				private:
					ClientImpl impl_;
					Endpoint remote_addr_;
					Scheduler* sche_;
					OptionPtr option_ptr_;
					ProtoT* proto_;
					SessionHandlerPtr handler_ptr_;
			};
	}
}
