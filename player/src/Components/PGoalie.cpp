#include "PGoalie.h"

//BUILDER COMMENT. DO NOT REMOVE. auxinclude begin
//BUILDER COMMENT. DO NOT REMOVE. auxinclude end

PGoalie::PGoalie()
{
	_BallDetector = BallDetector::getInstance();
	_Attention = Attention::getInstance();
	_Body = Body::getInstance();

	state = Initial;
}

PGoalie::~PGoalie()
{

}

void PGoalie::Initial_state_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Initial_state_code
//BUILDER COMMENT. DO NOT REMOVE. end Initial_state_code
}

void PGoalie::Esperar_state_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Esperar_state_code
	_Attention->setPercentage("Ball", 1.0);
	_Attention->setPercentage("Goal", 0.0);

//BUILDER COMMENT. DO NOT REMOVE. end Esperar_state_code
}

void PGoalie::Tirarse_state_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Tirarse_state_code
//BUILDER COMMENT. DO NOT REMOVE. end Tirarse_state_code
}

void PGoalie::Levantarse_state_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Levantarse_state_code
//BUILDER COMMENT. DO NOT REMOVE. end Levantarse_state_code
}

void PGoalie::Tirado_state_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Tirado_state_code
//BUILDER COMMENT. DO NOT REMOVE. end Tirado_state_code
}

bool PGoalie::Initial2Esperar0_transition_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Initial2Esperar0_transition_code
return true;
//BUILDER COMMENT. DO NOT REMOVE. end Initial2Esperar0_transition_code
}

bool PGoalie::Esperar2Tirarse0_transition_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Esperar2Tirarse0_transition_code
ObjectState* balls = _BallDetector->getObj();

if((_BallDetector->ballmodel->elapsedTimeSinceLastObs < ObjectState::RECENTLY)&&
(balls->getDistance()<1200.0))
{
_Body->selKick("TXTSUICIDE");
return true;
}else
return false;
//BUILDER COMMENT. DO NOT REMOVE. end Esperar2Tirarse0_transition_code
}

bool PGoalie::Tirarse2Tirado0_transition_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Tirarse2Tirado0_transition_code
return true;
//BUILDER COMMENT. DO NOT REMOVE. end Tirarse2Tirado0_transition_code
}

bool PGoalie::Tirado2Levantarse0_transition_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Tirado2Levantarse0_transition_code
return (getStopWatch()>4000);
//BUILDER COMMENT. DO NOT REMOVE. end Tirado2Levantarse0_transition_code
}

void
PGoalie::step(void)
{
	switch (state)
	{
	case Initial:

		if (isTime2Run()) {
			Initial_state_code();

			if (Initial2Esperar0_transition_code()) {
				state = Esperar;
			}
		}

		break;
	case Esperar:
		_BallDetector->step();

		if (isTime2Run()) {
			Esperar_state_code();

			if (Esperar2Tirarse0_transition_code()) {
				state = Tirarse;
			}
		}

		_Attention->step();
		_Body->step();
		break;
	case Tirarse:

		if (isTime2Run()) {
			Tirarse_state_code();

			if (Tirarse2Tirado0_transition_code()) {
				state = Tirado;
				resetStopWatch();
			}
		}

		_Body->step();
		break;
	case Levantarse:

		if (isTime2Run()) {
			Levantarse_state_code();

		}

		_Body->step();
		break;
	case Tirado:

		if (isTime2Run()) {
			Tirado_state_code();

			if (Tirado2Levantarse0_transition_code()) {
				state = Levantarse;
			}
		}

		break;
	default:
		cout << "[PGoalie::step()] Invalid state\n";
	}
}

//BUILDER COMMENT. DO NOT REMOVE. auxcode begin

void PGoalie::restart()
{
	state = Initial;
}
//BUILDER COMMENT. DO NOT REMOVE. auxcode end


