#include "tcp_server.h"
#include <glog/logging.h>

namespace enjoyc
{
	namespace net
	{
		using reuse_port = boost::asio::detail::socket_option::boolean<SOL_SOCKET, SO_REUSEPORT>;
		boost_ec TcpServer::start(Endpoint ep, Scheduler* sche, 
				OptionPtr ptr, SessionHandlerPtr handler_ptr)
		{
			sche_ = sche;
			option_ptr_ = ptr;
			handler_ptr_ = handler_ptr;
			local_addr_ = ep;
			acceptor_ptr_.reset(new boost::asio::ip::tcp::acceptor(get_tcp_io_service()));	
			try
			{
				acceptor_ptr_->open(boost::asio::ip::tcp::endpoint(ep).protocol());

				acceptor_ptr_->set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
				acceptor_ptr_->set_option(boost::asio::ip::tcp::socket::keep_alive(true));
				acceptor_ptr_->set_option(boost::asio::ip::tcp::no_delay(true));
				acceptor_ptr_->set_option(reuse_port(true));

				acceptor_ptr_->bind(ep);
				acceptor_ptr_->listen(option_ptr_->net_.listen_backlog_);
				
			}catch(boost::system::system_error& e)
			{
				return e.code();
			}
			
			go co_scheduler(sche_)[=]()
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
			for(;;)
			{
				DLOG(INFO) << __FUNCTION__ ;
				TcpSocketPtr socket_ptr = std::make_shared<boost::asio::ip::tcp::socket>
					(get_tcp_io_service());
				boost_ec ec;
				acceptor_ptr_->accept(*socket_ptr, ec);
				if(ec)
				{
					LOG(ERROR) << __FUNCTION__ << this << " accpet wrong " << ec.message();
					this->shutdown();
					return;
				}
				
				auto new_handler_ptr = handler_ptr_->get_copy();
								
				auto tcp_session_ptr = std::make_shared<TcpSession>(sche_, socket_ptr, option_ptr_, new_handler_ptr, local_addr_);
				tcp_session_ptr->start();
				option_ptr_->cb_.accept_cb_(tcp_session_ptr);

			}
		}

	}
}

