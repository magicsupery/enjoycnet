#pragma once

#include <memory>
#include <rapidhttp/rapidhttp.h>
#include <glog/logging.h>

namespace enjoyc
{
	namespace net
	{
		using HttpRequest = rapidhttp::HttpDocumentRef;
		using HttpResponse = rapidhttp::HttpDocumentRef;
		class HttpCodec
		{
			public:
				using ReadCallback = std::function<void(HttpRequest&)>;
			public:
				HttpCodec(ReadCallback&& read_callback)
					:request_(rapidhttp::Request),
					read_callback_(read_callback)
			{

			}

				uint32_t parse_message(const char* data, uint32_t len)
				{

					auto handled = request_.PartailParse(data, len);
					DLOG(INFO) << __FUNCTION__ << " " << this << " len " << len << " handled " << handled;
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
