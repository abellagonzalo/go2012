#ifndef AleatoryMoveComponent_H
#define AleatoryMoveComponent_H

#include "Component.h"
#include "Singleton.h"
#include "Body.h"
#include "Dictionary.h"
#include <IceE/IceE.h>
#include <multimediaI.h>
#include <iostream>
#include <list>
#include <algorithm>
#include <time.h>
#include <sstream>
#include <IceE/IceE.h>
#include <demencia.h>


using namespace bica;

class AleatoryMoveComponent : public Component, public Singleton<AleatoryMoveComponent>, public AleatoryMove
{
public:

	AleatoryMoveComponent();
	virtual ~AleatoryMoveComponent();

	void init(const string newName, AL::ALPtr<AL::ALBroker> parentBroker);
	void step();

	void add(string move)			{ moves.push_back(move); };
	void remove(string move)		{ moves.remove(move); };
	std::list<string> get()		 	{ return moves; };
	void execute();
	bool isAvailable(string move);

	void setConfFile(string filePath) 	{ confFilePath = filePath; };
	string getConfFile() 				{ return confFilePath; };
	bool loadConfFile();
	bool saveConfFile();

	void add(const std::string& move, const Ice::Current& c)			{ add(move); };
	void remove(const std::string& move, const Ice::Current& c)			{ remove(move); };		
	StringSequence get(const Ice::Current& c)							{ return StringSequence(moves.begin(), moves.end()); };
	void execute(const Ice::Current& c)									{ execute(); };
	bool isAvailable(const std::string& move, const Ice::Current& c)	{ return isAvailable(move); };

	void setConfFile(const std::string& path, const Ice::Current& c)	{ setConfFile(path); };
	string getConfFile(const Ice::Current& c)							{ return getConfFile(); };
	bool loadConfFile(const Ice::Current& c)							{ return loadConfFile(); };
	bool saveConfFile(const Ice::Current& c)							{ return saveConfFile(); };
private:
	
	std::list<string> moves;
	Body *body;
	string confFilePath;

	string createKeyWithNumber(string key, int number);
};

#endif

