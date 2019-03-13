#include "tcp_session.h"
#include <glog/logging.h>


namespace enjoyc
{
	namespace net
	{

		TcpSession::TcpSession(TcpSocketPtr tcp_socket_ptr, OptionPtr option_ptr, 
				Endpoint const& local_addr)
			:socket_ptr_(tcp_socket_ptr),
			 local_addr_(local_addr),
			 remote_addr_(tcp_socket_ptr->remote_endpoint().address(),
					 tcp_socket_ptr->remote_endpoint().port(), 
					 local_addr.get_proto_type(), local_addr.get_path()),
			 option_session_ptr_(std::unique_ptr<
					 OptionSessionDefine>(new OptionSessionDefine(option_ptr->define_data_)))
		{
			
		}

		void TcpSession::start()
		{
			go [=]()
			{
				this->go_send();
			};

			go [=]()
			{
				this->go_receive();
			};
		}

		void TcpSession::send(Buffer&& buf)
		{

		}

		void TcpSession::shutdown()
		{
		}

		Endpoint TcpSession::local_addr()
		{
			return	local_addr_;
		}

		Endpoint TcpSession::remote_addr()
		{
			return	remote_addr_;
		}

		void TcpSession::go_send()
		{
			DLOG(INFO) << __FUNCTION__ ;
		}

		void TcpSession::go_receive()
		{
			DLOG(INFO) << __FUNCTION__ ;
		}

		tcp_io_service& get_tcp_io_service()
		{
			static tcp_io_service io_service;
			return	io_service;
		}

	}
}

