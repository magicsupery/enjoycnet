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
		class ServerInterface
		{
			virtual ~ServerInterface(){}
			virtual void start(Endpoint ep) = 0;
			virtual void shutdown() = 0;
			virtual OptionPtr get_option() = 0; 
			virtual SessionEntry get_session() = 0;
		};
	}
}
