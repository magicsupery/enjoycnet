#pragma once
#include "../abstract/session_handler.h"
#include <rapidhttp/rapidhttp.h>
#include <functional>
#include <unordered_map>
namespace enjoyc
{
	namespace net
	{
		using HttpCb = std::function<bool(rapidhttp::HttpDocument&, rapidhttp::HttpDocument&)>;
		using MethodHandlerMap = std::unordered_map<std::string, HttpCb>;
		using HandlerMap = std::unordered_map<std::string, MethodHandlerMap>;
		class HttpHandler: public SessionHandler
		{
			public:
				HttpHandler();
				~HttpHandler() = default;

			public:
				virtual void parse_message(
						SessionEntry session_entry, const char* data, size_t data_len) override;

				virtual SessionHandlerPtr get_copy() override;
			
			public:
				void register_handler(std::string const& method, std::string const& uri, HttpCb cb);
			private:
				rapidhttp::HttpDocument doc_;
				HandlerMap handler_map_;
		};
	}
}
