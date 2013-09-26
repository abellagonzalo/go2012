#ifndef SearchNet_H
#define SearchNet_H

#include "Component.h"
#include "Localization.h"
#include "GoalDetector.h"
#include "Attention.h"
#include "Singleton.h"

//BUILDER COMMENT. DO NOT REMOVE. auxinclude begin
//BUILDER COMMENT. DO NOT REMOVE. auxinclude end

class SearchNet : public Component, public Singleton<SearchNet>
{
public:

	SearchNet();
	~SearchNet();

	void step();
private:

	static const int Initial	= 0;
	static const int StateS	= 1;
	int state;

	Localization *_Localization;
	GoalDetector *_GoalDetector;
	Attention *_Attention;

	void Initial_state_code(void);
	void StateS_state_code(void);
	bool Initial2StateS0_transition_code(void);
//BUILDER COMMENT. DO NOT REMOVE. auxcode begin
public:
	void restart();
	bool finished();

	float initTilt, initPan;
	bool  fin;
//BUILDER COMMENT. DO NOT REMOVE. auxcode end
};

#endif

