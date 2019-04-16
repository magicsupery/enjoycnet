#pragma once
#include "utils.h"
#include <enjoycco/coroutine.h>
#include <utility>
#include <ev++.h>
#include <assert.h>

namespace enjoyc
{
	namespace net
	{
		class CoEvent: public NonCopyable
		{
			public:
				CoEvent(ev::dynamic_loop* ev_loop, int fd)
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

				~CoEvent()
				{
					assert(read_co_ == nullptr and write_co_ == nullptr);

					read_wathcer_.stop();
					write_wathcer_.stop();
				}

			public:
				// yield from event, wait for cb
				void wait_read()
				{
					ASSERT_IN_COROUTINE;
					read_wathcer_.start();
					read_co_ = co::CoroutineContext::this_coroutine();
					CO_YIELD;
				}

				void wait_write()
				{

					ASSERT_IN_COROUTINE;
					write_wathcer_.start();
					write_co_ = co::CoroutineContext::this_coroutine();
					CO_YIELD;
				}
				// cb from libev
				void read_cb(ev::io &w, int revents)
				{
					read_wathcer_.stop();
					auto co = std::exchange(read_co_, nullptr);
					co->start();
				}

				void write_cb(ev::io &w, int revents)
				{
					write_wathcer_.stop();
					auto co = std::exchange(write_co_, nullptr);
					co->start();
				}

			private:
				ev::io read_wathcer_;
				ev::io write_wathcer_;

				enjoyc::co::Coroutine* read_co_;
				enjoyc::co::Coroutine* write_co_;
		};
	}
}
