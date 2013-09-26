#ifndef pruebag2gpos_H
#define pruebag2gpos_H

#include "Component.h"
#include "Localization.h"
#include "GoalDetector.h"
#include "Go2gpos.h"
#include "Body.h"
#include "Attention.h"
#include "Singleton.h"

//BUILDER COMMENT. DO NOT REMOVE. auxinclude begin
#include <IceE/IceE.h>
#include <componentsI.h>

using namespace bica;
//BUILDER COMMENT. DO NOT REMOVE. auxinclude end

class pruebag2gpos : public Component, public Singleton<pruebag2gpos>, public pruebag2gposManager
{
public:

	pruebag2gpos();
	~pruebag2gpos();

	void step();
private:

	static const int Initial	= 0;
	static const int Going	= 1;
	int state;

	Localization *_Localization;
	GoalDetector *_GoalDetector;
	Go2gpos *_Go2gpos;
	Body *_Body;
	Attention *_Attention;

	void Initial_state_code(void);
	void Going_state_code(void);
	bool Initial2Going0_transition_code(void);
//BUILDER COMMENT. DO NOT REMOVE. auxcode begin
public:
	void setGoalPos(double x, double y);
	void init(const string newName, AL::ALPtr<AL::ALBroker> parentBroker);
	virtual void setGoalPos( double x, double y, const Ice::Current& c);
private:
	double goalposx, goalposy;

//BUILDER COMMENT. DO NOT REMOVE. auxcode end
};

#endif

