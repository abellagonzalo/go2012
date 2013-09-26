#include "BodyMovesParserFactory.h"
 
BezierMoveParser* 
BodyMovesParserFactory::createBezierMovesParser(const std::string &file)	
{ 
	return new BezierMoveParser(file); 
}
 
OldMovesParser* 
BodyMovesParserFactory::createOldMovesParser(const std::string &file)		
{ 
	return new OldMovesParser(file); 
}

FixMovesParser*
BodyMovesParserFactory::createFixMovesParser(const std::string &file)
{
	return new FixMovesParser(file);
}
 
BodyMovesParser* 
BodyMovesParserFactory::createBodyMovesParser(const std::string &file)		
{
	std::string movExtension(".mov");
	std::string kickExtension(".kick");
	std::string fixExtension(".fix");

	if (file.find(movExtension) != std::string::npos)
		return static_cast<BodyMovesParser*>(createBezierMovesParser(file));

	else if (file.find(kickExtension) != std::string::npos)
		return static_cast<OldMovesParser*>(createOldMovesParser(file));

	else if (file.find(fixExtension) != std::string::npos)
		return static_cast<FixMovesParser*>(createFixMovesParser(file));
	
	return NULL;	
}
