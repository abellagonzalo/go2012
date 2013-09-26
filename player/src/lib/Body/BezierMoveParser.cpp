#include "BezierMoveParser.h"
#include <boost/tr1/regex.hpp>
#include <iostream>
#include <fstream>

void 
BezierMoveParser::BezierMoveParserElement1::handle(std::string& line,
							 				   std::vector<std::string>& names,
							 				   AL::ALValue& times,
							 				   AL::ALValue& keys)
{
	// Regular expression
	const std::tr1::regex pattern(stringVar + dot + stringVar + openPar + intVar + closePar + semicolon);

	// Sequence of submatches
	std::tr1::match_results<std::string::const_iterator> result;

	// Matching
	bool valid = std::tr1::regex_match(line, result, pattern);

	if (valid) {
		//std::cout << "[BezierMoveParserElement1]: " << result[1] << " - " << result[2] << " - " << result[3] << std::endl;
		std::string s1 = result[1].str();
		std::string s2 = result[2].str();
		int n3 = stringToInt(result[3].str());

		if 		((s1=="names") && (s2=="reserve")) {
			names.reserve(n3);

		} else if ((s2=="arraySetSize")) {
			if 		(s1=="times") times.arraySetSize(n3);
			else if (s1=="keys") keys.arraySetSize(n3);
		}

	} else {
		ParserElement::handle(line, names, times, keys);
	}
}

void 
BezierMoveParser::BezierMoveParserElement2::handle(std::string& line,
							 std::vector<std::string>& names,
							 AL::ALValue& times,
							 AL::ALValue& keys)
{
	// Regular expression
	const std::tr1::regex pattern("names" + dot + "push_back" + openPar + invCommas + stringVar + invCommas + closePar + semicolon);

	// Sequence of submatches
	std::tr1::match_results<std::string::const_iterator> result;

	// Matching
	bool valid = std::tr1::regex_match(line, result, pattern);

	if (valid) {
		//std::cout << "[BezierMoveParserElement2]: " << result[1] << std::endl;
		names.push_back(result[1].str());
	} else {
		ParserElement::handle(line, names, times, keys);
	}
}

void 
BezierMoveParser::BezierMoveParserElement3::handle(std::string& line,
							 std::vector<std::string>& names,
							 AL::ALValue& times,
							 AL::ALValue& keys)
{
	// Regular expression
	const std::tr1::regex pattern(stringVar + openBraq + intVar + closeBraq + dot + "arraySetSize" + openPar + intVar + closePar + semicolon);

	// Sequence of submatches
	std::tr1::match_results<std::string::const_iterator> result;

	// Matching
	bool valid = std::tr1::regex_match(line, result, pattern);

	if (valid) {
		//std::cout << "[BezierMoveParserElement3]: " << result[1]  << " - " << result[2] << " - " << result[3] << std::endl;
		std::string s1 = result[1].str();
		int n2 = stringToInt(result[2].str());
		int n3 = stringToInt(result[3].str());

		if 		(s1 == "times") times[n2].arraySetSize(n3);
		else if (s1 == "keys")  keys[n2].arraySetSize(n3);

	} else {
		ParserElement::handle(line, names, times, keys);
	}
}

void 
BezierMoveParser::BezierMoveParserElement4::handle(std::string& line,
							 std::vector<std::string>& names,
							 AL::ALValue& times,
							 AL::ALValue& keys)
{
	// Regular expression
	const std::tr1::regex pattern("times" + openBraq + intVar + closeBraq + openBraq + intVar + closeBraq + " = " + floatVar + semicolon);

	// Sequence of submatches
	std::tr1::match_results<std::string::const_iterator> result;

	// Matching
	bool valid = std::tr1::regex_match(line, result, pattern);

	if (valid) {
		//std::cout << "[BezierMoveParserElement4]: " << result[1]  << " - " << result[2] << " - " << result[3] << std::endl;
		int n1 = stringToInt(result[1].str());
		int n2 = stringToInt(result[2].str());
		double f3 = stringToDouble(result[3].str());
		times[n1][n2] = f3;
	} else {
		ParserElement::handle(line, names, times, keys);
	}
}

void 
BezierMoveParser::BezierMoveParserElement5::handle(std::string& line,
							 std::vector<std::string>& names,
							 AL::ALValue& times,
							 AL::ALValue& keys)
{
	// Regular expression
	std::string alvalue1 = "AL::ALValue::array" + openPar + intVar + ", " + floatVar + ", " + floatVar + closePar;	
	std::string alvalue2 = "AL::ALValue::array" + openPar + floatVar + ", " + alvalue1 + ", " + alvalue1 + closePar + ";";
	const std::tr1::regex pattern("keys" + openBraq + intVar + closeBraq + openBraq + intVar + closeBraq + " = " + alvalue2);

	// Sequence of submatches
	std::tr1::match_results<std::string::const_iterator> result;

	// Matching
	bool valid = std::tr1::regex_match(line, result, pattern);

	if (valid) {
		//std::cout << "[BezierMoveParserElement5]: " << result[1]  << " - " << result[2] << " - " << result[3] << " - " << result[4]  << " - " << result[5] << " - " << result[6] << " - " << result[7]  << " - " << result[8] << " - " << result[9] << std::endl;
		int n1 = stringToInt(result[1].str());
		int n2 = stringToInt(result[2].str());
		double f3 = stringToDouble(result[3].str());
		int n4 = stringToInt(result[4].str());
		double f5 = stringToDouble(result[5].str());
		double f6 = stringToDouble(result[6].str());
		int n7 = stringToInt(result[7].str());
		double f8 = stringToDouble(result[8].str());
		double f9 = stringToDouble(result[9].str());
		keys[n1][n2] = AL::ALValue::array(f3, AL::ALValue::array(n4, f5, f6), AL::ALValue::array(n7, f8, f9));
	} else {
		ParserElement::handle(line, names, times, keys);
	}
}

/*  BezierMoveParser methods  */
BezierMoveParser::BezierMoveParser(const std::string &file) : BodyMovesParser(file) 
{
	// Elements of the chain are sorted by
	// the number of times they are called.
	// First those which are called more often.
	_root = &_link5;
	_root->add(&_link4);
	_root->add(&_link3);
	_root->add(&_link2);
	_root->add(&_link1);
	_root->add(&_end);
}

void
BezierMoveParser::parse()
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
