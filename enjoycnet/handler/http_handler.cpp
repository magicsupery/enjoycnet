#include "http_handler.h"
#include "../abstract/session.h"
#include <glog/logging.h>


namespace enjoyc
{
	namespace net
	{
		
		HttpHandler::HttpHandler()
			:doc_(rapidhttp::Request),
			router_ptr_(new HttpRouter)
		{
		}

		size_t HttpHandler::parse_message(SessionEntry session_entry, const char* data, size_t data_len)
		{
			auto handled = doc_.PartailParse(data, data_len);
			assert(handled == data_len);

			if(doc_.ParseError())
			{
				session_entry->shutdown();
				LOG(ERROR) << __FUNCTION__ << " " << this <<
					" parse error" << doc_.ParseError().message();
				return 0;
			}
			if(doc_.ParseDone())
			{

				//DLOG(INFO) << "uri is " << doc_.GetUri();
				rapidhttp::HttpDocument doc(rapidhttp::Response);
				
				router_ptr_->route(doc_, doc);
				size_t len = doc.ByteSize();
				char buf[len];
				if(doc.Serialize(buf, len))
				{
					DLOG(INFO) << "send res";
					session_entry->send_no_delay(buf, len);
				}
				else
				{
					LOG(ERROR) << __FUNCTION__ << " " << session_entry.get_ptr() << " Serialize error" <<
						doc.ParseError().message();
					session_entry->shutdown();
					return 0;
				}

				//DLOG(INFO) << "connection is " << doc_.GetField("Connection");
				if(doc_.GetField("Connection") == "close")
				{
					doc_.Reset();
					session_entry->shutdown();
				}	
				else
				{
					doc_.Reset();
				}
				
				return handled;
			}

			return 0;
		}

		SessionHandlerPtr HttpHandler::get_copy()
		{
			auto ptr = std::make_shared<HttpHandler>();
			ptr->set_router_ptr(router_ptr_);
			return ptr;
		}

	}
}




