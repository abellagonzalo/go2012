#ifndef BodyMovesParserFactory_H
#define BodyMovesParserFactory_H

#include "BodyMovesParser.h"
#include "BezierMoveParser.h"
#include "OldMovesParser.h"
#include "FixMovesParser.h"

/**
* Factory to create BodyMovesParsers.
* Important! All methods return a pointer to a parser 
* already instantiated. Don't forget to delete it after 
* using it.
*/
class BodyMovesParserFactory
{
	public:
		/** Returns a BezierMoveParser pointer instantiated */
		static BezierMoveParser* createBezierMovesParser(const std::string &file);

		/** Returns an OldMovesParser pointer instantiated */
		static OldMovesParser* createOldMovesParser(const std::string &file);

		/** Returns a FixMovesParser pointer instantiated */
		static FixMovesParser* createFixMovesParser(const std::string &file);

		/** Returns a generic BodyMovesParser depending on the file extension */
		static BodyMovesParser* createBodyMovesParser(const std::string &file);
};

#endif // BodyMovesParserFactory_H
