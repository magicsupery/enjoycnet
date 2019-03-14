#include "default_parser.h"
#include "../../abstract/session.h"

namespace enjoyc
{
	namespace net
	{

		void DefaultParser::parse_message(
				SessionEntry session_entry, const char* data, size_t data_len)
		{
			session_entry->send(data, data_len);

			session_entry->shutdown();
		}
	}
}


