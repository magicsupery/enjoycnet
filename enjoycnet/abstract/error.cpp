#include "error.h"
#include <exception>

namespace enjoyc
{
	namespace net
	{

		const char* ErrorCategory::name() const noexcept
		{
			return "network_error";
		}
		std::string ErrorCategory::message(int ec) const
		{
			switch(ec)
			{
				case int(ErrorCode::ec_ok):
					return "ok";
				case int(ErrorCode::ec_url_parse_wrong):
					return "url_parse_wrong";
				case int(ErrorCode::ec_dns_not_found):
					return "dns_not_found";
				case int(ErrorCode::ec_not_support_ptotocol):
					return "not_support_ptotocol";
			}
			return "";
		}

		const boost::system::error_category& GetNetworkErrorCategory()
		{
			static ErrorCategory obj;
			return obj;
		}

		boost_ec MakeNetworkErrorCode(ErrorCode code)
		{
			return boost_ec((int)code, GetNetworkErrorCategory());
		}

		void ThrowError(ErrorCode code, const char* what)
		{
			if (std::uncaught_exception()) return ;
			throw boost::system::system_error(MakeNetworkErrorCode(code), what);
		}

	}// net
}//enjoyc


