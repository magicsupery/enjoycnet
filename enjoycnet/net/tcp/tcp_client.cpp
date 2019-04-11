#include "tcp_client.h"

namespace enjoyc
{
	namespace net
	{

		boost_ec TcpClient::connect(Endpoint ep, Scheduler* sche, OptionPtr ptr, SessionHandlerPtr handler_ptr)
		{
			return	boost_ec();
		}

		void TcpClient::shutdown()
		{
			
		}

		OptionPtr TcpClient::get_option()
		{
			return	OptionPtr();
		}

		SessionEntry TcpClient::get_session()
		{
			return	SessionEntry();
		}

	}
}

