#include "tcp.h"
#include "tcp_server.h"


namespace enjoyc
{
	namespace net
	{

		std::shared_ptr<ServerImplInterface> Tcp::create_server_impl()
		{
			return std::static_pointer_cast<ServerImplInterface>(
					std::make_shared<TcpServer>());
		}

		std::shared_ptr<ClientImplInterface> Tcp::create_client_impl()
		{
			return nullptr;
		}

		Tcp* Tcp::instance()
		{
			static Tcp tcp;
			return	&tcp;
		}

	}
}

