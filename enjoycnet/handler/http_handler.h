#pragma once
#include "../abstract/session_handler.h"
#include "http_router.h"

namespace enjoyc
{
	namespace net
	{

		class HttpHandler: public SessionHandler
		{
			public:
				HttpHandler();
				~HttpHandler() = default;

			public:
				virtual size_t parse_message(
						SessionEntry session_entry, const char* data, size_t data_len) override;

				virtual SessionHandlerPtr get_copy() override;
			
			public:
				void set_router_ptr(HttpRouterPtr router_ptr)
				{
					router_ptr_ = router_ptr;
				}

			private:
				rapidhttp::HttpDocumentRef doc_;

				HttpRouterPtr router_ptr_;
		};
	}
}
