#include "Go2gpos.h"

//BUILDER COMMENT. DO NOT REMOVE. auxinclude begin
//BUILDER COMMENT. DO NOT REMOVE. auxinclude end

Go2gpos::Go2gpos()
{
	_WorldState = WorldState::getInstance();
	_Go2pos = Go2pos::getInstance();
	_Body = Body::getInstance();
	state = Initial;
}

Go2gpos::~Go2gpos()
{

}

void Go2gpos::Initial_state_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Initial_state_code
//BUILDER COMMENT. DO NOT REMOVE. end Initial_state_code
}

void Go2gpos::Go2Pos_state_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Go2Pos_state_code
float t;

Pose2D pos = _WorldState->getPosition();
Vector2<double> fieldCoord;

fieldCoord.x = goalposx;
fieldCoord.y = goalposy;

Vector2<double> newpos = Pose2D::fieldCoord2Relative(pos, fieldCoord);


if(absolute)
{
	t = normalizePi(goalpost - pos.rotation);
}else
{
	t = goalpost;
}

	_Go2pos->setGoalPosXY(newpos.x, newpos.y, t);

//BUILDER COMMENT. DO NOT REMOVE. end Go2Pos_state_code
}

bool Go2gpos::InitiallyLocalized_transition_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin InitiallyLocalized_transition_code
return true;
//BUILDER COMMENT. DO NOT REMOVE. end InitiallyLocalized_transition_code
}

void
Go2gpos::step(void)
{
	switch (state)
	{
	case Initial:

		if (isTime2Run()) {
			startDebugInfo();
			Initial_state_code();

			if (InitiallyLocalized_transition_code()) {
				state = Go2Pos;
			}
			endDebugInfo();
		}

		break;
	case Go2Pos:
		_WorldState->step();

		if (isTime2Run()) {
			startDebugInfo();
			Go2Pos_state_code();
			endDebugInfo();
		}

		_Go2pos->step();
		_Body->step();
		break;
	default:
		cout << "[Go2gpos::step()] Invalid state\n";
	}
}

//BUILDER COMMENT. DO NOT REMOVE. auxcode begin
void
Go2gpos::setGoalPosRelAng(double x, double y, double t)
{
	goalposx = x;
	goalposy = y;
	goalpost = t;

	absolute = false;

}
void
Go2gpos::setGoalPosAbsAng(double x, double y, double t)
{
	goalposx = x;
	goalposy = y;
	goalpost = t;

	absolute = true;

}


void
Go2gpos::init(const string newName, AL::ALPtr<AL::ALBroker> parentBroker)
{
	Component::init(newName, parentBroker);
	goalposx = goalposy = goalpost = 0.0;
	absolute = true;
	setFreqTime(MEDIUM_RATE);
}

void
Go2gpos::setGoalPos( double x, double y, const Ice::Current& c)
{
	setGoalPosRelAng(x, y, 0.0);
}

//BUILDER COMMENT. DO NOT REMOVE. auxcode end

