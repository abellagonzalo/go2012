#include "SearchNet.h"

//BUILDER COMMENT. DO NOT REMOVE. auxinclude begin
//BUILDER COMMENT. DO NOT REMOVE. auxinclude end

SearchNet::SearchNet()
{
	_Localization = Localization::getInstance();
	_GoalDetector = GoalDetector::getInstance();
	_Attention = Attention::getInstance();

	state = Initial;
}

SearchNet::~SearchNet()
{

}

void SearchNet::Initial_state_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Initial_state_code
_Attention->setPercentage("Goal", 1.0);
//BUILDER COMMENT. DO NOT REMOVE. end Initial_state_code
}

void SearchNet::StateS_state_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin StateS_state_code
_Attention->setPercentage("Goal", 1.0);
//BUILDER COMMENT. DO NOT REMOVE. end StateS_state_code
}

bool SearchNet::Initial2StateS0_transition_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Initial2StateS0_transition_code
return true;
//BUILDER COMMENT. DO NOT REMOVE. end Initial2StateS0_transition_code
}

void
SearchNet::step(void)
{

	switch (state)
	{
	case Initial:

		if (isTime2Run()) {
			Initial_state_code();

			if (Initial2StateS0_transition_code()) {
				state = StateS;
			}
		}

		break;
	case StateS:
		_Localization->step();
		_GoalDetector->step();

		if (isTime2Run()) {
			StateS_state_code();

		}

		_Attention->step();
		break;
	default:
		cout << "[SearchNet::step()] Invalid state\n";
	}
}

//BUILDER COMMENT. DO NOT REMOVE. auxcode begin
bool
SearchNet::finished(void)
{
	return fin;
}

void
SearchNet::restart()
{
	fin = false;
	state = Initial;
}

//BUILDER COMMENT. DO NOT REMOVE. auxcode end


