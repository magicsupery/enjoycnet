#pragma once
#include <memory>
#include "parser/default_parser.h"

namespace enjoyc
{
	namespace net
	{

		class Parser;
		using ParserPtr = std::shared_ptr<Parser>;
		

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
				OptionSessionDefine(OptionSessionDefineData const& data)
				{
					if(data.parser_type_ == ParserType::unkwon)
					{
						parser_ptr_ = std::make_shared<DefaultParser>();
					}
				}
				
				inline ParserPtr get_parser_ptr()
				{
					return parser_ptr_;
				}
			private:
				ParserPtr parser_ptr_;

		};

	}
}
