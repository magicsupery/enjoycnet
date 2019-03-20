#include "http_router.h"
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

		HttpRouter::HttpRouter()
			{
				for(auto name : method_names)
				{
					handler_map_[name] = MethodHandlerMap();
				}		
			}

		void HttpRouter::register_handler(std::string const& method, std::string const& uri, HttpCb cb)
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

		void HttpRouter::route(HttpRequest& req, HttpResponse& res)
		{

			auto& method = req.GetMethod();	
			if(handler_map_.find(method) != handler_map_.end())
			{
				auto& method_handler_map = handler_map_[method];
				auto& uri = req.GetUri();
				if(method_handler_map.find(uri) != method_handler_map.end())
				{
					if(method_handler_map[uri](req, res))
					{
						res.SetStatusCode(200);
						res.SetStatus("OK");
						res.SetField("Content-Length", std::to_string(res.GetBody().size()));
						return;
					}
				}
			}

			//default handler not found
			res.SetStatusCode(404);
			res.SetStatus("Not Found");
			res.SetField("Content-Length", "0");
		}

	}
}




