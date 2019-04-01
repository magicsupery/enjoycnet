#pragma once
#include <memory>
#include "endpoint.h"
#include "session.h"
#include "session_handler.h"
#include "../option/option.h"
#include <libgo/coroutine.h>

namespace enjoyc
{
	namespace net
	{
		class Option;
		using OptionPtr = std::shared_ptr<Option>;
		using Scheduler = co::Scheduler;
		class ServerImplInterface
		{
			public:
				virtual ~ServerImplInterface(){}
				virtual boost_ec start(Endpoint ep, Scheduler* sche, 
						OptionPtr ptr, SessionHandlerPtr handler_ptr) = 0;
				virtual void shutdown() = 0;
				virtual OptionPtr get_option() = 0; 
				virtual SessionEntry get_session() = 0;
		};
	}
}
