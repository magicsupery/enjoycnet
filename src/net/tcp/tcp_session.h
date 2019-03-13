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
						Endpoint const& local_addr);

				~TcpSession();
			public:
				virtual void start() override;
				virtual void send(Buffer&& buf) override;
				virtual void send(const char* data, size_t len) override;
				virtual void shutdown() override;
				virtual Endpoint local_addr() override;
				virtual Endpoint remote_addr() override;
			
			private:
				void go_send();
				void go_receive();

				void on_close_send();
				void on_close_receive();

			private:
				TcpSocketPtr socket_ptr_;
				
				Endpoint local_addr_;
				Endpoint remote_addr_;	
				
				OptionSessionPtr option_session_ptr_;

				co::atomic_t<bool> send_flag_;
				co::atomic_t<bool> receive_flag_;
				co::atomic_t<bool> closed_flag_;

				MsgChan  msg_chan_;
				int max_read_buf_num_;
		};
			
		using tcp_io_service = boost::asio::io_service;
		tcp_io_service& get_tcp_io_service();
	}
}
