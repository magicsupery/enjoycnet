#pragma once
#include <memory>
#include "endpoint.h"
#include "session.h"
#include "../option/option.h"

namespace enjoyc
{
	namespace net
	{
		class Option;
		using OptionPtr = std::shared_ptr<Option>;
		class ServerImplInterface
		{
			public:
				virtual ~ServerImplInterface(){}
				virtual boost_ec start(Endpoint ep, OptionPtr ptr) = 0;
				virtual void shutdown() = 0;
				virtual OptionPtr get_option() = 0; 
				virtual SessionEntry get_session() = 0;
		};
	}
}
