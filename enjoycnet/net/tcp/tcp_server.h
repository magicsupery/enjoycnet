#pragma once
#include "tcp_session.h"
#include "../../abstract/server_interface.h"
#include "../../abstract/session_handler.h"

namespace enjoyc
{
	namespace net
	{
		using AcceptorPtr = std::unique_ptr<boost::asio::ip::tcp::acceptor>;
		class TcpServer: public std::enable_shared_from_this<TcpServer>,
						public ServerImplInterface
		{
			public:
				TcpServer() = default;
				~TcpServer() = default;
			public:
				// from ImplInterface
				virtual boost_ec start(Endpoint ep, OptionPtr ptr, SessionHandlerPtr handler_ptr) override;
				virtual void shutdown() override;
				virtual OptionPtr get_option() override;
				virtual SessionEntry get_session() override;
			
			private:
				virtual void go_accept();

			private:
				OptionPtr option_ptr_;
				SessionHandlerPtr handler_ptr_;
				Endpoint local_addr_;
				AcceptorPtr acceptor_ptr_;
				
		};
	}
}
