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

using namespace enjoyc::net;
using namespace std;
int main(int , char** argv)
{
	::google::InitGoogleLogging(argv[0]);
	::google::LogToStderr();
	DLOG(INFO) << __FUNCTION__ ;
	ThreadContext::init();

	enjoyc::co::Coroutine s([]{
			Endpoint ep("0.0.0.0", 1234);
			Acceptor<Tcp>  acc(
					[](Socket<Tcp> const& ns)
					{
						std::cout << "get connection from " << ns.remote_addr() << std::endl;
						std::shared_ptr<Connection<Tcp, HttpCodec>> con_ptr = std::make_shared<Connection<Tcp, HttpCodec>>
								(ThreadContext::this_io_context(), ns,
								[&](HttpRequest const& req){

								std::cout <<" read_callback " << ns.remote_addr() << std::endl;
								HttpResponse res(rapidhttp::Response);

								res.SetBody("hello world");

								res.SetStatusCode(200);
								res.SetStatus("OK");
								res.SetField("Content-Length", std::to_string(11));

								auto len = res.ByteSize();
								char buf[len];
								res.Serialize(buf, len);
								con_ptr->write(buf, len);

								con_ptr->close();
								});
						
						bool ret;
						do{
							ret = con_ptr->read();
						}while(ret);

						DLOG(INFO) << "end connection " << ns.remote_addr();

					});

			if(not acc.listen(ep, 1024))
			{
				std::cout << "wrong listen " << errno << std::endl;
				return;	
			}	

			while(true)
			{
				int res = acc.accept();
				if(res < 0)
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
	s.start();

	ThreadContext::this_io_context()->run();
}
