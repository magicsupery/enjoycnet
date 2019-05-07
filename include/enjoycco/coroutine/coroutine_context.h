#pragma once
#include "../utils/noncopyable.h"

namespace enjoyc
{
	namespace co
	{
		class Coroutine;
		class CoroutineContext: public NonCopyable
		{
			private:
				thread_local static Coroutine* current_;
			public:
				inline static void set_current_coroutine(Coroutine* current)
				{
					current_ = current;
				}

				inline static Coroutine* this_coroutine()
				{
					return current_;
				}

				inline static bool in_coroutine_context()
				{
					return current_ != nullptr;
				}
		};

		enum class Coroutine_S
		{
			INVALID = 0,
			READY,
			RUNNING,
			FINISHED,
		};

		enum class RetCode
		{
			ret_success = 0,
			ret_already_running = 1,
			ret_already_finished = 2,
		};
	}
}
