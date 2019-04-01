#include <glog/logging.h>
#include <libgo/coroutine.h>
#include <enjoycnet/server.h>
#include <enjoycnet/net/tcp/tcp_session.h>
#include <libgo/netio/unix/reactor.h>

using namespace enjoyc::net;
using boost::system::error_code;

void task(int i)
{
	LOG(ERROR) << "before task " << i;
	

	LOG(ERROR) << "after task " << i;
}

void accept()
{
	int i = 0;
	for(; i < 100; i++)
	{
		go [i]{task(i);};
	}

}


using Sessions =  std::set<SessionEntry>;
void init_server(Scheduler* sche)
{
	Sessions sessions;
	boost_ec ec;

	Option ops;
	ops.cb_.accept_cb_ = [&](SessionEntry const& s){
		sessions.insert(s);
	};
	ops.cb_.dis_cb_= [&](SessionEntry const& s){
		sessions.erase(s);
	};
	Server<Tcp, HttpHandler> s(Endpoint::generate("0.0.0.0", 9876, ec),
			sche, &ops);

	auto handler = std::static_pointer_cast<HttpHandler>(s.get_handler());

	auto http_router_ptr = std::make_shared<HttpRouter>();

	http_router_ptr->register_handler("GET", "/test",
		   	[](rapidhttp::HttpDocumentRef& req, rapidhttp::HttpDocument& res)-> bool{
				res.SetBody("hello world");
				return true;
			});


	http_router_ptr->register_handler("POST", "/test",
		   	[](rapidhttp::HttpDocumentRef& req, rapidhttp::HttpDocument& res) -> bool{
				res.SetBody("post test");
				return true;
			});

	handler->set_router_ptr(http_router_ptr);
	if(ec)
	{
		std::cout << "parse wrong with " << ec.message() << std::endl;
		return;
	}
	
	ec = s.start();
	if(ec)
	{
		std::cout << "start wrong with " << ec.message() << std::endl;
		return;
	}

	sche->Start(1, 1);
}

int main(int argc, char** argv)
{
	google::InitGoogleLogging(argv[0]);
	
	std::vector<std::thread* > threads;
	std::vector<Scheduler*>	 sches;
	for(size_t i = 0; i < std::thread::hardware_concurrency(); i++)	
	{
		Scheduler* sche = Scheduler::Create();
		threads.emplace_back(new std::thread(init_server, sche));
		sches.emplace_back(sche);
	}

	for(auto& thread : threads)
		thread->join();

}
