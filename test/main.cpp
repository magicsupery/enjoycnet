#include <glog/logging.h>
#include <libgo/coroutine.h>
#include <enjoycnet/server.h>

using namespace enjoyc::net;
using boost::system::error_code;
int main()
{

	boost_ec ec;
	Server<Tcp, HttpHandler> s(Endpoint::generate("127.0.0.1", 9876, ec));

	auto handler = std::static_pointer_cast<HttpHandler>(s.get_handler());

	auto http_router_ptr = std::make_shared<HttpRouter>();

	http_router_ptr->register_handler("GET", "/test",
		   	[](rapidhttp::HttpDocument& req, rapidhttp::HttpDocument& res)-> bool{
				res.SetBody("get test");
				std::cout << "1111" << std::endl;
				return true;
			});


	http_router_ptr->register_handler("POST", "/test",
		   	[](rapidhttp::HttpDocument& req, rapidhttp::HttpDocument& res) -> bool{
				res.SetBody("post test");
				std::cout << "1111" << std::endl;
				return true;
			});

	handler->set_router_ptr(http_router_ptr);
	if(ec)
	{
		std::cout << "parse wrong with " << ec.message() << std::endl;
		return 0;
	}
	
	// set options 

	ec = s.start();
	if(ec)
	{
		std::cout << "start wrong with " << ec.message() << std::endl;
		return 0;
	}

	std::cout << " 1111 " << std::endl;
	//::co::CoroutineOptions::getInstance().debug |= co::dbg_channel;
	co_sched.Start();

}
