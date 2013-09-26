#include "FollowBall.h"

//BUILDER COMMENT. DO NOT REMOVE. auxinclude begin
//BUILDER COMMENT. DO NOT REMOVE. auxinclude end

FollowBall::FollowBall()
{
	_BallDetector = BallDetector::getInstance();
	_Body = Body::getInstance();

	ctrlW = new PIDController("W", 0.0, 0.5, 0.0, 1.0);
	ctrlW->setPIDKs(1.0, 0.2, 0.1);
	ctrlL = new PIDController("L", 0.0, 0.5, 0.0, 1.0);
	ctrlL->setPIDKs(1.0, 0.1, 0.2);
	ctrlV = new PIDController("V", 0.0, 0.5, 0.0, 1.0);
	ctrlV->setPIDKs(1.0, 0.1, 0.2);
	state = Initial;
}

FollowBall::~FollowBall()
{

}

void FollowBall::Initial_state_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Initial_state_code
//BUILDER COMMENT. DO NOT REMOVE. end Initial_state_code
}

void FollowBall::NotSeeingBall_state_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin NotSeeingBall_state_code
_Body->setVel(0.0, 0.0, 0.0);
//BUILDER COMMENT. DO NOT REMOVE. end NotSeeingBall_state_code
}

void FollowBall::SeeingBall_state_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin SeeingBall_state_code
ObjectState* balls = _BallDetector->getObj();
Vector2<double> pos = balls->getPositionInRelativeCoordinates();

float v,w,l;
float theta = balls->getAngle();
float rho = balls->getDistance();

l = 0.0;
v = 0.0;

ctrlW->setReference(theta);
ctrlL->setReference(pos.y/200.0);
ctrlV->setReference((pos.x-700.0)/200.0);
w = ctrlW->getOutput();
l = ctrlL->getOutput();
v = ctrlV->getOutput();

cerr<<"["<<v<<","<<l<<","<<w<<"]\t("<<pos.x<<","<<pos.y<<")"<<endl;
//cerr<<theta/M_PI<<" -> "<<w<<endl;

/*

	w = theta / 2.5;

	if(fabs(theta)<toRadians(20.0))
	{
		if(rho>600.0)
		{
			v = 1.0;
		}else if(rho>150.0)
		{
			v = rho/600.0;
		}else
			v = 0.0;
	}else
		v = 0.0;


if(v > 1.0) v = 1.0;
if(v < -1.0) v = -1.0;
if(w > 1.0) w = 1.0;
if(w < -1.0) w = -1.0;
if(l > 1.0) l = 1.0;
if(l < -1.0) l = -1.0;
*/
_Body->setVel(v,w,l);

//BUILDER COMMENT. DO NOT REMOVE. end SeeingBall_state_code
}

bool FollowBall::BallInitiallyNotSeen_transition_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin BallInitiallyNotSeen_transition_code
if(!_BallDetector->seen())
	return true;
else
	return false;
//BUILDER COMMENT. DO NOT REMOVE. end BallInitiallyNotSeen_transition_code
}

bool FollowBall::BallInitiallySeen_transition_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin BallInitiallySeen_transition_code
if(_BallDetector->seen())
	return true;
else
	return false;
//BUILDER COMMENT. DO NOT REMOVE. end BallInitiallySeen_transition_code
}

bool FollowBall::BallLost_transition_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin BallLost_transition_code
return _BallDetector->ballmodel->elapsedTimeSinceLastObs > ObjectState::CURRENTLY;
//BUILDER COMMENT. DO NOT REMOVE. end BallLost_transition_code
}

bool FollowBall::BallFound_transition_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin BallFound_transition_code
if(_BallDetector->ballmodel->elapsedTimeSinceLastObs < ObjectState::CURRENTLY)
	return true;
else
	return false;
//BUILDER COMMENT. DO NOT REMOVE. end BallFound_transition_code
}

void
FollowBall::step(void)
{
	switch (state)
	{
	case Initial:
		_BallDetector->step();

		if (isTime2Run()) {
			//startDebugInfo();
			Initial_state_code();

			if (BallInitiallyNotSeen_transition_code()) {
				state = NotSeeingBall;
			}
			else if (BallInitiallySeen_transition_code()) {
				state = SeeingBall;
			}
			//endDebugInfo();
		}

		break;
	case NotSeeingBall:
		_BallDetector->step();

		if (isTime2Run()) {
			//startDebugInfo();
			NotSeeingBall_state_code();

			if (BallFound_transition_code()) {
				state = SeeingBall;
			}
			//endDebugInfo();
		}

		_Body->step();
		break;
	case SeeingBall:
		_BallDetector->step();

		if (isTime2Run()) {
			SeeingBall_state_code();

			if (BallLost_transition_code()) {
				state = NotSeeingBall;
			}
		}

		_Body->step();
		break;
	default:
		cout << "[FollowBall::step()] Invalid state\n";
	}
}

//BUILDER COMMENT. DO NOT REMOVE. auxcode begin

void
FollowBall::init(const string newName, AL::ALPtr<AL::ALBroker> parentBroker)
{
	Component::init(newName, parentBroker);

	setFreqTime(SHORT_RATE);
}

//BUILDER COMMENT. DO NOT REMOVE. auxcode end


