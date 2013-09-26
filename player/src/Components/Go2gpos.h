#ifndef Go2gpos_H
#define Go2gpos_H

#include "Component.h"
#include "WorldState.h"
#include "Go2pos.h"
#include "Body.h"
#include "Singleton.h"

//BUILDER COMMENT. DO NOT REMOVE. auxinclude begin
#include <IceE/IceE.h>
#include <componentsI.h>

using namespace bica;
//BUILDER COMMENT. DO NOT REMOVE. auxinclude end

class Go2gpos : public Component, public Singleton<Go2gpos>, public Go2posManager
{
public:

	Go2gpos();
	~Go2gpos();

	void step();
private:

	static const int Initial	= 0;
	static const int Go2Pos	= 1;
	int state;

	WorldState *_WorldState;
	Go2pos *_Go2pos;
	Body *_Body;

	void Initial_state_code(void);
	void Go2Pos_state_code(void);
	bool InitiallyLocalized_transition_code(void);
//BUILDER COMMENT. DO NOT REMOVE. auxcode begin
public:
	void setGoalPosRelAng(double x, double y, double t);
	void setGoalPosAbsAng(double x, double y, double t);
	void init(const string newName, AL::ALPtr<AL::ALBroker> parentBroker);
	virtual void setGoalPos( double x, double y, const Ice::Current& c);
private:
	double goalposx, goalposy, goalpost;
	bool absolute;

//BUILDER COMMENT. DO NOT REMOVE. auxcode end
};

#endif

