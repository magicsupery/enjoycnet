#pragma once
#include "utils.h"
#include <ev++.h>
#include <enjoycco/coroutine.h>

namespace enjoyc
{
	namespace net
	{
		class CoEvent: public NonCopyable
		{
			public:
				CoEvent(ev::dynamic_loop* ev_loop, int fd);
				~CoEvent();
			public:
				// yield from event, wait for cb
				void wait_read();
				void wait_write();

				void read_cb(ev::io &w, int revents);
				void write_cb(ev::io &w, int revents);

			private:
				ev::io read_wathcer_;
				ev::io write_wathcer_;

				enjoyc::co::Coroutine* read_co_;
				enjoyc::co::Coroutine* write_co_;
		};
	}
}
