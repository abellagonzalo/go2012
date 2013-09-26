#ifndef BodyMovesParser_H
#define BodyMovesParser_H

#include <alproxies/almotionproxy.h>
#include <string>
#include <vector>

/**
* Base class to parse the move files.
*/
class BodyMovesParser
{
	public:
		BodyMovesParser(const std::string& file) : _file(file) { };
		virtual ~BodyMovesParser() 				{ };
		virtual void parse()					{ };
		std::string& getFile()					{ return _file; };
		std::vector<std::string>& getNames()	{ return _names; };
		AL::ALValue& getTimes()					{ return _times; };
		AL::ALValue& getKeys()					{ return _keys; };

	protected:
		/**
		* Base element to implement the Chain of Responsibility pattern.
		*/
		class ParserElement {
			public:
				ParserElement()										{ _next=0; };
				virtual ~ParserElement()							{ };
				void setNext(ParserElement *next)					{ _next=next; };
				void add(ParserElement *next)						{ _next ? _next->add(next) : setNext(next); };
				virtual void handle(std::string& line,
									std::vector<std::string>& names,
									AL::ALValue& times,
									AL::ALValue& keys)				{ _next->handle(line, names, times, keys); };

			private:
				ParserElement* _next;
		};

		/**
		* Last element to include in the chain. It stores each line that 
		* hasn't been processed by any of the previous elements.
		*/
		class LinesNotProcessedParserElement : public ParserElement {
			public:
				LinesNotProcessedParserElement() : ParserElement() 	{ };
				virtual ~LinesNotProcessedParserElement()			{ };
				virtual void handle(std::string& line,
									std::vector<std::string>& names,
									AL::ALValue& times,
									AL::ALValue& keys)				{ _linesNotProcessed.push_back(line); };
				std::vector<std::string>& getLinesNotProcessed()	{ return _linesNotProcessed; };

			private:
				std::vector<std::string> _linesNotProcessed;
		};

	private:
		std::string _file;
		std::vector<std::string> _names;
		AL::ALValue _times;
		AL::ALValue _keys;
};

#endif // BodyMovesParser_H
