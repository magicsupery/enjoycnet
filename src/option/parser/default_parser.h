#pragma once
#include "parser.h"

namespace enjoyc
{
	namespace net
	{
		class DefaultParser: public Parser
		{
			public:
				DefaultParser() = default;
				~DefaultParser() = default;

			public:
				virtual void parse_message(
						SessionEntry session_entry, const char* data, size_t data_len) override;
		};
	}
}
