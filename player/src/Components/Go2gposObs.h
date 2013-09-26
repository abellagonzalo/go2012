#ifndef Go2gposObs_H
#define Go2gposObs_H

#include "Component.h"
#include "GTLocalization.h"
#include "Body.h"
#include "Singleton.h"

//BUILDER COMMENT. DO NOT REMOVE. auxinclude begin
#include <IceE/IceE.h>
#include <componentsI.h>

using namespace bica;
//BUILDER COMMENT. DO NOT REMOVE. auxinclude end

class Go2gposObs : public Component, public Singleton<Go2gposObs>, public Go2posManager
{
public:

	Go2gposObs();
	~Go2gposObs();

	void step();
private:

	static const int Initial	= 0;
	static const int Localized	= 1;
	static const int NotLocalized	= 2;
	int state;

	GTLocalization *_GTLocalization;
	//Obstacles *_Obstacles;
	Body *_Body;

	void Initial_state_code(void);
	void Localized_state_code(void);
	void NotLocalized_state_code(void);
	bool InitiallyLocalized_transition_code(void);
	bool Lost_transition_code(void);
	bool Found_transition_code(void);
	bool InitialllyNotLocalized_transition_code(void);
//BUILDER COMMENT. DO NOT REMOVE. auxcode begin

public:

	void init(const string newName, AL::ALPtr<AL::ALBroker> parentBroker);

	void setGoalPos(double x, double y);
	virtual void setGoalPos( double x, double y, const Ice::Current& c);
	string getDebugData(void);
private:

	double goalposx, goalposy;
	double robotposx, robotposy;
	double goalvx, goalvy;

//BUILDER COMMENT. DO NOT REMOVE. auxcode end
};

#endif

