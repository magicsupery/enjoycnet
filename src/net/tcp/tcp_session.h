#pragma once
#include "../../abstract/session.h"
#include "../../option/option.h"
#include <libgo/coroutine.h>

namespace enjoyc
{
	namespace net
	{
		using TcpSocket = boost::asio::ip::tcp::socket;
		class TcpSession
			:public std::enable_shared_from_this<TcpSession>,
			 public SessionBase
		{
			public:
				explicit TcpSession(TcpSocket& tcp_socket, OptionPtr option_ptr,
						Endpoint const& local_addr, Endpoint const& remote_addr);

			public:
				virtual void start() override;
				virtual void send(Buffer&& buf) override;
				virtual void shutdown() override;
				virtual Endpoint local_addr() override;
				virtual Endpoint remote_addr() override;
			
			private:
				void go_send();
				void go_receive();
			private:
				TcpSocket socket_;
				
				co::atomic_t<bool> send_flag_;
				co::atomic_t<bool> receive_flag_;
				
		};
			
	}
}
