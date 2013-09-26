#include "Defender.h"

Defender::Defender()
{
	_FaceBall = FaceBall::getInstance();
	_Body = Body::getInstance();
	_BallDetector = BallDetector::getInstance();
	_SearchBall = SearchBall::getInstance();

	state = Initial;
}

Defender::~Defender()
{

}

void Defender::Initial_state_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Initial_state_code
//BUILDER COMMENT. DO NOT REMOVE. end Initial_state_code
}

void Defender::StayInPos_state_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin StayInPos_state_code
	_Body->stop();
//BUILDER COMMENT. DO NOT REMOVE. end StayInPos_state_code
}

void Defender::SearchingBall_state_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin SearchingBall_state_code
//BUILDER COMMENT. DO NOT REMOVE. end SearchingBall_state_code
}

bool Defender::Initial2SearchingBall0_transition_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Initial2SearchingBall0_transition_code
//BUILDER COMMENT. DO NOT REMOVE. end Initial2SearchingBall0_transition_code
}

bool Defender::SearchingBall2StayInPos0_transition_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin SearchingBall2StayInPos0_transition_code
return _BallDetector->seen();
//BUILDER COMMENT. DO NOT REMOVE. end SearchingBall2StayInPos0_transition_code
}

bool Defender::StayInPos2SearchingBall0_transition_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin StayInPos2SearchingBall0_transition_code
return (!_BallDetector->seen());
//BUILDER COMMENT. DO NOT REMOVE. end StayInPos2SearchingBall0_transition_code
}

void
Defender::step(void)
{
	switch (state)
	{
	case Initial:

		if (isTime2Run()) {
			Initial_state_code();

			if (Initial2SearchingBall0_transition_code()) {
				state = SearchingBall;
			}
		}

		break;
	case StayInPos:
		_FaceBall->step();

		if (isTime2Run()) {
			StayInPos_state_code();

			if (StayInPos2SearchingBall0_transition_code()) {
				state = SearchingBall;
			}
		}

		_Body->step();
		break;
	case SearchingBall:
		_BallDetector->step();
		_SearchBall->step();

		if (isTime2Run()) {
			SearchingBall_state_code();

			if (SearchingBall2StayInPos0_transition_code()) {
				state = StayInPos;
			}
		}

		break;
	default:
		cout << "[Defender::step()] Invalid state\n";
	}
}
//BUILDER COMMENT. DO NOT REMOVE. auxcode begin

void
Defender::init(const string newName, AL::ALPtr<AL::ALBroker> parentBroker) {
	Component::init(newName, parentBroker);

	setFreqTime(MEDIUM_RATE);

}
//BUILDER COMMENT. DO NOT REMOVE. auxcode end


