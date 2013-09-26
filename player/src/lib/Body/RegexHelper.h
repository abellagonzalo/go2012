#ifndef RegexHelper_H
#define RegexHelper_H

#include <string>
#include <stdlib.h>

/**
* Class with some methods to help with regex expressions.
*/
class RegexHelper
{
public:
	static const std::string floatVar;
	static const std::string intVar;
	static const std::string stringVar;
	static const std::string openBraq;
	static const std::string closeBraq;
	static const std::string openPar;
	static const std::string closePar;
	static const std::string dot;
	static const std::string semicolon;
	static const std::string invCommas;
	static const std::string anything;
	static const std::string plus;

	int stringToInt(std::string s) 			{ return atoi(s.c_str()); };
	double stringToDouble(std::string s)	{ return atof(s.c_str()); };
};

#endif // RegexHelper_H
