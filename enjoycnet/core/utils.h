#pragma once

namespace enjoyc
{
	namespace net
	{
		class NonCopyable
		{
			public:
				NonCopyable() = default;
				~NonCopyable() = default;

				NonCopyable(NonCopyable const& other)  = delete ;

				NonCopyable& operator=(NonCopyable const& other)  = delete ;
		};
	}
}
