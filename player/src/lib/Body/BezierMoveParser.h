#ifndef BezierMoveParser_H
#define BezierMoveParser_H

#include "RegexHelper.h"
#include "BodyMovesParser.h"

/**
* The parser implemented with Chain Of Responsibility pattern.
*/
class BezierMoveParser : public BodyMovesParser
{
	public:
		BezierMoveParser(const std::string &file);
		virtual ~BezierMoveParser() {};
		virtual void parse();

	private:
		/**
		* A link of the chain.
		*
		* This link handles the strings with format:
		*   {string}.{string}({numbers});
		*/
		class BezierMoveParserElement1 : public ParserElement, public RegexHelper
		{
			public:
				BezierMoveParserElement1() : ParserElement() {};
				virtual ~BezierMoveParserElement1() {};
				virtual void handle(std::string& line,
									std::vector<std::string>& names,
									AL::ALValue& times,
									AL::ALValue& keys);
		};

		/**
		* A link of the chain.
		*
		* This link handles the strings with format:
		*   names.push_back("{chars}");
		*/
		class BezierMoveParserElement2 : public ParserElement, public RegexHelper
		{
			public:
				BezierMoveParserElement2() : ParserElement() {};
				virtual ~BezierMoveParserElement2() {};
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
		class BezierMoveParserElement3 : public ParserElement, public RegexHelper
		{
			public:
				BezierMoveParserElement3() : ParserElement() {};
				virtual ~BezierMoveParserElement3() {};
				virtual void handle(std::string& line,
									std::vector<std::string>& names,
									AL::ALValue& times,
									AL::ALValue& keys);
		};

		/**
		* A link of the chain.
		*
		* This link handles the strings with format:
		*   times[{int}][{int}] = {float};
		*/
		class BezierMoveParserElement4 : public ParserElement, public RegexHelper
		{
			public:
				BezierMoveParserElement4() : ParserElement() {};
				virtual ~BezierMoveParserElement4() {};
				virtual void handle(std::string& line,
									std::vector<std::string>& names,
									AL::ALValue& times,
									AL::ALValue& keys);
		};

		/**
		* A link of the chain.
		*
		* This link handles the strings with format:
		*   keys[{int}][{int}] = AL::ALValue::array({float}, AL::ALValue::array({int}, {float}, float), AL::ALValue::array({int}, {float}, {float}));
		*/
		class BezierMoveParserElement5 : public ParserElement, public RegexHelper
		{
			public:
				BezierMoveParserElement5() : ParserElement() {};
				virtual ~BezierMoveParserElement5() {};
				virtual void handle(std::string& line,
									std::vector<std::string>& names,
									AL::ALValue& times,
									AL::ALValue& keys);
		};

		// Elements of the chain 
		ParserElement *_root;
		BezierMoveParserElement1 _link1;
		BezierMoveParserElement2 _link2;
		BezierMoveParserElement3 _link3;
		BezierMoveParserElement4 _link4;
		BezierMoveParserElement5 _link5;
		LinesNotProcessedParserElement _end;
};




#endif //BezierMoveParser_H
