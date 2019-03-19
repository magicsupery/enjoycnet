#include "http_handler.h"
#include "../abstract/session.h"
#include <glog/logging.h>


namespace enjoyc
{
	namespace net
	{
		
		const std::string method_names[] =
		{
			"GET",
			"POST",
			"PUT",
			"DELETE",
		};

		HttpHandler::HttpHandler()
			:doc_(rapidhttp::Request)
			{
				for(auto name : method_names)
				{
					handler_map_[name] = MethodHandlerMap();
				}		
			}

		void HttpHandler::parse_message(SessionEntry session_entry, const char* data, size_t data_len)
		{
			doc_.PartailParse(data, data_len);

			if(doc_.ParseError())
			{
				session_entry->shutdown();
				LOG(ERROR) << __FUNCTION__ << " " << this <<
					" parse error" << doc_.ParseError().message();
				return;
			}
			if(doc_.ParseDone())
			{

				DLOG(INFO) << "uri is " << doc_.GetUri();
				rapidhttp::HttpDocument doc(rapidhttp::Response);
				
				//process handler
				auto& method = doc_.GetMethod();	
				bool not_found = true;
				if(handler_map_.find(method) != handler_map_.end())
				{
					auto& method_handler_map = handler_map_[method];
					auto& uri = doc_.GetUri();
					if(method_handler_map.find(uri) != method_handler_map.end())
					{
						if(method_handler_map[uri](doc_, doc))
						{
							doc.SetStatusCode(200);
							doc.SetStatus("OK");
							doc.SetField("Content-Length", std::to_string(doc.GetBody().size()));
						}

						not_found = false;
					}
				}

				if(not_found)
				{
					//default handler not found
					doc.SetStatusCode(404);
					doc.SetStatus("Not Found");
					doc.SetField("Content-Length", "0");
				}

				size_t len = doc.ByteSize();
				char buf[len];
				if(doc.Serialize(buf, len))
				{
					session_entry->send(buf, len);
				}
				else
				{
					LOG(ERROR) << __FUNCTION__ << " " << session_entry.get_ptr() << " Serialize error" <<
						doc.ParseError().message();
				}

				doc_.Reset();
			}
		}

		SessionHandlerPtr HttpHandler::get_copy()
		{
			return std::make_shared<HttpHandler>();
		}

		void HttpHandler::register_handler(std::string const& method, std::string const& uri, HttpCb cb)
		{
			if(handler_map_.find(method) != handler_map_.end())
			{
				handler_map_[method][uri] = cb;
			}
			else
			{
				LOG(ERROR) << "unsupported http handler type " << method;
			}
		}

	}
}




