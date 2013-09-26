#include "ApproachBall.h"

//BUILDER COMMENT. DO NOT REMOVE. auxinclude begin
//BUILDER COMMENT. DO NOT REMOVE. auxinclude end

ApproachBall::ApproachBall()
{
	_BallDetector = BallDetector::getInstance();
	_Go2pos = Go2pos::getInstance();
	state = Initial;
}

ApproachBall::~ApproachBall()
{

}

void ApproachBall::Initial_state_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Initial_state_code
//BUILDER COMMENT. DO NOT REMOVE. end Initial_state_code
}

void ApproachBall::BalNear_state_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin BalNear_state_code
ObjectState* balls = _BallDetector->getObj();
Vector2<double> pos = balls->getPositionInRelativeCoordinates();

float dx, dy;

if(pos.y>-2.0)
{
	dx = 190.0;
	dy = 65.0;
}else
{
	dx = 190.0;
	dy = -38.0;
}

if(pos.x<-200.0)
	_Go2pos->setGoalPosXY(pos.x-dx, 0.0, balls->getAngle());
else
	_Go2pos->setGoalPosXY(pos.x-dx, pos.y-dy, 0.0);


//BUILDER COMMENT. DO NOT REMOVE. end BalNear_state_code
}

void ApproachBall::BallFar_state_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin BallFar_state_code
ObjectState* balls = _BallDetector->getObj();
Vector2<double> pos = balls->getPositionInRelativeCoordinates();

float dx, dy;

dx = 180.0;
dy = -2.0;

//cerr<<"pelota a "<<balls->getAngle()<<endl;

_Go2pos->setGoalPosXY(pos.x-dx, 0.0, balls->getAngle());

//BUILDER COMMENT. DO NOT REMOVE. end BallFar_state_code
}

bool ApproachBall::BallFar22BalNear0_transition_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin BallFar22BalNear0_transition_code
ObjectState* balls = _BallDetector->getObj();
float rho = balls->getDistance();

if(rho < 400)
	return true;
else
	return false;
//BUILDER COMMENT. DO NOT REMOVE. end BallFar22BalNear0_transition_code
}

bool ApproachBall::BalNear2BallFar20_transition_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin BalNear2BallFar20_transition_code
ObjectState* balls = _BallDetector->getObj();
float rho = balls->getDistance();

if(rho > 500)
	return true;
else
	return false;
//BUILDER COMMENT. DO NOT REMOVE. end BalNear2BallFar20_transition_code
}

bool ApproachBall::Initial2BallFar20_transition_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Initial2BallFar20_transition_code
	return true;
//BUILDER COMMENT. DO NOT REMOVE. end Initial2BallFar20_transition_code
}

void
ApproachBall::step(void)
{
	switch (state)
	{
	case Initial:

		if (isTime2Run()) {
			startDebugInfo();
			Initial_state_code();

			if (Initial2BallFar20_transition_code()) {
				state = BallFar;
			}
			endDebugInfo();
		}

		break;
	case BalNear:
		_BallDetector->step();

		if (isTime2Run()) {
			startDebugInfo();
			BalNear_state_code();

			if (BalNear2BallFar20_transition_code()) {
				state = BallFar;
			}
			endDebugInfo();
		}

		_Go2pos->step();
		break;
	case BallFar:
		_BallDetector->step();

		if (isTime2Run()) {
			startDebugInfo();
			BallFar_state_code();

			if (BallFar22BalNear0_transition_code()) {
				state = BalNear;
			}
			endDebugInfo();
		}

		_Go2pos->step();
		break;
	default:
		cout << "[ApproachBall::step()] Invalid state\n";
	}
}

//BUILDER COMMENT. DO NOT REMOVE. auxcode begin

void
ApproachBall::init(const string newName, AL::ALPtr<AL::ALBroker> parentBroker) {
	Component::init(newName, parentBroker);

	setFreqTime(SHORT_RATE);

}
//BUILDER COMMENT. DO NOT REMOVE. auxcode end

