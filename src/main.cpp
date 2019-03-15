#include <glog/logging.h>
#include <libgo/coroutine.h>
#include "net/tcp/tcp.h"
#include "net/server.h"
#include "option/option.h"


using namespace enjoyc::net;
using boost::system::error_code;
int main()
{

	boost_ec ec;
	Server<Tcp, Option> s(Endpoint::generate("127.0.0.1", 9876, ec));
	if(ec)
	{
		std::cout << "parse wrong with " << ec.message() << std::endl;
		return 0;
	}


	ec = s.start();
	//::co::CoroutineOptions::getInstance().debug |= co::dbg_channel;
	co_sched.Start();

}
