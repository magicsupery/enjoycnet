#include "tcp_server.h"
#include <glog/logging.h>

namespace enjoyc
{
	namespace net
	{
		
		boost_ec TcpServer::start(Endpoint ep, OptionPtr ptr)
		{
			option_ptr_ = ptr;
			local_addr_ = ep;
			acceptor_ptr_.reset(new boost::asio::ip::tcp::acceptor(get_tcp_io_service()));	
			try
			{
				acceptor_ptr_->open(boost::asio::ip::tcp::endpoint(ep).protocol());

				acceptor_ptr_->set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
				acceptor_ptr_->set_option(boost::asio::ip::tcp::socket::keep_alive(true));
				acceptor_ptr_->set_option(boost::asio::ip::tcp::no_delay(true));

				acceptor_ptr_->bind(ep);
				acceptor_ptr_->listen(option_ptr_->net_.listen_backlog_);
				
			}catch(boost::system::system_error& e)
			{
				return e.code();
			}
			
			go [=]()
			{
				this->go_accept();
			};	

			return boost_ec();
		}

		void TcpServer::shutdown()
		{

		}

		OptionPtr TcpServer::get_option()
		{
			return	option_ptr_;
		}

		SessionEntry TcpServer::get_session()
		{
			return	SessionEntry();
		}

		void TcpServer::go_accept()
		{
			std::vector<std::shared_ptr<TcpSession>> sessions;
			for(;;)
			{
				DLOG(INFO) << __FUNCTION__ ;
				TcpSocketPtr socket_ptr = std::make_shared<boost::asio::ip::tcp::socket>
					(get_tcp_io_service());
				acceptor_ptr_->accept(*socket_ptr);
				
				auto tcp_session_ptr = std::make_shared<TcpSession>(socket_ptr, option_ptr_, local_addr_);
				tcp_session_ptr->start();
				sessions.emplace_back(tcp_session_ptr);
			}
		}

	}
}

