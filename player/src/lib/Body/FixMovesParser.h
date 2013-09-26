#ifndef FixMovesParser_H
#define FixMovesParser_H

#include "RegexHelper.h"
#include "BodyMovesParser.h"

class FixMovesParser : public BodyMovesParser
{
	public:
		FixMovesParser(const std::string &file);
		virtual ~FixMovesParser() {};
		virtual void parse();

	private:
		/**
		* Link of the chain.
		* 
		* This link handles the string with format:
		*	{string}*
		*/
		class FixMovesParserElement1 : public ParserElement, public RegexHelper
		{
			public:
				FixMovesParserElement1() : ParserElement() {};
				virtual void handle(std::string& line,
									std::vector<std::string>& names,
									AL::ALValue& times,
									AL::ALValue& keys);
		};

		/**
		* Link of the chain.
		* 
		* This link handles the string with format:
		*	{float}*
		*/
		class FixMovesParserElement2 : public ParserElement, public RegexHelper
		{
			public:
				FixMovesParserElement2();
				virtual void handle(std::string& line,
									std::vector<std::string>& names,
									AL::ALValue& times,
									AL::ALValue& keys);
			private:
				int _lineCounter;
		};

		/**
		* Link of the chain.
		* 
		* This link handles the string with format:
		*	{something}*
		*/
		class FixMovesParserElement3 : public ParserElement, public RegexHelper
		{
			public:
				FixMovesParserElement3();
				virtual void handle(std::string& line,
									std::vector<std::string>& names,
									AL::ALValue& times,
									AL::ALValue& keys);

			private:
				int _lineCounter;
		};

		ParserElement *_root;
		FixMovesParserElement1 _link1;
		FixMovesParserElement2 _link2;
		FixMovesParserElement3 _link3;
		LinesNotProcessedParserElement _end;
};

#endif // FixMovesParser_H