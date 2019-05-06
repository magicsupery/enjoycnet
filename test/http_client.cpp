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
		
		Connector<Tcp> connector([](Socket<Tcp> const& socket){
			GO([&](){
				Socket<Tcp> mysocket(socket);
				DLOG(INFO) << "connect to " << mysocket.remote_addr();
				mysocket.close();
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
