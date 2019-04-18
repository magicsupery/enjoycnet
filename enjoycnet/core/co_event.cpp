#include "co_event.h"
#include "io_context.h"
#include <utility>
#include <assert.h>
#include <glog/logging.h>

namespace enjoyc
{
	namespace net
	{

		CoEvent::CoEvent(ev::dynamic_loop* ev_loop, int fd)
			:read_co_(nullptr),
			write_co_(nullptr)
		{
			read_wathcer_.set(ev_loop->raw_loop);
			read_wathcer_.set(fd, EV_READ);
			read_wathcer_.set<CoEvent, &CoEvent::read_cb>(this);

			write_wathcer_.set(ev_loop->raw_loop);
			write_wathcer_.set(fd, EV_WRITE);
			write_wathcer_.set<CoEvent, &CoEvent::write_cb>(this);
		}

		CoEvent::~CoEvent()
		{
			assert(read_co_ == nullptr and write_co_ == nullptr);
			DLOG(INFO) << __FUNCTION__ << " " << this;
			read_wathcer_.stop();
			write_wathcer_.stop();
		}

		void CoEvent::wait_read()
		{
			ASSERT_IN_COROUTINE;
			
			assert(read_co_ == nullptr);
			read_wathcer_.start();
			read_co_ = co::CoroutineContext::this_coroutine();
			DLOG(INFO) << __FUNCTION__ << " co is " << read_co_;
			CO_YIELD;
		}

		void CoEvent::wait_write()
		{

			ASSERT_IN_COROUTINE;
			assert(write_co_ == nullptr);
			write_wathcer_.start();
			write_co_ = co::CoroutineContext::this_coroutine();
			CO_YIELD;
		}
		// cb from libev
		void CoEvent::read_cb(ev::io &w, int revents)
		{

			DLOG(INFO) << __FUNCTION__ << " read_co is " << read_co_ << " event is " << this;
			assert(read_co_ != nullptr);

			read_wathcer_.stop();
			START(std::exchange(read_co_, nullptr));
		}

		void CoEvent::write_cb(ev::io &w, int revents)
		{

			assert(write_co_ != nullptr);
			write_wathcer_.stop();
			START(std::exchange(write_co_, nullptr));
		}
	}
}
