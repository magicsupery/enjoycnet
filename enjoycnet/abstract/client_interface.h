#pragma once
#include <memory>
#include "endpoint.h"
#include "session.h"
#include "session_handler.h"
#include <libgo/coroutine.h>

namespace enjoyc
{
	namespace net
	{
		class Option;
		using OptionPtr = std::shared_ptr<Option>;
		using Scheduler = co::Scheduler;
		class ClientImplInterface
		{
			public:
				virtual ~ClientImplInterface(){}
				virtual boost_ec connect(Endpoint ep, Scheduler* sche, 
						OptionPtr ptr, SessionHandlerPtr handler_ptr) = 0;
				virtual void shutdown() = 0;
				virtual OptionPtr get_option() = 0;
				virtual SessionEntry get_session() = 0;
		};
	}
}
