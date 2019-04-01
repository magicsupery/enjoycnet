#pragma once
#include <memory>
#include "../abstract/session.h"

namespace enjoyc
{
	namespace net
	{
		class OptionNet
		{
			public:
				OptionNet()
					:listen_backlog_(1024),
					msg_chan_buffer_num_(1024),
					read_buffer_size_(8 * 1024),
					max_read_buf_size_limit_(64 * 1024),
					max_read_buf_shrik_size_(32 * 1024)
			{
			}

			public:
				size_t listen_backlog_;
				size_t msg_chan_buffer_num_;
				size_t read_buffer_size_;
				size_t max_read_buf_size_limit_;
				size_t max_read_buf_shrik_size_;
		};
		
		using AccpetCallback = std::function<void(SessionEntry const&)>;
		using DisconnectCallback = std::function<void(SessionEntry const&)>;

		class OptionCb
		{
			public:
				OptionCb()
					:accept_cb_([](SessionEntry const& s){}),
					dis_cb_([](SessionEntry const& s){})
					{}
				AccpetCallback accept_cb_;		
				DisconnectCallback dis_cb_;
		};
		class Option
		{
			public:
				OptionNet net_;
				OptionCb cb_;
		};

		using OptionPtr = std::shared_ptr<Option>;
	}
}
