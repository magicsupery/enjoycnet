#include "core/io_context.h"
#include "core/co_event.h"
#include "core/socket.h"
#include "core/proto_tcp.h"
#include "core/acceptor.h"
#include "core/connection.h"
#include "core/codec.h"
#include <enjoycco/coroutine.h>

#include <iostream>
#include <glog/logging.h>
#include <thread>

using namespace enjoyc::net;
using namespace std;

void http_server()
{

	ThreadContext::init();

	std::vector<enjoyc::co::Coroutine> cos;
	GO([&]{

			Endpoint ep("0.0.0.0", 9876);
			Acceptor<Tcp>  acc(
					[&](Socket<Tcp> const& ns)
					{
					DLOG(INFO) << "get connection from " << ns.remote_addr() << std::endl;

					GO([&](){
							std::shared_ptr<Connection<Tcp, HttpCodec>> con_ptr = std::make_shared<Connection<Tcp, HttpCodec>>
							(ThreadContext::this_io_context(), ns,
							 [&](HttpRequest& req){
								 HttpResponse res(rapidhttp::Response);

								 res.SetBody("hello world");

								 res.SetStatusCode(200);
								 res.SetStatus("OK");
								 res.SetField("Content-Length", std::to_string(11));

								 auto len = res.ByteSize();
								 char buf[len];
								 res.Serialize(buf, len);
								 con_ptr->write(buf, len);
								
							 	 	 
								 if(std::string("close").compare(
											 req.GetField("Connection")) == 0)
								 {
									//con_ptr->close();		
								 }

							 });

							bool ret;
							do{
								ret = con_ptr->read();
							}while(ret);
							DLOG(INFO) << "end connection from " << ns.remote_addr() << std::endl;
					});
					});

			if(not acc.listen(ep, 1024))
			{
				std::cout << "wrong listen " << errno << std::endl;
				return;	
			}	

			while(true)
			{
				int res = acc.accept();
				DLOG(INFO) << __FUNCTION__ << " accept " << res;
				if(not res)
				{
					if(errno == EINTR or errno == EWOULDBLOCK or
							errno == ECONNABORTED or errno == EPROTO)
					{
						continue;
					}
					else
					{
						std::cout << "wrong accept " << errno << std::endl;
					}
				}
			}
	});

	ThreadContext::this_io_context()->run();
}
int main(int , char** argv)
{
	::google::InitGoogleLogging(argv[0]);
	::google::LogToStderr();
	DLOG(INFO) << __FUNCTION__ << " start";

	vector<thread*> threads;

	for(int i = 0; i < 20; i++)
	{
		threads.emplace_back(new thread(http_server));
	}


	for(auto thread : threads)
		thread->join();

	DLOG(INFO) << __FUNCTION__ << " done";
}
