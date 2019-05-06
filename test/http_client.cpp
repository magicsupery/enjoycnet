#include <enjoycco/coroutine.h>
#include <enjoycnet/all.h>

#include <iostream>
#include <glog/logging.h>
#include <thread>

using namespace enjoyc::net;
using namespace std;

void http_client()
{

	ThreadContext::init();
	GO([&]{
		Endpoint ep("0.0.0.0", 9876);
		
		Connector<Tcp> connector([](Socket<Tcp> const& ns){
				GO([&](){
						std::shared_ptr<Connection<Tcp, HttpClientCodec>> con_ptr
						= std::make_shared<Connection<Tcp, HttpClientCodec>>
						(ThreadContext::this_io_context(), ns,
						 [&](HttpResponse& res){
						 	DLOG(INFO) << "read HttpResponse " << res.GetBody().c_str();
						 });
						
						auto remote_addr = ns.remote_addr();	
						HttpRequest req(rapidhttp::Request);
						req.SetUri("/test");
						req.SetMethod("GET");
						//req.SetField("Connection", "close\r\n");
						req.SetField("Accept", "text\r\n");

						auto len = req.ByteSize();
						DLOG(INFO) << "len " << len;
						char buf[len];
						req.Serialize(buf, len);
						con_ptr->write(buf, len);

						bool ret;
						do{
						ret = con_ptr->read();
						}while(ret);
						DLOG(INFO) << "end connection from " << remote_addr << std::endl;
				});
		});

		connector.connect(ep);

	});

	ThreadContext::this_io_context()->run();
}
int main(int , char** argv)
{
	::google::InitGoogleLogging(argv[0]);
	::google::LogToStderr();
	DLOG(INFO) << __FUNCTION__ << " start";
	vector<thread*> threads;
	threads.emplace_back(new thread(http_client));

	for(auto thread : threads)
		thread->join();

	DLOG(INFO) << __FUNCTION__ << " done";
}
