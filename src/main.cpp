#include "abstract/endpoint.h"
#include <glog/logging.h>
#include "net/server.h"
#include <libgo/coroutine.h>


using namespace enjoyc::net;
using boost::system::error_code;
int main()
{
	Server s;	
	auto ec = s.start("tcp://127.0.0.1:9876", std::make_shared<Option>());
	if(ec)
	{
		std::cout << "parse wrong with " << ec.message() << std::endl;
	}

	::co::CoroutineOptions::getInstance().debug |= co::dbg_channel;
	co_sched.Start();

}
