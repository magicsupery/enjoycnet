#pragma once
#include <memory>

namespace enjoyc
{
	namespace net
	{

		class SessionEntry;
		class SessionHandler
		{
			public:
				virtual ~SessionHandler(){};
				virtual size_t parse_message(SessionEntry session_entry, const char* data, size_t data_len) = 0;
				virtual std::shared_ptr<SessionHandler> get_copy() = 0;
		};

		using SessionHandlerPtr = std::shared_ptr<SessionHandler>;
	}
}
