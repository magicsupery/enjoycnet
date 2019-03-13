#pragma once
#include "../../abstract/session.h"
#include "../../option/option.h"
#include <libgo/coroutine.h>

namespace enjoyc
{
	namespace net
	{
		using TcpSocket = boost::asio::ip::tcp::socket;
		using TcpSocketPtr = std::shared_ptr<TcpSocket>;
		
		using BufferPtr = std::shared_ptr<Buffer>;
		using MsgChan = co_chan<BufferPtr>;
		using OptionSessionPtr = std::unique_ptr<OptionSessionDefine>;
		class TcpSession
			:public std::enable_shared_from_this<TcpSession>,
			 public SessionBase
		{
			public:
				explicit TcpSession(TcpSocketPtr tcp_socket_ptr, OptionPtr option_ptr,
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
				TcpSocketPtr socket_ptr_;
				
				Endpoint local_addr_;
				Endpoint remote_addr_;	
				
				OptionSessionPtr option_session_ptr_;

				co::atomic_t<bool> send_flag_;
				co::atomic_t<bool> receive_flag_;

				MsgChan  msg_chan_;
				
		};
			
		using tcp_io_service = boost::asio::io_service;
		tcp_io_service& get_tcp_io_service();
	}
}
