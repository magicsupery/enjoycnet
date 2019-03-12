#pragma once
#include <memory>
#include "endpoint.h"
#include "session.h"

namespace enjoyc
{
	namespace net
	{
		class Option;
		using OptionPtr = std::shared_ptr<Option>;
		class ClientInterface
		{
			virtual ~ClientInterface(){}
			virtual void connect(Endpoint ep) = 0;
			virtual void shutdown() = 0;
			virtual OptionPtr get_option() = 0; 
			virtual SessionEntry get_session() = 0;
		};
	}
}
