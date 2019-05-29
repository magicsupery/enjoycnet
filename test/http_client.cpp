#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define GOOGLE_GLOG_DLL_DECL
#endif

#include <enjoycnet/all.h>
#include <enjoycco/coroutine.h>

#include <iostream>
#include <glog/logging.h>
#include <thread>

using namespace enjoyc::net;
using namespace std;

void http_client()
{

	ThreadContext::init();
	GO([&]{
		Endpoint ep("127.0.0.1", 9876);
		
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
#ifdef _WIN32
						char* buf = (char*)_alloca(len);
						req.Serialize(buf, len);
						con_ptr->write(buf, len);
#else
						char buf[len]:
						req.Serialize(buf.data(), len);
						con_ptr->write(buf.data(), len);
#endif
						bool ret;
						do{
						ret = con_ptr->read();
						}while(ret);
						DLOG(INFO) << "end connection from " << remote_addr << std::endl;
				});
		});

		bool ret = connector.connect(ep);
		if (!ret)
		{
			LOG(ERROR) << "can not connect to " << ep;
		}

	});

	ThreadContext::this_io_context()->run();
}
int main(int , char** argv)
{
#ifdef _WIN32
	WORD wVersionRequested = MAKEWORD(2, 2);
	WSADATA wsaData;
	WSAStartup(wVersionRequested, &wsaData);
#endif
	::google::InitGoogleLogging(argv[0]);
	::google::LogToStderr();
	LOG(INFO) << __FUNCTION__ << " start";
	vector<thread*> threads;
	threads.emplace_back(new thread(http_client));

	for(auto thread : threads)
		thread->join();

	DLOG(INFO) << __FUNCTION__ << " done";
}
