#ifndef OldMovesParser_H
#define OldMovesParser_H

#include "BodyMovesParser.h"
#include "RegexHelper.h"

/**
* The parser implemented with Chain Of Responsibility pattern.
*/
class OldMovesParser : public BodyMovesParser
{
	public:
		OldMovesParser(const std::string &file);
		virtual ~OldMovesParser() {};
		virtual void parse();

	private:
		/**
		* A link of the chain.
		*
		* This link handles the strings with format:
		*   {string}.arraySetSize({int});
		*/
		class OldMovesParserElement1 : public ParserElement, public RegexHelper
		{
			public:
				OldMovesParserElement1() : ParserElement() {};
				virtual ~OldMovesParserElement1() {};
				virtual void handle(std::string& line,
									std::vector<std::string>& names,
									AL::ALValue& times,
									AL::ALValue& keys);
		};

		/**
		* A link of the chain.
		*
		* This link handles the strings with format:
		*   names[{int}] = "{string}";
		*/
		class OldMovesParserElement2 : public ParserElement, public RegexHelper
		{
			public:
				OldMovesParserElement2() : ParserElement() {};
				virtual ~OldMovesParserElement2() {};
				virtual void handle(std::string& line,
									std::vector<std::string>& names,
									AL::ALValue& times,
									AL::ALValue& keys);
		};
	
		/**
		* A link of the chain.
		*
		* This link handles the strings with format:
		*   {string}[{int}].arraySetSize({int});
		*/
		class OldMovesParserElement3 : public ParserElement, public RegexHelper
		{
			public:
				OldMovesParserElement3() : ParserElement() {};
				virtual ~OldMovesParserElement3() {};
				virtual void handle(std::string& line,
									std::vector<std::string>& names,
									AL::ALValue& times,
									AL::ALValue& keys);
		};

		/**
		* A link of the chain.
		*
		* This link handles the strings with format:
		*   duration = {float};
		*/
		class OldMovesParserElement4 : public ParserElement, public RegexHelper
		{
			public:
				OldMovesParserElement4() : ParserElement() { _duration=0.0; };
				virtual ~OldMovesParserElement4() {};
				virtual void handle(std::string& line,
									std::vector<std::string>& names,
									AL::ALValue& times,
									AL::ALValue& keys);
				double getDuration()	{ return _duration; };

			private:
				double _duration;
		};

		/**
		* A link of the chain.
		*
		* This link handles the strings with format:
		*   times[{int}][{int}] = duration;
		*/
		class OldMovesParserElement5 : public ParserElement, public RegexHelper
		{
			public:
				OldMovesParserElement5() : ParserElement() {};		
				virtual ~OldMovesParserElement5() {};
				virtual void handle(std::string& line,
									std::vector<std::string>& names,
									AL::ALValue& times,
									AL::ALValue& keys);
				void setDurationProvider(OldMovesParserElement4* link)	{ _durationProvider = link; };

			private:
				OldMovesParserElement4 *_durationProvider;
		};

		/**
		* A link of the chain.
		*
		* This link handles the strings with format:
		*   keys[{int}][{int}] = ALValue::array({float}, ALValue::array({int}, {float}, float), ALValue::array({int}, {float}, {float}));
		*/
		class OldMovesParserElement6 : public ParserElement, public RegexHelper
		{
			public:
				OldMovesParserElement6() : ParserElement()	{};
				virtual ~OldMovesParserElement6()			{};
				virtual void handle(std::string& line,
									std::vector<std::string>& names,
									AL::ALValue& times,
									AL::ALValue& keys);
		};

		/**
		* A link of the chain.
		*
		* This link handles the strings with format:
		*   times[{int}][{int}] = (float)times[{int}][{int}] + duration; //Comment
		*/
		class OldMovesParserElement7 : public ParserElement, public RegexHelper
		{
			public:
				OldMovesParserElement7() : ParserElement()	{};
				virtual ~OldMovesParserElement7() 			{};
				virtual void handle(std::string& line,
									std::vector<std::string>& names,
									AL::ALValue& times,
									AL::ALValue& keys);
				void setDurationProvider(OldMovesParserElement4* link)	{ _durationProvider = link; };

			private:
				OldMovesParserElement4 *_durationProvider;
		};

		/**
		* A link of the chain.
		*
		* This link handles the strings with format:
		*   times[{int}][{int}] = (float)times[{int}][{int}] + {float}; //Comment
		*/
		class OldMovesParserElement8 : public ParserElement, public RegexHelper
		{
			public:
				OldMovesParserElement8() : ParserElement()	{};
				virtual ~OldMovesParserElement8() 			{};
				virtual void handle(std::string& line,
									std::vector<std::string>& names,
									AL::ALValue& times,
									AL::ALValue& keys);
		};

		// Elements of the chain.
		ParserElement *_root;
		OldMovesParserElement1 _link1;
		OldMovesParserElement2 _link2;
		OldMovesParserElement3 _link3;
		OldMovesParserElement4 _link4;
		OldMovesParserElement5 _link5;
		OldMovesParserElement6 _link6;
		OldMovesParserElement7 _link7;
		OldMovesParserElement8 _link8;
		LinesNotProcessedParserElement _end;
};





#endif // OldMovesParser_H
