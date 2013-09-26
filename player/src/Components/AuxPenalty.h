#ifndef AuxPenalty_H
#define AuxPenalty_H

#include "Component.h"
#include "PGoalie.h"
#include "Striker2.h"
#include "WorldState.h"

#include "Singleton.h"

//BUILDER COMMENT. DO NOT REMOVE. auxinclude begin
//BUILDER COMMENT. DO NOT REMOVE. auxinclude end

class AuxPenalty : public Component, public Singleton<AuxPenalty>
{
public:

	AuxPenalty();
	~AuxPenalty();

	void step();
private:

	static const int Initial	= 0;
	static const int Goalie	= 1;
	static const int Kicker	= 2;
	int state;

	PGoalie *_PGoalie;
	Striker2 *_Striker2;
	WorldState *_WorldState;

	void Initial_state_code(void);
	void Goalie_state_code(void);
	void Kicker_state_code(void);
	bool Initial2Goalie0_transition_code(void);
	bool Goalie2Initial0_transition_code(void);
	bool Kicker2Initial0_transition_code(void);
	bool Initial2Kicker0_transition_code(void);
//BUILDER COMMENT. DO NOT REMOVE. auxcode begin
public:

	void restart();
//BUILDER COMMENT. DO NOT REMOVE. auxcode end
};

#endif

