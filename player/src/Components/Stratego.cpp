#include "Stratego.h"

//BUILDER COMMENT. DO NOT REMOVE. auxinclude begin
//BUILDER COMMENT. DO NOT REMOVE. auxinclude end

Stratego::Stratego()
{
	_StrategyRoles = StrategyRoles::getInstance();
	_Striker2 = Striker2::getInstance();
	_BallDetector = BallDetector::getInstance();
	_Body = Body::getInstance();
	_Attention = Attention::getInstance();
	_Go2gpos = Go2gpos::getInstance();
	state = Initial;
}

Stratego::~Stratego()
{

}

void Stratego::Initial_state_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Initial_state_code

//BUILDER COMMENT. DO NOT REMOVE. end Initial_state_code
}

void Stratego::StrikerRol_state_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin StrikerRol_state_code
	//cout << "Striker\n";
	leds->setLed(Leds::REYE_LED, Leds::COLOR_WHITE, true);
//BUILDER COMMENT. DO NOT REMOVE. end StrikerRol_state_code
}

void Stratego::AttackerRol_state_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin AttackerRol_state_code
	leds->setLed(Leds::REYE_LED, Leds::COLOR_GREEN, true);


int x,y;

_StrategyRoles->getIdealSupporterPos(&x, &y);

cout << "AttackerRol -> "<<(float)x<<","<<(float)y<<endl;

ObjectState* balls = _BallDetector->getObj();

if(_BallDetector->ballmodel->elapsedTimeSinceLastObs < ObjectState::LONG_TIME)
{
	_Attention->setPercentage("Ball", 0.5);
	_Attention->setPercentage("Goal", 0.5);
	_Go2gpos->setGoalPosRelAng((float)x, (float)y, balls->getAngle());
	//_Body->setVelW(balls->getAngle()/1.5);
}else
{

	_Attention->setPercentage("Ball", 0.7);
	_Attention->setPercentage("Goal", 0.3);
	_Go2gpos->setGoalPosRelAng((float)x, (float)y, 0.2);
}



//BUILDER COMMENT. DO NOT REMOVE. end AttackerRol_state_code
}

void Stratego::DefenderRol_state_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin DefenderRol_state_code
leds->setLed(Leds::REYE_LED, Leds::COLOR_BLUE, true);



int x,y;

_StrategyRoles->getIdealDefensePos(&x, &y);

cout << "Defender -> "<<x<<","<<y<<endl;

ObjectState* balls = _BallDetector->getObj();

if(_BallDetector->ballmodel->elapsedTimeSinceLastObs < ObjectState::LONG_TIME)
{
	_Attention->setPercentage("Ball", 0.5);
	_Attention->setPercentage("Goal", 0.5);
	_Go2gpos->setGoalPosRelAng((float)x, (float)y, balls->getAngle());
	//_Body->setVelW(balls->getAngle()/1.5);
}else
{

	_Attention->setPercentage("Ball", 0.7);
	_Attention->setPercentage("Goal", 0.3);
	_Go2gpos->setGoalPosRelAng((float)x, (float)y, 0.2);
	//_Body->setVelW(0.2);
}

//BUILDER COMMENT. DO NOT REMOVE. end DefenderRol_state_code
}

void Stratego::RealInitial_state_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin RealInitial_state_code
//BUILDER COMMENT. DO NOT REMOVE. end RealInitial_state_code
}

bool Stratego::StrikerRol2DefenderRol0_transition_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin StrikerRol2DefenderRol0_transition_code
	return (StrategyRoles::getInstance()->getCurrentRole() == DEFENDER_ROLE);
//BUILDER COMMENT. DO NOT REMOVE. end StrikerRol2DefenderRol0_transition_code
}

bool Stratego::DefenderRol2StrikerRol0_transition_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin DefenderRol2StrikerRol0_transition_code
	return (StrategyRoles::getInstance()->getCurrentRole() == KICKER_ROLE);
//BUILDER COMMENT. DO NOT REMOVE. end DefenderRol2StrikerRol0_transition_code
}

bool Stratego::AttackerRol2StrikerRol0_transition_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin AttackerRol2StrikerRol0_transition_code
	return (StrategyRoles::getInstance()->getCurrentRole() == KICKER_ROLE);
//BUILDER COMMENT. DO NOT REMOVE. end AttackerRol2StrikerRol0_transition_code
}

bool Stratego::StrikerRol2AttackerRol0_transition_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin StrikerRol2AttackerRol0_transition_code
	return (StrategyRoles::getInstance()->getCurrentRole() == SUPPORTER_ROLE);
//BUILDER COMMENT. DO NOT REMOVE. end StrikerRol2AttackerRol0_transition_code
}

bool Stratego::DefenderRol2AttackerRol0_transition_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin DefenderRol2AttackerRol0_transition_code
	return (StrategyRoles::getInstance()->getCurrentRole() == SUPPORTER_ROLE);
//BUILDER COMMENT. DO NOT REMOVE. end DefenderRol2AttackerRol0_transition_code
}

bool Stratego::AttackerRol2DefenderRol0_transition_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin AttackerRol2DefenderRol0_transition_code
	return (StrategyRoles::getInstance()->getCurrentRole() == DEFENDER_ROLE);
//BUILDER COMMENT. DO NOT REMOVE. end AttackerRol2DefenderRol0_transition_code
}

bool Stratego::RealInitial2StrikerRol0_transition_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin RealInitial2StrikerRol0_transition_code
	return (StrategyRoles::getInstance()->getCurrentRole() == KICKER_ROLE);

//BUILDER COMMENT. DO NOT REMOVE. end RealInitial2StrikerRol0_transition_code
}

bool Stratego::RealInitial2AttackerRol0_transition_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin RealInitial2AttackerRol0_transition_code
	return (StrategyRoles::getInstance()->getCurrentRole() == SUPPORTER_ROLE);
//BUILDER COMMENT. DO NOT REMOVE. end RealInitial2AttackerRol0_transition_code
}

bool Stratego::RealInitial2DefenderRol0_transition_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin RealInitial2DefenderRol0_transition_code
	return (StrategyRoles::getInstance()->getCurrentRole() == DEFENDER_ROLE);
//BUILDER COMMENT. DO NOT REMOVE. end RealInitial2DefenderRol0_transition_code
}

bool Stratego::Initial2RealInitial0_transition_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Initial2RealInitial0_transition_code
return true;
//BUILDER COMMENT. DO NOT REMOVE. end Initial2RealInitial0_transition_code
}

void
Stratego::step(void)
{
	switch (state)
	{
	case Initial:

		if (isTime2Run()) {
			Initial_state_code();

			if (Initial2RealInitial0_transition_code()) {
				state = RealInitial;
			}
		}

		break;
	case StrikerRol:
		_StrategyRoles->step();

		if (isTime2Run()) {
			StrikerRol_state_code();

			if (StrikerRol2DefenderRol0_transition_code()) {
				state = DefenderRol;
			}
			else if (StrikerRol2AttackerRol0_transition_code()) {
				state = AttackerRol;
			}
		}

		_Striker2->step();
		break;
	case AttackerRol:
		_StrategyRoles->step();
		_BallDetector->step();

		if (isTime2Run()) {
			AttackerRol_state_code();

			if (AttackerRol2StrikerRol0_transition_code()) {
				state = StrikerRol;
			}
			else if (AttackerRol2DefenderRol0_transition_code()) {
				state = DefenderRol;
			}
		}

		_Body->step();
		_Attention->step();
		_Go2gpos->step();
		break;
	case DefenderRol:
		_StrategyRoles->step();
		_BallDetector->step();

		if (isTime2Run()) {
			DefenderRol_state_code();

			if (DefenderRol2StrikerRol0_transition_code()) {
				state = StrikerRol;
			}
			else if (DefenderRol2AttackerRol0_transition_code()) {
				state = AttackerRol;
			}
		}

		_Go2gpos->step();
		_Body->step();
		_Attention->step();
		break;
	case RealInitial:
		_StrategyRoles->step();

		if (isTime2Run()) {
			RealInitial_state_code();

			if (RealInitial2StrikerRol0_transition_code()) {
				state = StrikerRol;
			}
			else if (RealInitial2AttackerRol0_transition_code()) {
				state = AttackerRol;
			}
			else if (RealInitial2DefenderRol0_transition_code()) {
				state = DefenderRol;
			}
		}

		break;
	default:
		cout << "[Stratego::step()] Invalid state\n";
	}
}

//BUILDER COMMENT. DO NOT REMOVE. auxcode begin

void
Stratego::init(const string newName, AL::ALPtr<AL::ALBroker> parentBroker) {
	Component::init(newName, parentBroker);

	setFreqTime(LONG_RATE);

	leds = Leds::getInstance();
}
//BUILDER COMMENT. DO NOT REMOVE. auxcode end

