#include "core/io_context.h"
#include "core/co_event.h"
#include "core/socket.h"
#include "core/proto_tcp.h"
#include "core/acceptor.h"
#include "core/connection.h"
#include "core/codec.h"
#include <enjoycco/coroutine.h>

#include <iostream>

using namespace enjoyc::net;
using namespace std;
int main()
{
	ThreadContext::init();

	enjoyc::co::Coroutine s([]{
			Endpoint ep("0.0.0.0", 1234);
			Acceptor<Tcp>  acc(
					[](Socket<Tcp> ns)
					{

						Connection<Tcp, HttpCodec> con(ThreadContext::this_io_context(), ns,
								[](HttpRequest const& req){});

							

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
