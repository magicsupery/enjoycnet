#pragma once
#include <memory>

namespace enjoyc
{
	namespace net
	{
		class OptionParser
		{
		};

		using OptionParserPtr = std::shared_ptr<OptionParser>;
		

		enum class ParserType
		{
			unkwon = 0,
			http = 1,
		};

		class OptionSessionDefineData
		{
			public:
				OptionSessionDefineData()
					:parser_type_(ParserType::unkwon)
				{
				}

				~OptionSessionDefineData() = default;
				OptionSessionDefineData(OptionSessionDefineData const& other) = default;

				ParserType parser_type_;
		};

		class OptionSessionDefine: public std::enable_shared_from_this<OptionSessionDefine>
		{
			public:
				OptionSessionDefine(OptionSessionDefineData const& data){}

			private:
				OptionParserPtr parser_ptr_;

		};

		class OptionNet
		{
			public:
				int listen_backlog_;
		};

		class Option
		{
			public:
				OptionNet net_;
				OptionSessionDefineData define_data_;
		};

		using OptionPtr = std::shared_ptr<Option>;
	}
}
