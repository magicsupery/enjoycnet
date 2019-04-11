#pragma once

#include "tcp_session.h"
#include "../../abstract/client_interface.h"
#include "../../abstract/session_handler.h"

namespace enjoyc
{
	namespace net
	{
		class TcpClient: public std::enable_shared_from_this<TcpClient>,
						public ClientImplInterface
		{
			public:
				TcpClient() = default;
				~TcpClient() = default;
				
			public:
				virtual boost_ec connect(Endpoint ep, Scheduler* sche, 
						OptionPtr ptr, SessionHandlerPtr handler_ptr) override;
				virtual void shutdown() override;
				virtual OptionPtr get_option() override;
				virtual SessionEntry get_session() override;

			private:
				Scheduler* sche_;
				OptionPtr option_ptr_;
				SessionHandlerPtr handler_ptr_;
				Endpoint local_addr_;
				std::unique_ptr<TcpSession> session_ptr_;
				TcpSocketPtr socket_ptr_;
		};
	}
}

