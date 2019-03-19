#pragma once
#include <memory>

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
					 read_buffer_num_(1024)
			{
			}

			public:
				int listen_backlog_;
				int msg_chan_buffer_num_;
				int read_buffer_num_;
		};

		class Option
		{
			public:
				OptionNet net_;
		};

		using OptionPtr = std::shared_ptr<Option>;
	}
}
