#include "RegexHelper.h"

const std::string RegexHelper::floatVar		= "(-?\\d+\\.\\d+|-?\\d+\\.\\d+[eE][+-]?\\d+)";
const std::string RegexHelper::intVar 		= "(\\d+)";
const std::string RegexHelper::stringVar	= "(\\w+)";
const std::string RegexHelper::openBraq 	= "\\[";
const std::string RegexHelper::closeBraq	= "\\]";
const std::string RegexHelper::openPar 		= "\\(";
const std::string RegexHelper::closePar 	= "\\)";
const std::string RegexHelper::dot			= "\\.";
const std::string RegexHelper::semicolon	= ";";
const std::string RegexHelper::invCommas	= "\"";
const std::string RegexHelper::anything		= ".*?";
const std::string RegexHelper::plus			= "\\+";
