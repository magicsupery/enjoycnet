#pragma once

#include <memory>
#include <rapidhttp/rapidhttp.h>
#include <glog/logging.h>

namespace enjoyc
{
	namespace net
	{
		template<typename Req, typename Res>
		class Codec
		{
			public:
				using ReadCallback = std::function<void(Res&)>;

				virtual uint32_t parse_message(const char* data, uint32_t len) = 0;

		};	

		using HttpRequest = rapidhttp::HttpDocumentRef;
		using HttpResponse = rapidhttp::HttpDocumentRef;
		class HttpServerCodec: public Codec<HttpRequest, HttpResponse>
		{
			public:
				HttpServerCodec(ReadCallback&& read_callback)
					:request_(rapidhttp::Request),
					read_callback_(read_callback)
				{

				}

				virtual uint32_t parse_message(const char* data, uint32_t len) override
				{

					auto handled = request_.PartailParse(data, len);
					DLOG(INFO) << __FUNCTION__ << " " << this << " len " << len << " handled " << handled;
					if(request_.ParseDone())
					{

						DLOG(INFO) << __FUNCTION__ << " " << this << " len " << len << " handled " << handled;
						read_callback_(request_);
						return handled;
					}

					return 0;
				}

			private:
				HttpRequest request_;
				ReadCallback read_callback_;
		};

		class HttpClientCodec: public Codec<HttpResponse, HttpRequest>
		{
			public:
				HttpClientCodec(ReadCallback&& read_callback)
					:response_(rapidhttp::Response),
					read_callback_(read_callback)
				{}

				uint32_t parse_message(const char* data, uint32_t len)
				{

					auto handled = response_.PartailParse(data, len);
					DLOG(INFO) << __FUNCTION__ << " " << this << " len " << len << " handled " << handled;
					if(response_.ParseDone())
					{

						DLOG(INFO) << __FUNCTION__ << " " << this << " len " << len << " handled " << handled;
						read_callback_(response_);
						return handled;
					}

					return 0;
				}

			private:
				HttpResponse response_;
				ReadCallback read_callback_;
		};
	}
}
