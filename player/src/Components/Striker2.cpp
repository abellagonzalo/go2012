#include "Striker2.h"

//BUILDER COMMENT. DO NOT REMOVE. auxinclude begin
//BUILDER COMMENT. DO NOT REMOVE. auxinclude end

Striker2::Striker2()
{
	_BallDetector = BallDetector::getInstance();
	_WorldState = WorldState::getInstance();
	_SearchBall = SearchBall::getInstance();
	_Attention = Attention::getInstance();
	_GoalDetector = GoalDetector::getInstance();
	_ApproachBall = ApproachBall::getInstance();
	_Go2pos = Go2pos::getInstance();
	_Body = Body::getInstance();
	state = Initial;
}

Striker2::~Striker2()
{

}

void Striker2::Initial_state_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Initial_state_code
//BUILDER COMMENT. DO NOT REMOVE. end Initial_state_code
}

void Striker2::SearchingBall_state_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin SearchingBall_state_code

//cerr<<"Striker::SEARCHING"<<endl;
_Attention->setPercentage("Goal", 0.0);

//BUILDER COMMENT. DO NOT REMOVE. end SearchingBall_state_code
}

void Striker2::Going2Ball_state_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Going2Ball_state_code
//cerr<<"Striker::GOING2BALL"<<endl;

ObjectState* balls = _BallDetector->getObj();
float rho = balls->getDistance();

if(rho < 800)
{
	_Attention->setPercentage("Ball", 1.0);
	_Attention->setPercentage("Goal", 0.0);
}else
{
	_Attention->setPercentage("Ball", 0.5);
	_Attention->setPercentage("Goal", 0.5);
}


//BUILDER COMMENT. DO NOT REMOVE. end Going2Ball_state_code
}

void Striker2::BallNear_state_code(void)
{
	ObjectState* balls = _BallDetector->getObj();
	Vector2<double> pos = balls->getPositionInRelativeCoordinates();
	Vector2<double> vleft = _WorldState->getGoalLVector();
	Vector2<double> vright = _WorldState->getGoalRVector();

	//cerr<<"====================================================="<<endl;
	//cerr<<"ball\t["<<pos.x<<", "<<pos.y<<"]"<<endl;
	updateGoalAngle();
	updateKicks();

float rho = balls->getDistance();

	_Attention->setPercentage("Ball", 1.0);
	_Attention->setPercentage("Goal", 0.0);

	//_Attention->setPercentage("Ball", 0.5);
	//_Attention->setPercentage("Goal", 0.5);


float angle2Goal, angle2Shoot;
float dx, dy;

angle2Goal = atan2((vleft.y+vright.y)/2.0, (vleft.x+vright.x)/2.0);

if(rho>300)
{
	angle2Shoot = 0.0;
}else
{
	angle2Shoot = goalAngle;

	if(angle2Shoot > 0.4) angle2Shoot = 0.4;
	if(angle2Shoot< -0.4) angle2Shoot = -0.4;
}

if(angle2Goal>0.0)
{
	dx = (maxx + minx)/2.0;
	dy = (rmaxy + rminy)/2.0;

	if((fabs(angle2Shoot)>0.3) || (( rho<300) && (pos.x<90)&&(pos.y<-130.0)))
	{
		dx = dx + 100.0;
		dy = dy + 50.0;
	}else
	if((fabs(angle2Shoot)>0.2)|| (( rho<300) && (pos.x<90)&&(pos.y<-130.0)))
	{
		dx = dx + 100.0;
		dy = dy + 50.0;
	}

}else
{
	dx = (maxx + minx)/2.0;
	dy = (lmaxy + lminy)/2.0;

	if((fabs(angle2Shoot)>0.3)|| (( rho<300) && (pos.x<90)&&(pos.y>130.0)))
	{
		dx = dx + 100.0;
		dy = dy - 50.0;
	}else
	if((fabs(angle2Shoot)>0.2)|| (( rho<300) && (pos.x<90)&&(pos.y>130.0)))
	{
		dx = dx + 100.0;
		dy = dy - 50.0;
	}

}

//cerr<<"Mov ["<<pos.x-dx<<", "<<pos.y-dy<<", "<<angle2Shoot<<"]"<<endl;

	if((angle2Shoot>=0.0) && (angle2Shoot<0.15)) angle2Shoot = 0.15;
	if((angle2Shoot<=0.0) && (angle2Shoot>-0.15)) angle2Shoot = -0.15;

	_Go2pos->setGoalPosXY(pos.x-dx, pos.y-dy, angle2Shoot);

//BUILDER COMMENT. DO NOT REMOVE. end BallNear_state_code
}

void Striker2::Shoot_state_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Shoot_state_code
	_Attention->setPercentage("Ball", 1.0);
	_Attention->setPercentage("Goal", 0.0);

//BUILDER COMMENT. DO NOT REMOVE. end Shoot_state_code
}

bool Striker2::Initial2SearchingBall0_transition_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Initial2SearchingBall0_transition_code

return true;
//BUILDER COMMENT. DO NOT REMOVE. end Initial2SearchingBall0_transition_code
}

bool Striker2::SearchingBall2Going2Ball0_transition_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin SearchingBall2Going2Ball0_transition_code
return _BallDetector->ballmodel->elapsedTimeSinceLastObs < ObjectState::RECENTLY;
//BUILDER COMMENT. DO NOT REMOVE. end SearchingBall2Going2Ball0_transition_code
}

bool Striker2::Going2Ball2SearchingBall0_transition_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Going2Ball2SearchingBall0_transition_code

if(_BallDetector->ballmodel->elapsedTimeSinceLastObs > ObjectState::LONG_TIME)
{
	_SearchBall->restartSearch();
	return true;
}else
	return false;

//BUILDER COMMENT. DO NOT REMOVE. end Going2Ball2SearchingBall0_transition_code
}

bool Striker2::Going2Ball2BallNear0_transition_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Going2Ball2BallNear0_transition_code



ObjectState* balls = _BallDetector->getObj();
float rho = balls->getDistance();

if(rho < 400)
{
	return true;
}else
{
	return false;
}

//BUILDER COMMENT. DO NOT REMOVE. end Going2Ball2BallNear0_transition_code
}

bool Striker2::BallNear2Going2Ball0_transition_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin BallNear2Going2Ball0_transition_code
ObjectState* balls = _BallDetector->getObj();
float rho = balls->getDistance();

if(rho > 600)
{
	return true;
}else
{
	return false;
}

//BUILDER COMMENT. DO NOT REMOVE. end BallNear2Going2Ball0_transition_code
}


bool Striker2::BallNear2Shoot0_transition_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin BallNear2Shoot0_transition_code


	if(_BallDetector->ballmodel->elapsedTimeSinceLastObs > ObjectState::RECENTLY)
		return false;
	//cerr<<"Ball time is "<<_BallDetector->ballmodel->elapsedTimeSinceLastObs<<endl;
	if(tr)
	{
		_Body->selKick("TXTSIDE2RSI");
		return true;
	}
	if(tl)
	{
		_Body->selKick("TXTSIDE2LSI");
		return true;
	}
	if(fr)
	{
		//cerr<<"++++++++++++++++\nShoot:Front Right "<<endl;

		_Body->selKick("TXTFWSTR");
		return true;
	}
	if(fl)
	{
		//cerr<<"++++++++++++++++\nShoot:Front Left "<<endl;
		_Body->selKick("TXTFWSTL");
		return true;
	}
	if(br)
	{
		//cerr<<"++++++++++++++++\nShoot:Back Right "<<endl;
		_Body->selKick("TXTBWR");
		return true;
	}
	if(bl)
	{
		//cerr<<"++++++++++++++++\nShoot:Back Left "<<endl;
		_Body->selKick("TXTBWL");
		return true;
	}

	return false;

//BUILDER COMMENT. DO NOT REMOVE. end BallNear2Shoot0_transition_code
}

bool Striker2::Shoot2BallNear0_transition_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Shoot2BallNear0_transition_code
return (getStopWatch()>2000);
//BUILDER COMMENT. DO NOT REMOVE. end Shoot2BallNear0_transition_code
}

bool Striker2::BallNear2SearchingBall0_transition_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin BallNear2SearchingBall0_transition_code
	if(_BallDetector->ballmodel->elapsedTimeSinceLastObs > ObjectState::LONG_TIME)
	{
		_SearchBall->restartSearch();
		return true;
	}else
		return false;
//BUILDER COMMENT. DO NOT REMOVE. end BallNear2SearchingBall0_transition_code
}

void
Striker2::step(void)
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
	case SearchingBall:
		_BallDetector->step();
		_WorldState->step();

		if (isTime2Run()) {

			startDebugInfo();

			SearchingBall_state_code();

			if (SearchingBall2Going2Ball0_transition_code()) {
				state = Going2Ball;
			}
			endDebugInfo();
		}

		_SearchBall->step();
		_Attention->step();
		break;
	case Going2Ball:
		_BallDetector->step();
		_WorldState->step();
		_GoalDetector->step();

		if (isTime2Run()) {
			startDebugInfo();

			Going2Ball_state_code();

			if (Going2Ball2SearchingBall0_transition_code()) {
				state = SearchingBall;
			}
			else if (Going2Ball2BallNear0_transition_code()) {
				state = BallNear;
			}
			endDebugInfo();
		}

		_Attention->step();
		_ApproachBall->step();
		break;
	case BallNear:
		_BallDetector->step();
		_WorldState->step();

		if (isTime2Run()) {
			startDebugInfo();

			BallNear_state_code();

			if (BallNear2Going2Ball0_transition_code()) {
				state = Going2Ball;
			}
			else if (BallNear2Shoot0_transition_code()) {
				state = Shoot;
				resetStopWatch();
			}
			else if (BallNear2SearchingBall0_transition_code()) {
				state = SearchingBall;
			}
			endDebugInfo();
		}

		_Attention->step();
		_Go2pos->step();
		break;
	case Shoot:
		_BallDetector->step();

		if (isTime2Run()) {
			startDebugInfo();

			Shoot_state_code();

			if (Shoot2BallNear0_transition_code()) {
				state = BallNear;
			}
			endDebugInfo();
		}

		_Body->step();
		_Attention->step();
		break;
	default:
		cout << "[Striker2::step()] Invalid state\n";
	}
}

//BUILDER COMMENT. DO NOT REMOVE. auxcode begin

void
Striker2::init(const string newName, AL::ALPtr<AL::ALBroker> parentBroker) {
	Component::init(newName, parentBroker);

	setFreqTime(MEDIUM_RATE);


	maxx = 220.0;
	minx = 135.0;

	lminy = 12.0;
	lmaxy = 74.0;
	rmaxy = 2.5;
	rminy = -60.0;

}



void
Striker2::updateKicks()
{

	bool safeKick = false;

	ObjectState* balls = _BallDetector->getObj();
	Vector2<double> pos = balls->getPositionInRelativeCoordinates();

	Vector2<double> vleft = _WorldState->getGoalLVector();
	Vector2<double> vright = _WorldState->getGoalRVector();

	float angR, angL;

	angR = atan2(vright.y, vright.x);
	angL = atan2(vleft.y, vleft.x);

	float amp = fabs(normalizePi(angR - angL));
	float margin = amp/10.0;

	angR = normalizePi(angR+margin);
	angL = normalizePi(angL-margin);

	float auxR, auxL;

	auxR = normalizePi(angR + pi_2);
	auxL = normalizePi(angL + pi_2);
	tr = ((auxR*auxL)<0.0) && (auxL>auxR) && (pos.x>minx) && (pos.x<maxx) && (pos.y>lminy) && (pos.y<lmaxy);

	//cerr<<"\tTo right\t["<<minx<<", "<<maxx<<"]t\t["<<lminy<<", "<<lmaxy<<"]"<<"\t("<<auxR<<", "<<auxL<<")";
	//if(tr) cerr<<"\tOK !!!!!"<<endl;
	//else cerr<<endl;

	auxR = normalizePi(angR - pi_2);
	auxL = normalizePi(angL - pi_2);
	tl = ((auxR*auxL)<0.0) && (auxL>auxR) && (pos.x>minx) && (pos.x<maxx) && (pos.y>rminy) && (pos.y<rmaxy);

	//cerr<<"\tTo left\t["<<minx<<", "<<maxx<<"]t\t["<<rminy<<", "<<rmaxy<<"]"<<"\t("<<auxR<<", "<<auxL<<")";
	//if(tl) cerr<<"\tOK !!!!!"<<endl;
	//else cerr<<endl;

	fr = ((angR*angL)<0.0) && (angL>angR) && (pos.x>minx) && (pos.x<maxx) &&(pos.y>rminy) && (pos.y<rmaxy);

	//cerr<<"\tFront right\t["<<minx<<", "<<maxx<<"]t\t["<rlminy<<", "<<rmaxy<<"]"<<"\t("<<angR<<", "<<angL<<")";
	//if(fr) cerr<<"\tOK !!!!!"<<endl;
	//else cerr<<endl;

	fl = ((angR*angL)<0.0) && (angL>angR) && (pos.x>minx) && (pos.x<maxx) &&(pos.y>lminy) && (pos.y<lmaxy);

	//cerr<<"\tFront left\t["<<minx<<", "<<maxx<<"]t\t["<<lminy<<", "<<lmaxy<<"]"<<"\t("<<angR<<", "<<angL<<")";
	//if(fl) cerr<<"\tOK !!!!!"<<endl;
	//else cerr<<endl;

	br = ((angR*angL)<0.0) && (angL<angR) && (pos.x>minx) && (pos.x<maxx) && (pos.y>rminy) && (pos.y<rmaxy);
	//cerr<<"\tBack right\t["<<minx<<", "<<maxx<<"]t\t["<<rminy<<", "<<rmaxy<<"]"<<"\t("<<angR<<", "<<angL<<")";
	//if(br) cerr<<"\tOK !!!!!"<<endl;
	//else cerr<<endl;

	bl = ((angR*angL)<0.0) && (angL<angR)&& (pos.x>minx) && (pos.x<maxx) && (pos.y>lminy) && (pos.y<lmaxy);

	//cerr<<"\tBack left\t["<<minx<<", "<<maxx<<"]t\t["<<lminy<<", "<<lmaxy<<"]"<<"\t("<<angR<<", "<<angL<<")";
	//if(bl) cerr<<"\tOK !!!!!"<<endl;
	//else cerr<<endl;

	//tr = tl = fr = fl = bl = br = false;

}

float
Striker2::goalMidAngle()
{
	float ret;
	float ang2G[4];
	float Mang2G[4];
	float auxR, auxL;
	Vector2<double> vleft = _WorldState->getGoalLVector();
	Vector2<double> vright = _WorldState->getGoalRVector();

	float angR, angL;
	angR = atan2(vright.y, vright.x);
	angL = atan2(vleft.y, vleft.x);


	//Angle to Front
	if(fabs(angR)>fabs(angL))
	{
		ang2G[0] = angL;
		Mang2G[0] = normalizePi((angL + angR)/2.0);

	}
	else
	{
		ang2G[0] = angR;
		Mang2G[0] = normalizePi((angL + angR)/2.0);
	}

	//angle to back
	auxL = normalizePi(angL+M_PI);
	auxR = normalizePi(angR+M_PI);
	if(fabs(auxR)>fabs(auxL))
	{
		ang2G[1] = auxR;
		Mang2G[1] = normalizePi((angL + angR)/2.0);

	}
	else
	{
		ang2G[1] = auxL;
		Mang2G[1] = normalizePi((angL + angR)/2.0);

	}

	//angle to Right
	auxL = normalizePi(angL+pi_2);
	auxR = normalizePi(angR+pi_2);
	if(fabs(auxR)>fabs(auxL))
	{
		ang2G[2] = auxR;
		Mang2G[2] = normalizePi((auxL + auxR)/2.0);
	}
	else
	{
		ang2G[2] = auxL;
		Mang2G[2] = normalizePi((auxL + auxR)/2.0);

	}

	//angle to Left
	auxL = normalizePi(angL-pi_2);
	auxR = normalizePi(angR-pi_2);
	if(fabs(auxR)>fabs(auxL))
	{
		ang2G[3] = auxR;
		Mang2G[3] = normalizePi((auxL + auxR)/2.0);
	}
	else
	{
		ang2G[3] = auxL;
		Mang2G[3] = normalizePi((auxL + auxR)/2.0);
	}


	////cerr<<"Mid Angles:"<<endl;
	float menor = M_PI;
	for(int i=0; i<4; i++)
	{
		//if(i==0) cerr<<"Front: "<<ang2G[i]<<endl;
		//if(i==1) cerr<<"Back: "<<ang2G[i]<<endl;
		//if(i==2) cerr<<"Right: "<<ang2G[i]<<endl;
		//if(i==3) cerr<<"Left: "<<ang2G[i]<<endl;
		if(fabs(ang2G[i])<menor)
		{
			ret = Mang2G[i];
			menor = ang2G[i];
		}
	}
	return ret;
}

void Striker2::updateGoalAngle()
{
	float ang2G[4];
	float auxR, auxL;
	Vector2<double> vleft = _WorldState->getGoalLVector();
	Vector2<double> vright = _WorldState->getGoalRVector();

	float angR, angL;
	angR = atan2(vright.y, vright.x);
	angL = atan2(vleft.y, vleft.x);

	//Angle to Front
	if(fabs(angR)>fabs(angL))
		ang2G[0] = angL;
	else
		ang2G[0] = angR;

	//angle to back
	auxL = normalizePi(angL+M_PI);
	auxR = normalizePi(angR+M_PI);
	if(fabs(auxR)>fabs(auxL))
		ang2G[1] = auxL;
	else
		ang2G[1] = auxR;

	//angle to Right
	auxL = normalizePi(angL+pi_2);
	auxR = normalizePi(angR+pi_2);
	if(fabs(auxR)>fabs(auxL))
		ang2G[2] = auxL;
	else
		ang2G[2] = auxR;

	//angle to Left
	auxL = normalizePi(angL-pi_2);
	auxR = normalizePi(angR-pi_2);
	if(fabs(auxR)>fabs(auxL))
		ang2G[3] = auxL;
	else
		ang2G[3] = auxR;


	//cerr<<"Angles:"<<endl;
	float menor = M_PI;
	for(int i=0; i<4; i++)
	{
		if(fabs(ang2G[i])<menor)
		{
			goalAngle = ang2G[i];
			menor = fabs(goalAngle);
		}
	}

}


//BUILDER COMMENT. DO NOT REMOVE. auxcode end

