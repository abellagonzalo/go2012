#include "pruebag2gpos.h"

//BUILDER COMMENT. DO NOT REMOVE. auxinclude begin
//BUILDER COMMENT. DO NOT REMOVE. auxinclude end

pruebag2gpos::pruebag2gpos()
{
	_Localization = Localization::getInstance();
	_GoalDetector = GoalDetector::getInstance();
	_Go2gpos = Go2gpos::getInstance();
	_Body = Body::getInstance();
	_Attention = Attention::getInstance();

	state = Initial;
}

pruebag2gpos::~pruebag2gpos()
{

}

void pruebag2gpos::Initial_state_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Initial_state_code
//BUILDER COMMENT. DO NOT REMOVE. end Initial_state_code
}

void pruebag2gpos::Going_state_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Going_state_code
_Attention->setPercentage("Goal", 1.0);

_Go2gpos->setGoalPosAbsAng(goalposx, goalposy, 0.0);

//cerr<<"Theta Pos: "<<thetapos<<" -> "<<normalize(thetapos/2.5)<<endl;
//BUILDER COMMENT. DO NOT REMOVE. end Going_state_code
}

bool pruebag2gpos::Initial2Going0_transition_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Initial2Going0_transition_code
	return true;
//BUILDER COMMENT. DO NOT REMOVE. end Initial2Going0_transition_code
}

void
pruebag2gpos::step(void)
{
	switch (state)
	{
	case Initial:

		if (isTime2Run()) {
			Initial_state_code();

			if (Initial2Going0_transition_code()) {
				state = Going;
			}
		}

		break;
	case Going:
		_Localization->step();
		_GoalDetector->step();

		if (isTime2Run()) {
			Going_state_code();

		}

		_Go2gpos->step();
		_Body->step();
		_Attention->step();
		break;
	default:
		cout << "[pruebag2gpos::step()] Invalid state\n";
	}
}

//BUILDER COMMENT. DO NOT REMOVE. auxcode begin
void
pruebag2gpos::setGoalPos(double x, double y)
{

	cerr<<"New pos at "<<x<<","<<y<<endl;
	goalposx = x;
	goalposy = y;
}

void
pruebag2gpos::init(const string newName, AL::ALPtr<AL::ALBroker> parentBroker)
{
	Component::init(newName, parentBroker);
	goalposx = goalposy = 0.0;
	setFreqTime(100);
}

void
pruebag2gpos::setGoalPos( double x, double y, const Ice::Current& c)
{
	setGoalPos(x, y);
}
//BUILDER COMMENT. DO NOT REMOVE. auxcode end


