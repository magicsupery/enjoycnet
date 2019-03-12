#pragma once
#include <vector>
#include "endpoint.h"

namespace enjoyc
{
	namespace net
	{
		using Buffer = std::vector<char>;
		class SessionBase
		{
			public:
				virtual void send(Buffer&& buf) = 0;
				virtual void shutdown() = 0;
				virtual Endpoint LocalAddr() = 0;
				virtual Endpoint RemoteAddr() = 0;
		};

		class FakeSession: public SessionBase
		{
			virtual void send(Buffer&& buf) override
			{
				return;
			}
			virtual void shutdown() override
			{
				return;
			}
			virtual Endpoint LocalAddr() override
			{
				return Endpoint();
			}
			virtual Endpoint RemoteAddr() override
			{
				return Endpoint();
			}
		};

		class SessionEntry
		{
			using SessionImpl = std::shared_ptr<SessionBase>;
			public:
				SessionEntry() = default;
				SessionEntry(SessionImpl impl): session_impl_(impl) {}
				
				template<typename T>
				explicit SessionEntry(std::shared_ptr<T> impl,
						typename std::enable_if<std::is_base_of<SessionBase, T>::value>::type* = nullptr)
				{
					session_impl_ = std::static_pointer_cast<SessionBase>(impl);
				}
				
				inline SessionBase* operator->()
				{
					return get_ptr();
				}

			private:
				inline SessionBase* get_ptr()
				{
					return session_impl_ ? session_impl_.get() : &fake_session_;
				}

				static FakeSession fake_session_;
				SessionImpl session_impl_;
		};
	}
}
