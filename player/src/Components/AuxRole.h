#ifndef AuxRole_H
#define AuxRole_H

#include "Component.h"
#include "GoalieURJC.h"
#include "Stratego.h"
#include "Singleton.h"

//BUILDER COMMENT. DO NOT REMOVE. auxinclude begin
//BUILDER COMMENT. DO NOT REMOVE. auxinclude end

class AuxRole : public Component, public Singleton<AuxRole>
{
public:

	AuxRole();
	~AuxRole();

	void step();
private:

	static const int Initial	= 0;
	static const int Portero	= 1;
	static const int Jugador	= 2;
	int state;

	GoalieURJC *_GoalieURJC;
	Stratego *_Stratego;

	void Initial_state_code(void);
	void Portero_state_code(void);
	void Jugador_state_code(void);
	bool Initial2Portero0_transition_code(void);
	bool Initial2Jugador0_transition_code(void);
//BUILDER COMMENT. DO NOT REMOVE. auxcode begin
public:
	void init(const string newName, AL::ALPtr<AL::ALBroker> parentBroker);
//BUILDER COMMENT. DO NOT REMOVE. auxcode end
};

#endif

