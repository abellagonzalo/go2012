#include "GameArbitrator.h"

//BUILDER COMMENT. DO NOT REMOVE. auxinclude begin
//BUILDER COMMENT. DO NOT REMOVE. auxinclude end

GameArbitrator::GameArbitrator()
{
	_Fallen = Fallen::getInstance();
	_Body = Body::getInstance();
	_Head = Head::getInstance();
	_Go2gpos = Go2gpos::getInstance();
	_Attention = Attention::getInstance();
	_Striker = Striker::getInstance();

	state = Initial;
}

GameArbitrator::~GameArbitrator()
{

}

void GameArbitrator::Initial_state_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Initial_state_code
//cerr<<"INITIAL"<<endl;

_Body->stop();
_Head->stop();
//BUILDER COMMENT. DO NOT REMOVE. end Initial_state_code
}

void GameArbitrator::Penalished_state_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Penalished_state_code
//cerr<<"PENALISHED"<<endl;
_Body->stop();
_Head->stop();

//BUILDER COMMENT. DO NOT REMOVE. end Penalished_state_code
}

void GameArbitrator::Set_state_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Set_state_code
//cerr<<"SET"<<endl;
_Attention->setPercentage("Ball", 1.0);
//BUILDER COMMENT. DO NOT REMOVE. end Set_state_code
}

void GameArbitrator::Ready_state_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Ready_state_code

//cerr<<"READY"<<endl;
_Attention->setPercentage("Ball", 1.0);
//BUILDER COMMENT. DO NOT REMOVE. end Ready_state_code
}

void GameArbitrator::Play_state_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Play_state_code

//cerr<<"PLAY"<<endl;
//BUILDER COMMENT. DO NOT REMOVE. end Play_state_code
}

void GameArbitrator::Finish_state_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Finish_state_code

//cerr<<"FINISH"<<endl;
//BUILDER COMMENT. DO NOT REMOVE. end Finish_state_code
}

void GameArbitrator::RecoveringFromFall_state_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin RecoveringFromFall_state_code
//cerr<<"GETTING UP"<<endl;
//BUILDER COMMENT. DO NOT REMOVE. end RecoveringFromFall_state_code
}

bool GameArbitrator::Finish2Initial0_transition_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Finish2Initial0_transition_code
return true;
//BUILDER COMMENT. DO NOT REMOVE. end Finish2Initial0_transition_code
}

bool GameArbitrator::Initial2Play0_transition_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Initial2Play0_transition_code
if(rgc->state == STATE_PLAYING)
{
	return true;
}else
	return false;
//BUILDER COMMENT. DO NOT REMOVE. end Initial2Play0_transition_code
}

bool GameArbitrator::Play2Initial0_transition_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Play2Initial0_transition_code
if(rgc->state != STATE_PLAYING)
{
	return true;
}		
else
	return false;
//BUILDER COMMENT. DO NOT REMOVE. end Play2Initial0_transition_code
}

bool GameArbitrator::Penalished2Initial0_transition_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Penalished2Initial0_transition_code
if(rgc->state != STATE_PLAYING)
	return true;
else
	return false;
//BUILDER COMMENT. DO NOT REMOVE. end Penalished2Initial0_transition_code
}

bool GameArbitrator::Set2RecoveringFromFall0_transition_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Set2RecoveringFromFall0_transition_code
if(_Fallen->getFallen().compare("up") != 0)
{
	if(_Fallen->getFallen().compare("back") == 0)
	{
		_Body->selKick(Body::GBACK);
		_Fallen->setActive(false);
	}
	else
	{
		_Body->selKick(Body::GFRONT);
		_Fallen->setActive(false);
	}
	return true;
}else
	return false;
//BUILDER COMMENT. DO NOT REMOVE. end Set2RecoveringFromFall0_transition_code
}

bool GameArbitrator::RecoveringFromFall2Initial0_transition_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin RecoveringFromFall2Initial0_transition_code

if(_Body->isStopped())
{
	_Fallen->setActive(true);
	return true;
}
//BUILDER COMMENT. DO NOT REMOVE. end RecoveringFromFall2Initial0_transition_code
}

bool GameArbitrator::Set2Initial0_transition_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Set2Initial0_transition_code
if(rgc->state != STATE_SET)
{
	return true;
}else
	return false;
//BUILDER COMMENT. DO NOT REMOVE. end Set2Initial0_transition_code
}

bool GameArbitrator::Initial2Ready0_transition_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Initial2Ready0_transition_code
if(rgc->state == STATE_READY)
{
	return true;
}else
	return false;
//BUILDER COMMENT. DO NOT REMOVE. end Initial2Ready0_transition_code
}

bool GameArbitrator::Ready2Initial0_transition_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Ready2Initial0_transition_code
if(rgc->state != STATE_READY)
{
	return true;
}	
else
	return false;
//BUILDER COMMENT. DO NOT REMOVE. end Ready2Initial0_transition_code
}

bool GameArbitrator::Play2Penalished0_transition_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Play2Penalished0_transition_code
if(gc->isPenalized())
{
	return true;
}else
	return false;
//BUILDER COMMENT. DO NOT REMOVE. end Play2Penalished0_transition_code
}

bool GameArbitrator::Penalished2Play0_transition_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Penalished2Play0_transition_code
if(!gc->isPenalized())
{
	return true;
}else
	return false;

//BUILDER COMMENT. DO NOT REMOVE. end Penalished2Play0_transition_code
}

bool GameArbitrator::Ready2RecoveringFromFall0_transition_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Ready2RecoveringFromFall0_transition_code
if(_Fallen->getFallen().compare("up") != 0)
{
	if(_Fallen->getFallen().compare("back") == 0)
		_Body->doFixMove("getUpBack");
	else
		_Body->doFixMove("getUpFront");
	return true;
}else
	return false;
//BUILDER COMMENT. DO NOT REMOVE. end Ready2RecoveringFromFall0_transition_code
}

bool GameArbitrator::Initial2Set0_transition_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Initial2Set0_transition_code
if(rgc->state == STATE_SET)
{
	if(gc->getPlayerNumber() == 1)
	{
		_Go2gpos->setGoalPos(-2500.0, 0.0);

	}else if(gc->getPlayerNumber() == 2)
	{
		if(gc->isMyKickOff())
		{
			_Go2gpos->setGoalPos(-1500.0, 1500.0);
		}else
			_Go2gpos->setGoalPos(-2000, 1500.0);
	}else if(gc->getPlayerNumber() == 3)
	{
		if(gc->isMyKickOff())
		{
			_Go2gpos->setGoalPos(-1000.0, 0.0);
		}else
			_Go2gpos->setGoalPos(-2000, 250.0);
	}else {
		if(gc->isMyKickOff())
		{
			_Go2gpos->setGoalPos(-1500.0, -1500.0);
		}else
			_Go2gpos->setGoalPos(-2000, -1500.0);
	}

	return true;
}else
	return false;
//BUILDER COMMENT. DO NOT REMOVE. end Initial2Set0_transition_code
}

bool GameArbitrator::Play2RecoveringFromFall0_transition_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Play2RecoveringFromFall0_transition_code
if(_Fallen->getFallen().compare("up") != 0)
{
	if(_Fallen->getFallen().compare("back") == 0)
		_Body->doFixMove("getUpBack");
	else
		_Body->doFixMove("getUpFront");
	return true;
}else
	return false;
//BUILDER COMMENT. DO NOT REMOVE. end Play2RecoveringFromFall0_transition_code
}

bool GameArbitrator::Initial2Finish0_transition_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Initial2Finish0_transition_code
if(rgc->state == STATE_FINISHED)
	return true;
else
	return false;
//BUILDER COMMENT. DO NOT REMOVE. end Initial2Finish0_transition_code
}

bool GameArbitrator::Initial2RecoveringFromFall0_transition_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Initial2RecoveringFromFall0_transition_code
if(_Fallen->getFallen().compare("up") != 0)
{
	if(_Fallen->getFallen().compare("back") == 0)
		_Body->doFixMove("getUpBack");
	else
		_Body->doFixMove("getUpFront");
	return true;
}else
	return false;

//BUILDER COMMENT. DO NOT REMOVE. end Initial2RecoveringFromFall0_transition_code
}

void
GameArbitrator::step(void)
{
	switch (state)
	{
	case Initial:
		_Fallen->step();

		if (isTime2Run()) {
			Initial_state_code();

			if (Initial2Play0_transition_code()) {
				state = Play;
			}
			else if (Initial2Ready0_transition_code()) {
				state = Ready;
			}
			else if (Initial2Set0_transition_code()) {
				state = Set;
			}
			else if (Initial2Finish0_transition_code()) {
				state = Finish;
			}
			else if (Initial2RecoveringFromFall0_transition_code()) {
				state = RecoveringFromFall;
			}
		}

		_Body->step();
		_Head->step();
		break;
	case Penalished:

		if (isTime2Run()) {
			Penalished_state_code();

			if (Penalished2Initial0_transition_code()) {
				state = Initial;
			}
			else if (Penalished2Play0_transition_code()) {
				state = Play;
			}
		}

		_Body->step();
		_Head->step();
		break;
	case Set:
		_Fallen->step();

		if (isTime2Run()) {
			Set_state_code();

			if (Set2RecoveringFromFall0_transition_code()) {
				state = RecoveringFromFall;
			}
			else if (Set2Initial0_transition_code()) {
				state = Initial;
			}
		}

		_Body->step();
		_Go2gpos->step();
		_Attention->step();
		break;
	case Ready:
		_Fallen->step();

		if (isTime2Run()) {
			Ready_state_code();

			if (Ready2Initial0_transition_code()) {
				state = Initial;
			}
			else if (Ready2RecoveringFromFall0_transition_code()) {
				state = RecoveringFromFall;
			}
		}

		_Body->step();
		_Attention->step();
		break;
	case Play:
		_Fallen->step();

		if (isTime2Run()) {
			Play_state_code();

			if (Play2Initial0_transition_code()) {
				state = Initial;
			}
			else if (Play2Penalished0_transition_code()) {
				state = Penalished;
			}
			else if (Play2RecoveringFromFall0_transition_code()) {
				state = RecoveringFromFall;
			}
		}

		_Body->step();
		_Striker->step();
		break;
	case Finish:

		if (isTime2Run()) {
			Finish_state_code();

			if (Finish2Initial0_transition_code()) {
				state = Initial;
			}
		}

		_Body->step();
		_Head->step();
		break;
	case RecoveringFromFall:
		_Body->step();
		_Fallen->step();

		if (isTime2Run()) {
			RecoveringFromFall_state_code();

			if (RecoveringFromFall2Initial0_transition_code()) {
				state = Initial;
			}
		}

		break;
	default:
		cout << "[GameArbitrator::step()] Invalid state\n";
	}
}

//BUILDER COMMENT. DO NOT REMOVE. auxcode begin

void GameArbitrator::init(const string newName, AL::ALPtr<AL::ALBroker> parentBroker)
{
	Component::init(newName, parentBroker);

	setFreqTime(1000);

	gc = new GameController();
	rgc = gc->getRgc();
}

GameController*
GameArbitrator::getRGC()
{
	return gc;
}


//BUILDER COMMENT. DO NOT REMOVE. auxcode end


