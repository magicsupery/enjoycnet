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

void http_server()
{

	ThreadContext::init();

	GO([&]{

			Endpoint ep("0.0.0.0", 9876);
			Acceptor<Tcp>  acc(
					[&](Socket<Tcp> const& ns)
					{
					DLOG(INFO) << "get connection from " << ns.remote_addr() << std::endl;

					GO([&](){
							std::shared_ptr<Connection<Tcp, HttpServerCodec>> con_ptr = std::make_shared<Connection<Tcp, HttpServerCodec>>
							(ThreadContext::this_io_context(), ns,
							 [&](HttpRequest& req){
							 DLOG(INFO) << "handle req ";
							 HttpResponse res(rapidhttp::Response);

							 res.SetBody("hello world");

							 res.SetStatusCode(200);
							 res.SetStatus("OK");
							 res.SetField("Content-Length", std::to_string(11));

							 auto len = res.ByteSize();
#ifdef _WIN32
							 char* buf = (char*)_alloca(len);
							 res.Serialize(buf, len);
							 con_ptr->write(buf, len);
#else
							 char buf[len];
							 res.Serialize(buf, len);
							 con_ptr->write(buf, len);
#endif

							 DLOG(INFO) << "connection filed is " << req.GetField("Connection").c_str();
							 if (std::string("close").compare(
								 req.GetField("Connection")) == 0)
							 {
								 con_ptr->close();
							 }

							});

							bool ret;
							do {
								ret = con_ptr->read();
							} while (ret);
							DLOG(INFO) << "end connection from " << ns.remote_addr() << std::endl;
					});
			});

			if (!acc.listen(ep, 4096))
			{
				LOG(ERROR) << "can not listen " << ep << " reason " << errno;
				return;
			}

			while (true)
			{
				bool res = acc.accept();
				if (!res)
				{
					if (errno == EINTR || errno == EWOULDBLOCK ||
						errno == ECONNABORTED || errno == EPROTO)
					{
						continue;
					}
					else
					{
						LOG(ERROR) << "wrong accept " << errno << std::endl;
					}
				}
			}

	});

	ThreadContext::this_io_context()->run();
}
int main(int, char** argv)
{
#ifdef _WIN32
	WORD wVersionRequested = MAKEWORD(2, 2);
	WSADATA wsaData;
	WSAStartup(wVersionRequested, &wsaData);
#endif
	::google::InitGoogleLogging(argv[0]);
	::google::LogToStderr();
	DLOG(INFO) << __FUNCTION__ << " start";
	/*
	vector<thread*> threads;
	for (size_t i = 0; 0 < 0; i++)
	{
		threads.emplace_back(new thread(http_server));
	}

	for (auto thread : threads)
		thread->join();
	
	*/
	http_server();
	DLOG(INFO) << __FUNCTION__ << " done";
}
