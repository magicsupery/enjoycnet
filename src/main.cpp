#include "abstract/endpoint.h"
#include <iostream>

using namespace enjoyc::net;
using boost::system::error_code;
int main()
{
	error_code ec;
	auto ep = Endpoint::from_string("https://127.0.0.1:1234", ec);
	
	if(ec)
	{
		std::cout << "parse wrong with " << ec.message() << std::endl;
	}
	else
	{
		std::cout << "parse string is " << ep.to_string(ec) << std::endl;
	}

}
