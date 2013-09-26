#include "PruebaAttention.h"

//BUILDER COMMENT. DO NOT REMOVE. auxinclude begin
//BUILDER COMMENT. DO NOT REMOVE. auxinclude end

PruebaAttention::PruebaAttention()
{
	_Attention = Attention::getInstance();
	_BallDetector = BallDetector::getInstance();
	_GoalDetector = GoalDetector::getInstance();
	_Body = Body::getInstance();

	state = Initial;
}

PruebaAttention::~PruebaAttention()
{

}

void PruebaAttention::Initial_state_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Initial_state_code
//BUILDER COMMENT. DO NOT REMOVE. end Initial_state_code
}

void PruebaAttention::Paso1_state_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Paso1_state_code
_Attention->setPercentage("Ball", 0.5);
_Attention->setPercentage("Goal", 0.5);
//_Attention->setPercentage("Wave", 1.0);
//BUILDER COMMENT. DO NOT REMOVE. end Paso1_state_code
}

void PruebaAttention::paso2_state_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin paso2_state_code
_Attention->setPercentage("Ball", 0.5);
_Attention->setPercentage("Goal", 0.5);
//BUILDER COMMENT. DO NOT REMOVE. end paso2_state_code
}

bool PruebaAttention::Initial2Visualizando0_transition_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Initial2Visualizando0_transition_code
return true;
//BUILDER COMMENT. DO NOT REMOVE. end Initial2Visualizando0_transition_code
}

bool PruebaAttention::Paso12paso20_transition_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Paso12paso20_transition_code
return false;
//BUILDER COMMENT. DO NOT REMOVE. end Paso12paso20_transition_code
}

bool PruebaAttention::paso22Paso10_transition_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin paso22Paso10_transition_code
return true;
//BUILDER COMMENT. DO NOT REMOVE. end paso22Paso10_transition_code
}

void
PruebaAttention::step(void)
{
	switch (state)
	{
	case Initial:

		if (isTime2Run()) {
			Initial_state_code();

			if (Initial2Visualizando0_transition_code()) {
				state = Paso1;
				resetStopWatch();
			}
		}

		break;
	case Paso1:
		_BallDetector->step();
		_GoalDetector->step();

		if (isTime2Run()) {
			Paso1_state_code();

			if (Paso12paso20_transition_code()) {
				state = paso2;
				resetStopWatch();
			}
		}

		_Attention->step();
		break;
	case paso2:
		_BallDetector->step();
		_GoalDetector->step();

		if (isTime2Run()) {
			paso2_state_code();

			if (paso22Paso10_transition_code()) {
				state = Paso1;
				resetStopWatch();
			}
		}

		_Body->step();
		_Attention->step();
		break;
	default:
		cout << "[PruebaAttention::step()] Invalid state\n";
	}
}

//BUILDER COMMENT. DO NOT REMOVE. auxcode begin
//BUILDER COMMENT. DO NOT REMOVE. auxcode end


