#pragma once
#include <stdlib.h>
namespace enjoyc
{
	namespace net
	{
		class SessionEntry;
		class Parser
		{
			public:
				Parser() = default;
				virtual ~Parser(){}
				virtual void parse_message(SessionEntry session_entry, const char* data, size_t data_len) = 0;
		};
	}
}
