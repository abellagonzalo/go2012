#include "AuxPenalty.h"

//BUILDER COMMENT. DO NOT REMOVE. auxinclude begin
//BUILDER COMMENT. DO NOT REMOVE. auxinclude end

AuxPenalty::AuxPenalty()
{
	_PGoalie = PGoalie::getInstance();

	_Striker2 = Striker2::getInstance();
	_WorldState = WorldState::getInstance();

	state = Initial;
}

AuxPenalty::~AuxPenalty()
{

}

void AuxPenalty::Initial_state_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Initial_state_code
//BUILDER COMMENT. DO NOT REMOVE. end Initial_state_code
}

void AuxPenalty::Goalie_state_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Goalie_state_code
//cerr<<"Penalty Goalie"<<endl;
	_WorldState->inMyField();

//BUILDER COMMENT. DO NOT REMOVE. end Goalie_state_code
}

void AuxPenalty::Kicker_state_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Kicker_state_code
//cerr<<"Penalty Kicker"<<endl;
	_WorldState->inOppField();

//BUILDER COMMENT. DO NOT REMOVE. end Kicker_state_code
}

bool AuxPenalty::Initial2Goalie0_transition_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Initial2Goalie0_transition_code

if(!GameController::getInstance()->isMyKickOff())
{
	_PGoalie->restart();
	return true;
}else
	return false;

//BUILDER COMMENT. DO NOT REMOVE. end Initial2Goalie0_transition_code
}

bool AuxPenalty::Goalie2Initial0_transition_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Goalie2Initial0_transition_code

if(GameController::getInstance()->isMyKickOff())
{
	return true;
}else
	return false;

//BUILDER COMMENT. DO NOT REMOVE. end Goalie2Initial0_transition_code
}

bool AuxPenalty::Kicker2Initial0_transition_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Kicker2Initial0_transition_code

if(!GameController::getInstance()->isMyKickOff())
{
	return true;
}else
	return false;

//BUILDER COMMENT. DO NOT REMOVE. end Kicker2Initial0_transition_code
}

bool AuxPenalty::Initial2Kicker0_transition_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Initial2Kicker0_transition_code

if(GameController::getInstance()->isMyKickOff())
{

	return true;
}else
	return false;

//BUILDER COMMENT. DO NOT REMOVE. end Initial2Kicker0_transition_code
}

void
AuxPenalty::step(void)
{
	switch (state)
	{
	case Initial:

		if (isTime2Run()) {
			Initial_state_code();

			if (Initial2Goalie0_transition_code()) {
				state = Goalie;
			}
			else if (Initial2Kicker0_transition_code()) {
				state = Kicker;
			}
			_WorldState->step();
		}

		break;
	case Goalie:

		if (isTime2Run()) {
			Goalie_state_code();

			if (Goalie2Initial0_transition_code()) {
				state = Initial;
			}
		}
		_WorldState->step();
		_PGoalie->step();
		break;
	case Kicker:

		if (isTime2Run()) {
			Kicker_state_code();

			if (Kicker2Initial0_transition_code()) {
				state = Initial;
			}
		}
		_Striker2->step();
		_WorldState->step();

		break;
	default:
		cout << "[AuxPenalty::step()] Invalid state\n";
	}
}

//BUILDER COMMENT. DO NOT REMOVE. auxcode begin
void AuxPenalty::restart()
{

	cerr<<"GOALIE RESTARTED"<<endl;
	_PGoalie->restart();
	state = Initial;
}
//BUILDER COMMENT. DO NOT REMOVE. auxcode end

