#include "SearchBall.h"

//BUILDER COMMENT. DO NOT REMOVE. auxinclude begin
//BUILDER COMMENT. DO NOT REMOVE. auxinclude end

SearchBall::SearchBall()
{
	_BallDetector = BallDetector::getInstance();
	_Body = Body::getInstance();
	_Attention = Attention::getInstance();

	state = Initial;
}

SearchBall::~SearchBall()
{

}

void SearchBall::Initial_state_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Initial_state_code
//BUILDER COMMENT. DO NOT REMOVE. end Initial_state_code
}

void SearchBall::RotateBody_state_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin RotateBody_state_code
//cerr<<"SB::RotateBody"<<endl;

	_Attention->setPercentage("Ball", 1.0);
	_Attention->setPercentage("Goal", 0.0);
	_Body->setVel(0.0, w, 0.0);
//BUILDER COMMENT. DO NOT REMOVE. end RotateBody_state_code
}

void SearchBall::Stopped_state_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Stopped_state_code
	_Body->setVel(0.0, 0.0, 0.0);
	_Attention->setPercentage("Ball", 1.0);
	_Attention->setPercentage("Goal", 0.0);

//BUILDER COMMENT. DO NOT REMOVE. end Stopped_state_code
}

bool SearchBall::Initial2Stopped0_transition_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Initial2Stopped0_transition_code
return true;
//BUILDER COMMENT. DO NOT REMOVE. end Initial2Stopped0_transition_code
}

bool SearchBall::Stopped2RotateBody0_transition_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Stopped2RotateBody0_transition_code
	ObjectState *ball =  _BallDetector->getObj();

if ((getStopWatch() > 8000) ||
	((fabs(ball->getAngle())>fromDegrees(60.0)) && (_BallDetector->elapsedTimeSinceLastObs() < ObjectState::LONG_TIME)))
{

	if(ball->getAngle()!=0.0)
		w = (ball->getAngle()/fabs(ball->getAngle()))*0.6;
	else
		w = 0.6;

	return true;
}else
	return false;
//BUILDER COMMENT. DO NOT REMOVE. end Stopped2RotateBody0_transition_code
}

bool SearchBall::RotateBody2Stopped0_transition_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin RotateBody2Stopped0_transition_code
return (getStopWatch() > 12000);
//BUILDER COMMENT. DO NOT REMOVE. end RotateBody2Stopped0_transition_code
}

void
SearchBall::step(void)
{
	switch (state)
	{
	case Initial:

		if (isTime2Run()) {
			startDebugInfo();
			Initial_state_code();

			if (Initial2Stopped0_transition_code()) {
				state = Stopped;
				resetStopWatch();
			}
			endDebugInfo();
		}

		break;
	case RotateBody:
		_BallDetector->step();

		if (isTime2Run()) {
			startDebugInfo();
			RotateBody_state_code();

			if (RotateBody2Stopped0_transition_code()) {
				state = Stopped;
				resetStopWatch();
			}
			endDebugInfo();
		}

		_Body->step();
		_Attention->step();
		break;
	case Stopped:
		_BallDetector->step();

		if (isTime2Run()) {
			startDebugInfo();
			Stopped_state_code();

			if (Stopped2RotateBody0_transition_code()) {
				state = RotateBody;
				resetStopWatch();
			}
			endDebugInfo();
		}

		_Attention->step();
		_Body->step();
		break;
	default:
		cout << "[SearchBall::step()] Invalid state\n";
	}
}

//BUILDER COMMENT. DO NOT REMOVE. auxcode begin
void
SearchBall::init(const string newName, AL::ALPtr<AL::ALBroker> parentBroker)
{
	Component::init(newName, parentBroker);
	setFreqTime(200);

	w = 0.3;
}

void SearchBall::restartSearch()
{
	state = Stopped;
	resetStopWatch();
}
//BUILDER COMMENT. DO NOT REMOVE. auxcode end


