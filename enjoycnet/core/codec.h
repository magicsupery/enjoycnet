#pragma once

#include <memory>
#include <rapidhttp/rapidhttp.h>
#include <iostream>

namespace enjoyc
{
	namespace net
	{
		using HttpRequest = rapidhttp::HttpDocumentRef;
		using HttpResponse = rapidhttp::HttpDocumentRef;
		class HttpCodec
		{
			public:
				using ReadCallback = std::function<void(HttpRequest const&)>;
			public:
				HttpCodec(ReadCallback&& read_callback)
					:request_(rapidhttp::Request),
					read_callback_(read_callback)
			{

			}

				uint32_t parse_message(const char* data, uint32_t len)
				{

					std::cout << "parse message len  " << len << std::endl;
					auto handled = request_.PartailParse(data, len);
					std::cout << "parse message handled " << handled << std::endl;
					if(request_.ParseDone())
					{
						read_callback_(request_);
						return handled;
					}

					return 0;
				}

			private:
				HttpRequest request_;
				ReadCallback read_callback_;
		};
	}
}
