#pragma once
#include <memory>
#include <rapidhttp/rapidhttp.h>
#include <functional>
#include <unordered_map>
namespace enjoyc
{
	namespace net
	{

		using HttpRequest = rapidhttp::HttpDocumentRef;
		using HttpResponse = rapidhttp::HttpDocument;
		using HttpCb = std::function<bool(HttpRequest&, HttpResponse&)>;
		using MethodHandlerMap = std::unordered_map<std::string, HttpCb>;
		using HandlerMap = std::unordered_map<std::string, MethodHandlerMap>;
		class HttpRouter
		{
			public:
				HttpRouter();
				~HttpRouter() = default;
			
			public:
				void register_handler(std::string const& method, std::string const& uri, HttpCb cb);
				
				void route(HttpRequest& req, HttpResponse& res);
			private:
				HandlerMap handler_map_;
		};

		using HttpRouterPtr = std::shared_ptr<HttpRouter>;
	}
}
