#pragma once
#include <boost/system/system_error.hpp>
namespace enjoyc
{
	namespace net
	{
		using boost_ec = boost::system::error_code;

		enum class ErrorCode: int
		{
			ec_ok = 0,
			ec_url_parse_wrong = 1,
			ec_dns_not_found = 2,
			ec_not_support_ptotocol = 3,
		};

		class ErrorCategory
			: public boost::system::error_category
		{
			public:
				virtual const char* name() const noexcept override;

				virtual std::string message(int ec) const override;
		};

		const boost::system::error_category& GetNetworkErrorCategory();

		boost_ec MakeNetworkErrorCode(ErrorCode code);

		void ThrowError(ErrorCode code, const char* what = "");
	}
}
