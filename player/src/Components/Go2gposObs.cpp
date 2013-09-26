#include "Go2gposObs.h"

Go2gposObs::Go2gposObs()
{
	_GTLocalization = GTLocalization::getInstance();
	//_Obstacles = Obstacles::getInstance();
	_Body = Body::getInstance();

	state = Initial;
}

Go2gposObs::~Go2gposObs()
{

}

void Go2gposObs::Initial_state_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Initial_state_code
//BUILDER COMMENT. DO NOT REMOVE. end Initial_state_code
}

void Go2gposObs::Localized_state_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Localized_state_code
	/*double vffrho, vfftheta;
	double x,y,t;

	_GTLocalization->getPosition(x,y,t);

	calculateVFFs(x, y, t, vffrho, vfftheta);

	cerr<<"("<<vffrho<<","<<vfftheta<<")"<<endl;
	_Body->setVel(vffrho, -vfftheta);*/

//BUILDER COMMENT. DO NOT REMOVE. end Localized_state_code
}

void Go2gposObs::NotLocalized_state_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin NotLocalized_state_code
//BUILDER COMMENT. DO NOT REMOVE. end NotLocalized_state_code
}

bool Go2gposObs::InitiallyLocalized_transition_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin InitiallyLocalized_transition_code
if(_GTLocalization->getReliability() > 0.8)
	return true;
else
	return false;
//BUILDER COMMENT. DO NOT REMOVE. end InitiallyLocalized_transition_code
}

bool Go2gposObs::InitialllyNotLocalized_transition_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin InitialllyNotLocalized_transition_code
if(_GTLocalization->getReliability() <= 0.8)
	return true;
else
	return false;
//BUILDER COMMENT. DO NOT REMOVE. end InitialllyNotLocalized_transition_code
}

bool Go2gposObs::Lost_transition_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Lost_transition_code
if(_GTLocalization->getReliability() <= 0.8)
	return true;
else
	return false;
//BUILDER COMMENT. DO NOT REMOVE. end Lost_transition_code
}

bool Go2gposObs::Found_transition_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Found_transition_code
if(_GTLocalization->getReliability() > 0.8)
	return true;
else
	return false;
//BUILDER COMMENT. DO NOT REMOVE. end Found_transition_code
}

void
Go2gposObs::step(void)
{
	switch (state)
	{
	case Initial:
		_GTLocalization->step();

		if (isTime2Run()) {
			Initial_state_code();

			if (InitiallyLocalized_transition_code())
				state = Localized;
			else if (InitialllyNotLocalized_transition_code())
				state = NotLocalized;
		}

		break;
	case Localized:
		_GTLocalization->step();
		//_Obstacles->step();

		if (isTime2Run()) {
			Localized_state_code();

			if (Lost_transition_code())
				state = NotLocalized;
		}

		_Body->step();
		break;
	case NotLocalized:
		_GTLocalization->step();

		if (isTime2Run()) {
			NotLocalized_state_code();

			if (Found_transition_code())
				state = Localized;
		}

		break;
	default:
		cout << "[Go2gposObs::step()] Invalid state\n";
	}
}
//BUILDER COMMENT. DO NOT REMOVE. auxcode begin


void
Go2gposObs::init(const string newName, AL::ALPtr<AL::ALBroker> parentBroker) {
	Component::init(newName, parentBroker);

	goalposx = goalposy = robotposx = robotposy = goalvx = goalvy = 0.0;

	setFreqTime(500);

}

void
Go2gposObs::setGoalPos(double x, double y)
{
	goalposx = x;
	goalposy = y;
}

void
Go2gposObs::setGoalPos( double x, double y, const Ice::Current& c)
{
	setGoalPos(x, y);
}


//BUILDER COMMENT. DO NOT REMOVE. auxcode end


