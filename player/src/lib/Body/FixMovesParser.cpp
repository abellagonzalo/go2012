#include "FixMovesParser.h"
#include <boost/tr1/regex.hpp>
#include <iostream>
#include <fstream>


void
FixMovesParser::FixMovesParserElement1::handle(std::string& line,
							 				   std::vector<std::string>& names,
							 				   AL::ALValue& times,
							 				   AL::ALValue& keys)
{
	// Regular expression
	const std::tr1::regex pattern("([a-zA-Z]+)");

	const std::tr1::regex_token_iterator<std::string::iterator> end;
	std::tr1::regex_token_iterator<std::string::iterator> it(line.begin(), line.end(), pattern); 
	
	// If empty pass to next link
	if (it == end) {
		ParserElement::handle(line, names, times, keys);

	} else {
		names.clear();
		while (it != end) {
			names.push_back(*it++);
		}
	}
}

FixMovesParser::FixMovesParserElement2::FixMovesParserElement2() 
: 
	ParserElement() 
{ 
	_lineCounter=0; 
};


void
FixMovesParser::FixMovesParserElement2::handle(std::string& line,
							 				   std::vector<std::string>& names,
							 				   AL::ALValue& times,
							 				   AL::ALValue& keys)
{
	// Security check
	if (keys.getSize() == names.size())  {
		ParserElement::handle(line, names, times, keys);
		return;
	}

	// Regular expression
	const std::tr1::regex pattern(floatVar);

	const std::tr1::regex_token_iterator<std::string::iterator> end;
	std::tr1::regex_token_iterator<std::string::iterator> begin(line.begin(), line.end(), pattern); 
	
	// If empty pass to next link
	if (begin == end) {
		ParserElement::handle(line, names, times, keys);

	// Parse line
	} else {
		AL::ALValue values;
		while (begin != end) {
			values.arrayPush(stringToDouble(*begin++));
		}
		keys.arrayPush(values);
	}
}

FixMovesParser::FixMovesParserElement3::FixMovesParserElement3() 
: 
	ParserElement() 
{ 
	_lineCounter=0; 
};

void
FixMovesParser::FixMovesParserElement3::handle(std::string& line,
							 				   std::vector<std::string>& names,
							 				   AL::ALValue& times,
							 				   AL::ALValue& keys)
{
	// Security check
	if (times.getSize() == names.size()) {
		ParserElement::handle(line, names, times, keys);
		return;
	}

	// Regular expression
	const std::tr1::regex pattern("(\\d+\\.\\d+|\\d+)");

	const std::tr1::regex_token_iterator<std::string::iterator> end;
	std::tr1::regex_token_iterator<std::string::iterator> begin(line.begin(), line.end(), pattern); 
	
	// If empty pass to next link
	if (begin == end) {
		ParserElement::handle(line, names, times, keys);

	// Parse line
	} else {
		AL::ALValue values;
		while (begin != end) {
			values.arrayPush(stringToDouble(*begin++));
		}
		times.arrayPush(values);
	}
}


FixMovesParser::FixMovesParser(const std::string &file)
:
	BodyMovesParser(file)
{
	// The order here is very important. If link1 doesn't 
	// go the first link, parser fails.
	_root = &_link1;
	_root->add(&_link2);
	_root->add(&_link3);
	_root->add(&_end);
}

void
FixMovesParser::parse()
{
	std::ifstream ifs (getFile().c_str(), std::ifstream::in);
	if (ifs.bad()) {
		ifs.close();
		std::cerr << "[Parser] Error parsing file " << getFile() << std::endl;
		return;
	}

	while (ifs.good()) {
		std::string line;
		getline(ifs, line);
		_root->handle(line, getNames(), getTimes(), getKeys());
	}

	ifs.close();
}