#pragma once
#include "../../abstract/proto.h"

namespace enjoyc
{
	namespace net
	{
		class Tcp: public Proto
		{
			public:
				Tcp() = default;
				virtual ~Tcp() = default;
			public:
				virtual std::shared_ptr<ServerImplInterface> create_server_impl() override;
				virtual std::shared_ptr<ClientImplInterface> create_client_impl() override;
				static Proto* instance() ;
		};
	}
}
