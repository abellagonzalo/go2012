#include "MatchPlayer.h"

//BUILDER COMMENT. DO NOT REMOVE. auxinclude begin
//BUILDER COMMENT. DO NOT REMOVE. auxinclude end

MatchPlayer::MatchPlayer()
{
	_Localization = Localization::getInstance();
	_Body = Body::getInstance();
	_Head = Head::getInstance();
	_GoalDetector = GoalDetector::getInstance();
	_BallDetector = BallDetector::getInstance();
	_WorldState = WorldState::getInstance();
	_Attention = Attention::getInstance();
	_Go2gpos = Go2gpos::getInstance();
	_AuxRole = AuxRole::getInstance();
	_AuxPenalty = AuxPenalty::getInstance();
	state = Initial;
}

MatchPlayer::~MatchPlayer()
{

}

void MatchPlayer::Initial_state_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Initial_state_code
	_WorldState->inMyField();

//BUILDER COMMENT. DO NOT REMOVE. end Initial_state_code
}

void MatchPlayer::Initial_GS_state_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Initial_GS_state_code
//cerr<<"Initial_GS"<<endl;
_Body->stop();
_Head->setPanPos(0.0, 0.3);
_Head->setTiltPos(0.0, 0.3);
_WorldState->inMyField();
//BUILDER COMMENT. DO NOT REMOVE. end Initial_GS_state_code
}

void MatchPlayer::Ready_GS_state_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Ready_GS_state_code
//cerr<<"Ready_GSS"<<endl;

	Vector2<double> goal;

	if(GameController::getInstance()->getPlayerNumber() == 1)
	{
		goal.x = -2900.0;
		goal.y = 0.0;
		//_Go2gpos->setGoalPosAbsAng(-2500.0, 0.0, 0.0);

	}else if(GameController::getInstance()->getPlayerNumber() == 2)
	{
		if(GameController::getInstance()->isMyKickOff())
		{
			goal.x = -1000.0;
			goal.y = 1000.0;
			//_Go2gpos->setGoalPosAbsAng(-1000.0, 1000.0, 0.0);
		}else
		{
			goal.x = -1500.0;
			goal.y = 1000.0;
			//_Go2gpos->setGoalPosAbsAng(-1500.0, 1000.0, 0.0);
		}

	}else if(GameController::getInstance()->getPlayerNumber() == 3)
	{
		if(GameController::getInstance()->isMyKickOff())
		{
			goal.x = -500.0;
			goal.y = 0.0;
			//_Go2gpos->setGoalPosAbsAng(-500.0, 0.0, 0.0);
		}else
		{
			goal.x = -1500.0;
			goal.y = 300.0;
			//_Go2gpos->setGoalPosAbsAng(-1500.0, 300.0, 0.0);
		}

	}else if(GameController::getInstance()->getPlayerNumber()== 4)
	{
		if(GameController::getInstance()->isMyKickOff())
		{
			goal.x = -1000.0;
			goal.y = -1000.0;
			//_Go2gpos->setGoalPosAbsAng(-1000.0, -1000.0, 0.0);
		}else
		{
			goal.x = -1500.0;
			goal.y = -1000.0;
			//_Go2gpos->setGoalPosAbsAng(-1500, -1000.0, 0.0);
		}
	}


Pose2D pos;
pos = _WorldState->getPosition();

static Vector2<double> goalrel = Pose2D::fieldCoord2Relative(pos, goal);
float rho = sqrt(goalrel.x*goalrel.x+goalrel.y*goalrel.y);

if(rho<200.0)
{
	_Go2gpos->setGoalPosAbsAng(goal.x, goal.y, 0.0);
	_Attention->setPercentage("Ball", 0.5);
	_Attention->setPercentage("Goal", 0.5);

}else
{
	float theta = atan2(goalrel.y, goalrel.x);

	_Go2gpos->setGoalPosRelAng(goal.x, goal.y, theta);
	_Attention->setPercentage("Ball", 0.0);
	_Attention->setPercentage("Goal", 1.0);
}

//_WorldState->inMyField();

//BUILDER COMMENT. DO NOT REMOVE. end Ready_GS_state_code
}

void MatchPlayer::Set_GS_state_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Set_GS_state_code
//cerr<<"Set_GS"<<endl;

_Attention->setPercentage("Ball", 0.7);
_Attention->setPercentage("Goal", 0.3);

_Body->stop();
_WorldState->inMyField();
//BUILDER COMMENT. DO NOT REMOVE. end Set_GS_state_code
}

void MatchPlayer::Playing_GS_state_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Playing_GS_state_code
//cerr<<"Playing_GS"<<endl;


//BUILDER COMMENT. DO NOT REMOVE. end Playing_GS_state_code
}

void MatchPlayer::Penalty_GS_state_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Penalty_GS_state_code
//cerr<<"Penalty_GS"<<endl;
//BUILDER COMMENT. DO NOT REMOVE. end Penalty_GS_state_code
}

void MatchPlayer::Penalised_state_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Penalised_state_code
//cerr<<"Penalised"<<endl;
_Body->stop();
_Head->setPanPos(0.0, 0.3);
_Head->setTiltPos(0.0, 0.3);
//BUILDER COMMENT. DO NOT REMOVE. end Penalised_state_code
}

void MatchPlayer::Finished_GS_state_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Finished_GS_state_code
//cerr<<"FINISHED"<<endl;
_Body->stop();
_Head->setPanPos(0.0, 0.3);
_Head->setTiltPos(0.0, 0.3);
//BUILDER COMMENT. DO NOT REMOVE. end Finished_GS_state_code
}

bool MatchPlayer::Initial_GS2Initial0_transition_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Initial_GS2Initial0_transition_code
return (GameController::getInstance()->getRgc()->state != STATE_INITIAL);
//BUILDER COMMENT. DO NOT REMOVE. end Initial_GS2Initial0_transition_code
}

bool MatchPlayer::Initial2Initial_GS0_transition_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Initial2Initial_GS0_transition_code

	if(GameController::getInstance()->getRgc()->state == STATE_INITIAL)
	{
		_Localization->resetToPosition(AbstractLocalization::reset2Initial);
		return true;
	}else
		return false;

//BUILDER COMMENT. DO NOT REMOVE. end Initial2Initial_GS0_transition_code
}

bool MatchPlayer::Ready_GS2Initial0_transition_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Ready_GS2Initial0_transition_code
return (GameController::getInstance()->getRgc()->state != STATE_READY);
//BUILDER COMMENT. DO NOT REMOVE. end Ready_GS2Initial0_transition_code
}

bool MatchPlayer::Set_GS2Initial0_transition_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Set_GS2Initial0_transition_code
return (GameController::getInstance()->getRgc()->state != STATE_SET);
//BUILDER COMMENT. DO NOT REMOVE. end Set_GS2Initial0_transition_code
}

bool MatchPlayer::Playing_GS2Initial0_transition_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Playing_GS2Initial0_transition_code
return (GameController::getInstance()->getRgc()->state != STATE_PLAYING);
//BUILDER COMMENT. DO NOT REMOVE. end Playing_GS2Initial0_transition_code
}

bool MatchPlayer::Penalty_GS2Initial0_transition_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Penalty_GS2Initial0_transition_code
return (GameController::getInstance()->getRgc()->state != STATE_PLAYING);
//BUILDER COMMENT. DO NOT REMOVE. end Penalty_GS2Initial0_transition_code
}

bool MatchPlayer::Initial2Ready_GS0_transition_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Initial2Ready_GS0_transition_code

	if(GameController::getInstance()->getRgc()->state == STATE_READY)
	{
		_Localization->resetToPosition(AbstractLocalization::reset2Ready);
		return true;
	}else
		return false;

//BUILDER COMMENT. DO NOT REMOVE. end Initial2Ready_GS0_transition_code
}

bool MatchPlayer::Initial2Set_GS0_transition_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Initial2Set_GS0_transition_code

	if(GameController::getInstance()->getRgc()->state == STATE_SET)
	{
		_Localization->resetToPosition(AbstractLocalization::reset2Set);
		return true;
	}else
		return false;

//BUILDER COMMENT. DO NOT REMOVE. end Initial2Set_GS0_transition_code
}

bool MatchPlayer::Initial2Playing_GS0_transition_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Initial2Playing_GS0_transition_code

if((GameController::getInstance()->getRgc()->state == STATE_PLAYING) && (GameController::getInstance()->getRgc()->secondaryState !=STATE2_PENALTYSHOOT))
{
	_Localization->resetToPosition(AbstractLocalization::reset2Playing);
	return true;
}else
	return false;

//BUILDER COMMENT. DO NOT REMOVE. end Initial2Playing_GS0_transition_code
}

bool MatchPlayer::Initial2Penalty_GS0_transition_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Initial2Penalty_GS0_transition_code
if ((GameController::getInstance()->getRgc()->state == STATE_PLAYING) && (GameController::getInstance()->getRgc()->secondaryState ==STATE2_PENALTYSHOOT)){

	_Localization->resetToPosition(AbstractLocalization::reset2Penalty);
	_AuxPenalty->restart();
	return true;
}else
	return false;

//BUILDER COMMENT. DO NOT REMOVE. end Initial2Penalty_GS0_transition_code
}

bool MatchPlayer::Initial_GS2Penalised1_transition_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Initial_GS2Penalised1_transition_code
return (GameController::getInstance()->isPenalized());
//BUILDER COMMENT. DO NOT REMOVE. end Initial_GS2Penalised1_transition_code
}

bool MatchPlayer::Penalised2Playing_GS1_transition_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Penalised2Playing_GS1_transition_code
if (!GameController::getInstance()->isPenalized())
{
	_Localization->resetToPosition(AbstractLocalization::resetFromPenalised);
	return true;
}else
{
	return false;
}
//BUILDER COMMENT. DO NOT REMOVE. end Penalised2Playing_GS1_transition_code
}

bool MatchPlayer::Playing_GS2Penalised1_transition_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Playing_GS2Penalised1_transition_code
return (GameController::getInstance()->isPenalized());
//BUILDER COMMENT. DO NOT REMOVE. end Playing_GS2Penalised1_transition_code
}

bool MatchPlayer::Initial2Finished_GS0_transition_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Initial2Finished_GS0_transition_code
return (GameController::getInstance()->getRgc()->state == STATE_FINISHED);
//BUILDER COMMENT. DO NOT REMOVE. end Initial2Finished_GS0_transition_code
}

bool MatchPlayer::Finished_GS2Initial0_transition_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Finished_GS2Initial0_transition_code
return (GameController::getInstance()->getRgc()->state != STATE_FINISHED);
//BUILDER COMMENT. DO NOT REMOVE. end Finished_GS2Initial0_transition_code
}

void
MatchPlayer::step(void)
{
	switch (state)
	{
	case Initial:

		if (isTime2Run()) {
			Initial_state_code();

			if (Initial2Initial_GS0_transition_code()) {
				state = Initial_GS;
			}
			else if (Initial2Ready_GS0_transition_code()) {
				state = Ready_GS;
			}
			else if (Initial2Set_GS0_transition_code()) {
				state = Set_GS;
			}
			else if (Initial2Playing_GS0_transition_code()) {
				state = Playing_GS;
			}
			else if (Initial2Penalty_GS0_transition_code()) {
				state = Penalty_GS;
			}
			else if (Initial2Finished_GS0_transition_code()) {
				state = Finished_GS;
			}
		}

		break;
	case Initial_GS:
		_Localization->step();

		if (isTime2Run()) {
			Initial_GS_state_code();

			if (Initial_GS2Initial0_transition_code()) {
				state = Initial;
			}
			else if (Initial_GS2Penalised1_transition_code()) {
				state = Penalised;
			}
		}

		_Body->step();
		_Head->step();
		break;
	case Ready_GS:
		_GoalDetector->step();
		_BallDetector->step();
		_WorldState->step();
		_Localization->step();

		if (isTime2Run()) {
			Ready_GS_state_code();

			if (Ready_GS2Initial0_transition_code()) {
				state = Initial;
			}
		}

		_Attention->step();
		_Body->step();
		_Go2gpos->step();
		break;
	case Set_GS:
		_GoalDetector->step();
		_BallDetector->step();
		_WorldState->step();
		_Localization->step();

		if (isTime2Run()) {
			Set_GS_state_code();

			if (Set_GS2Initial0_transition_code()) {
				state = Initial;
			}
		}

		_Attention->step();
		_Body->step();
		break;
	case Playing_GS:
		_Localization->step();

		if (isTime2Run()) {
			Playing_GS_state_code();

			if (Playing_GS2Initial0_transition_code()) {
				state = Initial;
			}
			else if (Playing_GS2Penalised1_transition_code()) {
				state = Penalised;
			}
		}

		_AuxRole->step();
		break;
	case Penalty_GS:
		_Localization->step();

		if (isTime2Run()) {
			Penalty_GS_state_code();

			if (Penalty_GS2Initial0_transition_code()) {
				state = Initial;
			}
		}

		_AuxPenalty->step();
		break;
	case Penalised:
		_Localization->step();

		if (isTime2Run()) {
			Penalised_state_code();

			if (Penalised2Playing_GS1_transition_code()) {
				state = Playing_GS;
			}
		}

		_Head->step();
		_Body->step();
		break;
	case Finished_GS:
		_Localization->step();

		if (isTime2Run()) {
			Finished_GS_state_code();

			if (Finished_GS2Initial0_transition_code()) {
				state = Initial;
			}
		}

		_Body->step();
		_Head->step();
		break;
	default:
		cout << "[MatchPlayer::step()] Invalid state\n";
	}
}

//BUILDER COMMENT. DO NOT REMOVE. auxcode begin

void
MatchPlayer::init(const string newName, AL::ALPtr<AL::ALBroker> parentBroker) {
	Component::init(newName, parentBroker);

	setFreqTime(LONG_RATE);

}

//BUILDER COMMENT. DO NOT REMOVE. auxcode end

