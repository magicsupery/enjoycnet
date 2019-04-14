#pragma once
#include "utils.h"
#include <enjoycco/coroutine.h>
#include <ev.h>

namespace enjoyc
{
	namespace net
	{
		class CoEvent: public NonCopyable
		{
			public:
				CoEvent(struct ev_loop* ev_loop, int fd)
					:ev_loop_(ev_loop),
					read_co_(nullptr),
					write_co_(nullptr)
				{

					ev_init(&read_watcher_, std::bind<&CoEvent::read_cb, this, _1, _2, _3>);
					ev_io_set(&read_wathcer_, fd, EV_READ);

					ev_init(&write_watcher_, std::bind<&CoEvent::write_cb, this, _1, _2, _3>);
					ev_io_set(&write_wathcer_, fd, EV_WRITE);
				}

				~CoEvent()
				{

				}

			public:
				// yield from event, wait for cb
				void wait_read()
				{
					ASSERT_IN_COROUTINE;
					ev_io_start(ev_loop_, &read_wathcer_);
					read_co_ = CoroutineContext::this_coroutine();
					CO_YIELD;
				}

				void wait_write()
				{

					ASSERT_IN_COROUTINE;
					ev_io_start(ev_loop_, &write_wathcer_);
					write_co_ = CoroutineContext::this_coroutine();
					CO_YIELD;
				}
				// cb from libev
				void read_cb(struct ev_loop* loop, ev_io *w, int revents)
				{
					ev_io_stop(ev_loop_, &read_wathcer_);
					read_co_->start();
				}

				void write_cb(struct ev_loop* loop, ev_io *w, int revents)
				{
					ev_io_stop(ev_loop_, &read_wathcer_);
					write_co_->start();
				}

			private:
				struct ev_loop* ev_loop_;
				struct ev_io read_wathcer_;
				struct ev_io write_wathcer_;

				enjoyc::co::Coroutine* read_co_;
				enjoyc::co::Coroutine* write_co_;
		};
	}
}
