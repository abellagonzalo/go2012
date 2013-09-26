#include "AuxRole.h"

//BUILDER COMMENT. DO NOT REMOVE. auxinclude begin
//BUILDER COMMENT. DO NOT REMOVE. auxinclude end

AuxRole::AuxRole()
{
	_GoalieURJC = GoalieURJC::getInstance();
	_Stratego = Stratego::getInstance();

	state = Initial;
}

AuxRole::~AuxRole()
{

}

void AuxRole::Initial_state_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Initial_state_code
//BUILDER COMMENT. DO NOT REMOVE. end Initial_state_code
}

void AuxRole::Portero_state_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Portero_state_code
//BUILDER COMMENT. DO NOT REMOVE. end Portero_state_code
}

void AuxRole::Jugador_state_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Jugador_state_code
//BUILDER COMMENT. DO NOT REMOVE. end Jugador_state_code
}

bool AuxRole::Initial2Portero0_transition_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Initial2Portero0_transition_code
return (GameController::getInstance()->getPlayerNumber() == 1);
//BUILDER COMMENT. DO NOT REMOVE. end Initial2Portero0_transition_code
}

bool AuxRole::Initial2Jugador0_transition_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Initial2Jugador0_transition_code
return (GameController::getInstance()->getPlayerNumber() != 1);
//BUILDER COMMENT. DO NOT REMOVE. end Initial2Jugador0_transition_code
}

void
AuxRole::step(void)
{
	switch (state)
	{
	case Initial:

		if (isTime2Run()) {
			//startDebugInfo();
			Initial_state_code();

			if (Initial2Portero0_transition_code()) {
				state = Portero;
			}
			else if (Initial2Jugador0_transition_code()) {
				state = Jugador;
			}
			//endDebugInfo();
		}

		break;
	case Portero:

		if (isTime2Run()) {
			//startDebugInfo();
			Portero_state_code();
			//endDebugInfo();
		}

		_GoalieURJC->step();
		break;
	case Jugador:

		if (isTime2Run()) {
			//startDebugInfo();
			Jugador_state_code();
			//endDebugInfo();

		}

		_Stratego->step();
		break;
	default:
		cout << "[AuxRole::step()] Invalid state\n";
	}
}

//BUILDER COMMENT. DO NOT REMOVE. auxcode begin

void
AuxRole::init(const string newName, AL::ALPtr<AL::ALBroker> parentBroker) {
	Component::init(newName, parentBroker);

	setFreqTime(LONG_RATE);

}
//BUILDER COMMENT. DO NOT REMOVE. auxcode end


