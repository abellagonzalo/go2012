#include "WorldState.h"
#include "WorldModel.h"

WorldState::WorldState()
{
	_Localization = Localization::getInstance();
	_BallDetector = BallDetector::getInstance();
	_GoalDetector = GoalDetector::getInstance();
	_RobotDetector = RobotDetector::getInstance();
	_SharedTeamInfo = SharedTeamInfo::getInstance();
	_GTLocalization = GTLocalization::getInstance();


	hyps.clear();

	myPos.translation.x = 0.0;
	myPos.translation.y = 0.0;
	myPos.rotation = 0.0;
	myReliability = 0.0;
}

WorldState::~WorldState()
{

}

void
WorldState::updateLocalHipotheses()
{
	list<int> recvHips;
	recvHips.clear();

	vector<tRobotHipothesis> hipotheses;
	_Localization->getPosition(hipotheses);

	//Add and update
	vector<tRobotHipothesis>::iterator it;
	for(it = hipotheses.begin(); it!=hipotheses.end(); ++it)
	{
		recvHips.push_back((*it).id);

		if(hyps.count((*it).id) > 0)
		{
			tState aux = hyps[(*it).id];
			aux.pos.rotation =(*it).t;
			aux.pos.translation.x =(*it).x;
			aux.pos.translation.y =(*it).y;

			aux.prob = ((*it).prob * aux.prob) / (1.0/hipotheses.size());

			hyps[(*it).id] = aux;
		}
		else
		{
			tState aux;
			aux.id = (*it).id;
			aux.pos.rotation =(*it).t;
			aux.pos.translation.x =(*it).x;
			aux.pos.translation.y =(*it).y;
			aux.prob =(*it).prob;

			hyps[(*it).id] = aux;
		}
	}

	//Clean removed
	map<int, tState>::iterator itm, itaux;
	list<int>::iterator iti;
	for(itm = hyps.begin(); itm!=hyps.end(); ++itm)
	{
		bool esta = false;
		for(iti = recvHips.begin(); iti!=recvHips.end(); ++iti)
		{
			if((*iti) == (*itm).second.id)
			{
				esta = true;
				break;
			}
		}

		if(!esta)
		{
			itaux = itm; itaux++;
			hyps.erase(itm);
			itm = itaux;
		}
	}

	normalize();
}

void
WorldState::normalize()
{
	float sum = 0.0;
	map<int, tState>::iterator itm;

	for(itm = hyps.begin(); itm!=hyps.end(); ++itm)
	{
		if((*itm).second.prob<0.0001) (*itm).second.prob = 0.0001;
		if((*itm).second.prob>1.0) (*itm).second.prob = 1.0;

		sum = sum + (*itm).second.prob;
	}

	for(itm = hyps.begin(); itm!=hyps.end(); ++itm)
		(*itm).second.prob = (*itm).second.prob/sum;
}

void
WorldState::updateBall()
{

	if (GameController::getInstance()->getRgc()->state != STATE_PLAYING)
		return;
	//cerr<<"==================================================="<<endl;
	//cerr<<"Soy robot "<<_SharedTeamInfo->getMyId()<<endl;
	//for (int i = 0; i < SharedTeamInfo::NUM_ROBOTS; i++)

	//if(i==0) //Si es el portero/*true ||*/
	//(( _SharedTeamInfo->getMyId() == 1) && (i !=( _SharedTeamInfo->getMyId()-1))) ||
	//(( _SharedTeamInfo->getMyId() != 1) && (i == 0)))
	//{
	/*	float pr, br;
			if(_SharedTeamInfo->teamData[i].getPosReliability() == "Low")
				pr = 0.3;
			else if(_SharedTeamInfo->teamData[i].getPosReliability() == "Medium")
				pr = 0.8;
			else
				pr = 1.0;

			if(_SharedTeamInfo->teamData[i].getBallReliability() == "Unreliable")
				br = 0.001;
			else if(_SharedTeamInfo->teamData[i].getBallReliability() == "Low")
				br = 0.3;
			else if(_SharedTeamInfo->teamData[i].getBallReliability() == "Medium")
				br = 0.5;
			else if(_SharedTeamInfo->teamData[i].getBallReliability() == "High")
				br = 0.8;
			else if(_SharedTeamInfo->teamData[i].getBallReliability() == "Most")
				br = 1.0;
	 */

	ObjectState* balls = _BallDetector->getObj();
	Vector2<double> posrel = balls->getPositionInRelativeCoordinates();

	float rho = sqrt(posrel.x*posrel.x+posrel.y+posrel.y);

	//cerr<<"WorldState::updateBall\tGoalKeeper ball = "<<_SharedTeamInfo->teamData[0].getBallReliability()<<endl;
	if((_SharedTeamInfo->teamData[0].getBallReliability() == "Most") ||
			(_SharedTeamInfo->teamData[0].getBallReliability() == "High"))
	{
		Pose2D rpos;

		rpos.translation.x = -3000.0;
		rpos.translation.y = 0.0;
		rpos.rotation = 0.0;

		Vector2<double> ballf = Pose2D::relative2FieldCoord(rpos,
				_SharedTeamInfo->teamData[0].getBallX(),
				_SharedTeamInfo->teamData[0].getBallY());

		float pf = 1.0;


		map<int, tState>::iterator it;
		for(it = hyps.begin(); it!=hyps.end(); ++it)
		{
			Vector2<double> posabs = Pose2D::relative2FieldCoord((*it).second.pos, posrel.x, posrel.y);

			float distance = sqrt((posabs.x - ballf.x) * (posabs.x - ballf.x) + (posabs.y - ballf.y) *(posabs.y - ballf.y));
			float po = normalDist(distance/2000.0, 0.0, 0.6);

			(*it).second.prob = ((po * (*it).second.prob) * pf);

		}

	}else if((_SharedTeamInfo->teamData[0].getBallReliability() == "Unreliable") ||
			(rho > 3500))
	{
		map<int, tState>::iterator it;
		for(it = hyps.begin(); it!=hyps.end(); ++it)
		{
			Vector2<double> posabs = Pose2D::relative2FieldCoord((*it).second.pos, posrel.x, posrel.y);

			if(posabs.x < -500.0)
				(*it).second.prob = (0.1 * (*it).second.prob);
			else
				(*it).second.prob = (0.8 * (*it).second.prob);
		}

	}

	normalize();
}

void
WorldState::UpdateOwnGoal(ObjectState *left, ObjectState *right)
{

	//cerr<<"Mi portería!!!"<<endl;
	ObjectState *p0l = &(_GoalDetector->goals->p0LeftEstimate);
	ObjectState *p0r = &(_GoalDetector->goals->p0RightEstimate);
	ObjectState *p1l = &(_GoalDetector->goals->p1LeftEstimate);
	ObjectState *p1r = &(_GoalDetector->goals->p1RightEstimate);


	if(p1l == left)
	{
		ownLeft = p1l;
		ownRight = p1r;
		opponentLeft = p0l;
		opponentRight = p0r;
	}else
	{
		ownLeft = p0l;
		ownRight = p0r;
		opponentLeft = p1l;
		opponentRight = p1r;
	}

	//cerr<<"Own at ["<<ownLeft->getPositionInRelativeCoordinates().x<<", "<<ownLeft->getPositionInRelativeCoordinates().y<<"] ["<<
	//		ownRight->getPositionInRelativeCoordinates().x<<", "<<ownRight->getPositionInRelativeCoordinates().y<<"]"<<endl;

	float distancezl = sqrt(ownLeft->getPositionInRelativeCoordinates().x*ownLeft->getPositionInRelativeCoordinates().x+ownLeft->getPositionInRelativeCoordinates().y*ownLeft->getPositionInRelativeCoordinates().y);
	float anglezl = atan2(ownLeft->getPositionInRelativeCoordinates().y, ownLeft->getPositionInRelativeCoordinates().x);
	float distancezr = sqrt(ownRight->getPositionInRelativeCoordinates().x*ownRight->getPositionInRelativeCoordinates().x+ownRight->getPositionInRelativeCoordinates().y*ownRight->getPositionInRelativeCoordinates().y);
	float anglezr = atan2(ownRight->getPositionInRelativeCoordinates().y, ownRight->getPositionInRelativeCoordinates().x);

	float goalx, goaly;
	float goallx, goally, goalrx, goalry ;

	goallx = -3000.0;
	goally = -700.0;
	goalrx = -3000.0;
	goalry = 700.0;

	map<int, tState>::iterator it;
	for(it = hyps.begin(); it!=hyps.end(); ++it)
	{
		Pose2D aux = (*it).second.pos;

		Vector2<double> lefta = Pose2D::relative2FieldCoord(aux, left->getPositionInRelativeCoordinates().x, left->getPositionInRelativeCoordinates().y);
		Vector2<double> righta = Pose2D::relative2FieldCoord(aux, right->getPositionInRelativeCoordinates().x, right->getPositionInRelativeCoordinates().y);


		float dl1 = sqrt((goallx - lefta.x) * (goallx - lefta.x) + (goally - lefta.y) *(goally - lefta.y));

		float difdist1 = fabs(distancezl-dl1);
		float pod1 = normalDist(difdist1/2000.0, 0.0, 0.6);

		float dl2 = sqrt((goalrx - righta.x) * (goalrx - righta.x) + (goalry - righta.y) *(goalry - righta.y));

		float difdist2 = fabs(distancezr-dl2);

		float pod2 = normalDist(difdist2/2000.0, 0.0, 0.6);

		(*it).second.prob = pod1 * pod2;// * (*it).second.prob) / 1.0);

	}


	normalize();

}

void
WorldState::UpdateOpponentGoal(ObjectState *left, ObjectState *right)
{


	ObjectState *p0l = &(_GoalDetector->goals->p0LeftEstimate);
	ObjectState *p0r = &(_GoalDetector->goals->p0RightEstimate);
	ObjectState *p1l = &(_GoalDetector->goals->p1LeftEstimate);
	ObjectState *p1r = &(_GoalDetector->goals->p1RightEstimate);


	if(p1l == left)
	{
		ownLeft = p0l;
		ownRight = p0r;
		opponentLeft = p1l;
		opponentRight = p1r;
	}else
	{
		ownLeft = p1l;
		ownRight = p1r;
		opponentLeft = p0l;
		opponentRight = p0r;
	}

	//cerr<<"Opponnet at ["<<opponentLeft->getPositionInRelativeCoordinates().x<<", "<<opponentLeft->getPositionInRelativeCoordinates().y<<"] ["<<
	//		opponentRight->getPositionInRelativeCoordinates().x<<", "<<opponentRight->getPositionInRelativeCoordinates().y<<"]"<<endl;

	float distancezl = sqrt(opponentLeft->getPositionInRelativeCoordinates().x*opponentLeft->getPositionInRelativeCoordinates().x+opponentLeft->getPositionInRelativeCoordinates().y*opponentLeft->getPositionInRelativeCoordinates().y);
	float anglezl = atan2(opponentLeft->getPositionInRelativeCoordinates().y, opponentLeft->getPositionInRelativeCoordinates().x);
	float distancezr = sqrt(opponentRight->getPositionInRelativeCoordinates().x*opponentRight->getPositionInRelativeCoordinates().x+opponentRight->getPositionInRelativeCoordinates().y*opponentRight->getPositionInRelativeCoordinates().y);
	float anglezr = atan2(opponentRight->getPositionInRelativeCoordinates().y, opponentRight->getPositionInRelativeCoordinates().x);

	float goallx, goally, goalrx, goalry ;

	goallx = 3000.0;
	goally = 700.0;
	goalrx = 3000.0;
	goalry = -700.0;

	map<int, tState>::iterator it;
	for(it = hyps.begin(); it!=hyps.end(); ++it)
	{
		Pose2D aux = (*it).second.pos;

		Vector2<double> lefta = Pose2D::relative2FieldCoord(aux, left->getPositionInRelativeCoordinates().x, left->getPositionInRelativeCoordinates().y);
		Vector2<double> righta = Pose2D::relative2FieldCoord(aux, right->getPositionInRelativeCoordinates().x, right->getPositionInRelativeCoordinates().y);


		float dl1 = sqrt((goallx - lefta.x) * (goallx - lefta.x) + (goally - lefta.y) *(goally - lefta.y));

		float difdist1 = fabs(distancezl-dl1);
		float pod1 = normalDist(difdist1/2000.0, 0.0, 0.6);

		float dl2 = sqrt((goalrx - righta.x) * (goalrx - righta.x) + (goalry - righta.y) *(goalry - righta.y));

		float difdist2 = fabs(distancezr-dl2);

		float pod2 = normalDist(difdist2/2000.0, 0.0, 0.6);

		(*it).second.prob = pod1 * pod2;// * (*it).second.prob) / 1.0);

	}

	normalize();

}

void
WorldState::ProcessPosts(ObjectState *left, ObjectState *right, list<AbstractSample> *mates, list<AbstractSample> *opponents)
{
	Pose2D vpose;
	float angle;

	angle = atan2(left->getPositionInRelativeCoordinates().y - right->getPositionInRelativeCoordinates().y, left->getPositionInRelativeCoordinates().x - right->getPositionInRelativeCoordinates().x);

	vpose.translation.x = 0.0;
	vpose.translation.y = 0.0;
	vpose.rotation = normalizePi((pi_2) - angle);


	Vector2<double> lefta = Pose2D::relative2FieldCoord(vpose, left->getPositionInRelativeCoordinates().x, left->getPositionInRelativeCoordinates().y);
	Vector2<double> righta = Pose2D::relative2FieldCoord(vpose, right->getPositionInRelativeCoordinates().x, right->getPositionInRelativeCoordinates().y);

	list<AbstractSample>::iterator itmates, itopponents;

	if(_SharedTeamInfo->getMyId()!=1)
		for(itmates = mates->begin(); itmates != mates->end(); ++itmates)
		{
			Vector2<double> mate = Pose2D::relative2FieldCoord(vpose, (*itmates).p3D.X/(*itmates).p3D.H, (*itmates).p3D.Y/(*itmates).p3D.H);

			//cerr<<"Mate dist: "<<fabs(mate.x - lefta.x)<<endl;

			if((fabs(mate.x - lefta.x) < 500.0) && (mate.y < (lefta.y + 300.0)) && (mate.y > (righta.y - 300.0)))
				UpdateOwnGoal(left, right);
		}

	for(itopponents = opponents->begin(); itopponents != opponents->end(); ++itopponents)
	{
		Vector2<double> oppo = Pose2D::relative2FieldCoord(vpose, (*itopponents).p3D.X/(*itopponents).p3D.H, (*itopponents).p3D.Y/(*itopponents).p3D.H);

		//cerr<<"Oppo dist: "<<fabs(oppo.x - lefta.x)<<endl;

		if((fabs(oppo.x - lefta.x) < 500.0) && (oppo.y < (lefta.y + 300.0)) && (oppo.y > (righta.y - 300.0)))
			UpdateOpponentGoal(left, right);
	}


}

void
WorldState::ProcessPosts(ObjectState *post, list<AbstractSample> *mates, list<AbstractSample> *opponents)
{
	ObjectState *p0l = &(_GoalDetector->goals->p0LeftEstimate);
	ObjectState *p0r = &(_GoalDetector->goals->p0RightEstimate);
	ObjectState *p1l = &(_GoalDetector->goals->p1LeftEstimate);
	ObjectState *p1r = &(_GoalDetector->goals->p1RightEstimate);

	float postx, posty;
	postx = post->getPositionInRelativeCoordinates().x;
	posty = post->getPositionInRelativeCoordinates().y;

	list<AbstractSample>::iterator itmates, itopponents;

	if(_SharedTeamInfo->getMyId()!=1)
		for(itmates = mates->begin(); itmates != mates->end(); ++itmates)
		{
			float matex, matey, dist;

			matex =  (*itmates).p3D.X/(*itmates).p3D.H;
			matey =  (*itmates).p3D.Y/(*itmates).p3D.H;

			dist = sqrt((matex-postx)*(matex-postx)+(matey-posty)*(matey-posty));
			if(dist < 300.0)
			{
				if(post == p1l)
					UpdateOwnGoal(p1l, p1r);
				if(post == p1r)
					UpdateOwnGoal(p1l, p1r);
				if(post == p0l)
					UpdateOwnGoal(p0l, p0r);
				if(post == p0r)
					UpdateOwnGoal(p0l, p0r);
			}
		}

	for(itopponents = opponents->begin(); itopponents != opponents->end(); ++itopponents)
	{
		float oppox, oppoy, dist;

		oppox =  (*itopponents).p3D.X/(*itopponents).p3D.H;
		oppoy =  (*itopponents).p3D.Y/(*itopponents).p3D.H;

		dist = sqrt((oppox-postx)*(oppox-postx)+(oppoy-posty)*(oppoy-posty));
		if(dist < 300.0)
		{
			if(post == p1l)
				UpdateOpponentGoal(p1l, p1r);
			if(post == p1r)
				UpdateOpponentGoal(p1l, p1r);
			if(post == p0l)
				UpdateOpponentGoal(p0l, p0r);
			if(post == p0l)
				UpdateOpponentGoal(p0l, p0r);
		}
	}

}

void
WorldState::updateRobots()
{

	ObjectState *p0l = &(_GoalDetector->goals->p0LeftEstimate);
	ObjectState *p0r = &(_GoalDetector->goals->p0RightEstimate);
	ObjectState *p1l = &(_GoalDetector->goals->p1LeftEstimate);
	ObjectState *p1r = &(_GoalDetector->goals->p1RightEstimate);

	/*cerr<<"["<<p0l->getReliability()<<"]\tp0 Left("<<p0l->getPositionInRelativeCoordinates().x<<","<<p0l->getPositionInRelativeCoordinates().y<<")"<<endl;
	cerr<<"["<<p0r->getReliability()<<"]\tp0 Right("<<p0r->getPositionInRelativeCoordinates().x<<","<<p0r->getPositionInRelativeCoordinates().y<<")"<<endl;
	cerr<<"["<<p1l->getReliability()<<"]\tp1 Left("<<p1l->getPositionInRelativeCoordinates().x<<","<<p1l->getPositionInRelativeCoordinates().y<<")"<<endl;
	cerr<<"["<<p1r->getReliability()<<"]\tp1 Right("<<p1r->getPositionInRelativeCoordinates().x<<","<<p1r->getPositionInRelativeCoordinates().y<<")"<<endl;
	 */
	list<AbstractSample> *mates, *opponents;
	mates = _RobotDetector->getTeammates();
	opponents = _RobotDetector->getOpponents();

	bool entera = false;

	if(((mates->size()+opponents->size())>0) && ((p0l->getReliability()+p0r->getReliability()+p1l->getReliability()+p1r->getReliability())>0))
	{
		if((p0l->getReliability()>ObjectState::HIGH_RELIABLE) && (p0r->getReliability()>ObjectState::HIGH_RELIABLE))
		{
			ProcessPosts(p0l, p0r, mates, opponents);
			entera = true;
		}

		if(!entera)
		{
			ObjectState *toprocess;
			int mayor = 0;

			if((p0l->getReliability()>mayor) && (p0l->getPositionInRelativeCoordinates().x>0.0))
			{
				toprocess = p0l;
				mayor = p0l->getReliability();
			}

			if((p0r->getReliability()>mayor)  && (p0r->getPositionInRelativeCoordinates().x>0.0))
			{
				toprocess = p0r;
				mayor = p0r->getReliability();
			}

			if((p1l->getReliability()>mayor)  && (p1l->getPositionInRelativeCoordinates().x>0.0))
			{
				toprocess = p1l;
				mayor = p1l->getReliability();
			}

			if((p1r->getReliability()>mayor)  && (p1r->getPositionInRelativeCoordinates().x>0.0))
			{
				toprocess = p1r;
				mayor = p1r->getReliability();
			}

			ProcessPosts(toprocess, mates, opponents);
		}
	}
}


void
WorldState::updateWorld()
{
	int idmayor;
	float mayor=0.0;

	map<int, tState>::iterator itm, itaux;
	list<int>::iterator iti;
	for(itm = hyps.begin(); itm!=hyps.end(); ++itm)
	{
		/*cerr<<"["<<(*itm).second.id<<"]\t"<<(*itm).second.prob<<"\t("<<(*itm).second.pos.translation.x<<", "
				<<(*itm).second.pos.translation.y<<", "
				<<(*itm).second.pos.rotation<<")"<<endl;
		 */
		if((*itm).second.prob > mayor)
		{
			mayor = (*itm).second.prob;
			idmayor = (*itm).second.id;
		}
	}

	myPos = hyps[idmayor].pos;
	myReliability = mayor;

	setGoals();

}

void
WorldState::inMyField()
{
	map<int, tState>::iterator it;
	for(it = hyps.begin(); it!=hyps.end(); ++it)
		if((*it).second.pos.translation.x>0.0)
			(*it).second.prob = 0.0;
}

void
WorldState::inOppField()
{
	map<int, tState>::iterator it;
	for(it = hyps.begin(); it!=hyps.end(); ++it)
		if((*it).second.pos.translation.x<0.0)
			(*it).second.prob = 0.0;
}

void
WorldState::updateGoalie(void)
{
	inMyField();

}

void
WorldState::step(void)
{
	_Localization->step();
	_BallDetector->step();
	_GoalDetector->step();
	_RobotDetector->step();
	_SharedTeamInfo->step();

	if (isTime2Run())
	{
		startDebugInfo();
		//cerr<<"=========================================================="<<endl;
		updateLocalHipotheses();

		//For testing!!!!!!!!!
		inMyField();

		if((_SharedTeamInfo->getMyId()==1))
		{
			updateGoalie();
			updateWorld();
			updateRobots();
			updateGVector();
		}else
		{
			updateBall();
			updateWorld();
			updateRobots();
			updateGVector();
		}

		// Log
		beginLog();
		double robotX, robotY, robotTh;
		_GTLocalization->getPosition( robotX, robotY, robotTh );
		addToLog ( "ROBOT_GT_X", ComponentSnapshot::LOG_DOUBLE, robotX);
		addToLog ( "ROBOT_GT_Y", ComponentSnapshot::LOG_DOUBLE, robotY );
		addToLog ( "ROBOT_GT_T", ComponentSnapshot::LOG_DOUBLE, robotTh );
		addToLog ( "robotPosX", ComponentSnapshot::LOG_DOUBLE, myPos.translation.x );
		addToLog ( "robotPosY", ComponentSnapshot::LOG_DOUBLE, myPos.translation.y );
		addToLog ( "robotPosT", ComponentSnapshot::LOG_DOUBLE, myPos.rotation );
		addToLog ( "robotPosR", ComponentSnapshot::LOG_DOUBLE, (double)myReliability );
		endLog( getName() );

		/*map<int, tState>::iterator it;
			for(it = hyps.begin(); it!=hyps.end(); ++it)
				cerr<<"\t["<<(*it).second.id<<"] ("<<(*it).second.pos.translation.x<<", "<<(*it).second.pos.translation.y<<", "<<(*it).second.pos.rotation<<") \t"<<(*it).second.prob<<endl;



		cerr<<"["<<myPos.translation.x<<", "<<myPos.translation.y<<", "<<myPos.rotation<<"]"<<endl;
		 */

		/*cerr<<"Opp:\t("<<(opponentLeft->getPositionInRelativeCoordinates().x + opponentLeft->getPositionInRelativeCoordinates().x) /2.0<<", "<<
				(opponentLeft->getPositionInRelativeCoordinates().y + opponentLeft->getPositionInRelativeCoordinates().y) /2.0<<")"<<endl;
		cerr<<"Own:\t("<<(ownLeft->getPositionInRelativeCoordinates().x + ownLeft->getPositionInRelativeCoordinates().x) /2.0<<", "<<
				(ownRight->getPositionInRelativeCoordinates().y + ownRight->getPositionInRelativeCoordinates().y) /2.0<<")"<<endl;
		cerr<<"Vector Right: ("<<vright.x<<", "<<vright.y<<")"<<endl;
		cerr<<"Vector Left: ("<<vleft.x<<", "<<vleft.y<<")"<<endl;
		 */

		/*myPos.translation.x = 0.0;
		myPos.translation.y = -2000.0;
		myPos.rotation = pi_2;*/
		//cerr<<"____________________________________________________________"<<endl;
		endDebugInfo();
	}
}

void
WorldState::updateGVector()
{

	if((opponentLeft->getReliability()+opponentRight->getReliability()+ownLeft->getReliability()+ownRight->getReliability())>0)
	{
		if((opponentLeft->getReliability()+opponentRight->getReliability())>=(ownLeft->getReliability()+ownRight->getReliability()))
		{

			vleft.x = opponentLeft->getPositionInRelativeCoordinates().x;
			vleft.y = opponentLeft->getPositionInRelativeCoordinates().y;
			vright.x = opponentRight->getPositionInRelativeCoordinates().x;
			vright.y = opponentRight->getPositionInRelativeCoordinates().y;

		}else
		{

			Vector2<double> aux, net;

			net.x = ownLeft->getPositionInRelativeCoordinates().x;
			net.y = ownLeft->getPositionInRelativeCoordinates().y;
			aux = Pose2D::relative2FieldCoord(myPos, net.x, net.y);

			aux.x = aux.x + 6000.0;
			vright = Pose2D::fieldCoord2Relative(myPos, aux);

			net.x = ownRight->getPositionInRelativeCoordinates().x;
			net.y = ownRight->getPositionInRelativeCoordinates().y;
			aux = Pose2D::relative2FieldCoord(myPos, net.x, net.y);

			aux.x = aux.x + 6000.0;
			vleft = Pose2D::fieldCoord2Relative(myPos, aux);

		}
	}else
	{

		Vector2<double> aux;

		aux.x = 3000.0;
		aux.y = 700.0;
		vleft = Pose2D::fieldCoord2Relative(myPos, aux);

		aux.x = 3000.0;
		aux.y = -700.0;
		vright = Pose2D::fieldCoord2Relative(myPos, aux);
	}

}

void
WorldState::setGoals()
{

	ObjectState *p0l = &(_GoalDetector->goals->p0LeftEstimate);
	ObjectState *p0r = &(_GoalDetector->goals->p0RightEstimate);
	ObjectState *p1l = &(_GoalDetector->goals->p1LeftEstimate);
	ObjectState *p1r = &(_GoalDetector->goals->p1RightEstimate);

	float p0x = (p0l->getPositionInRelativeCoordinates().x + p0r->getPositionInRelativeCoordinates().x) /2.0;
	float p0y = (p0l->getPositionInRelativeCoordinates().y + p0r->getPositionInRelativeCoordinates().y) /2.0;
	float p1x = (p1l->getPositionInRelativeCoordinates().x + p1r->getPositionInRelativeCoordinates().x) /2.0;
	float p1y = (p1l->getPositionInRelativeCoordinates().y + p1r->getPositionInRelativeCoordinates().y) /2.0;

	Vector2<double> posep0 = Pose2D::relative2FieldCoord(myPos, p0x, p0y);
	Vector2<double> posep1 = Pose2D::relative2FieldCoord(myPos, p1x, p1y);

	if((p0l->getReliability()+p0r->getReliability())>(p1l->getReliability()+p1r->getReliability()))
	{
		if(posep0.x>posep1.x)
		{
			opponentLeft = p0l;
			opponentRight = p0r;
			ownLeft = p1l;
			ownRight = p1r;
		}else
		{
			opponentLeft = p1l;
			opponentRight = p1r;
			ownLeft = p0l;
			ownRight = p0r;
		}
	}else
	{
		if(posep1.x>posep0.x)
		{
			opponentLeft = p1l;
			opponentRight = p1r;
			ownLeft = p0l;
			ownRight = p0r;
		}else
		{
			opponentLeft = p0l;
			opponentRight = p0r;
			ownLeft = p1l;
			ownRight = p1r;
		}
	}
	/*
	cerr<<"OPL: ("<<opponentLeft->getPositionInRelativeCoordinates().x<<", "<<opponentLeft->getPositionInRelativeCoordinates().y<<")"<<endl;
	cerr<<"OPR: ("<<opponentRight->getPositionInRelativeCoordinates().x<<", "<<opponentRight->getPositionInRelativeCoordinates().y<<")"<<endl;
	cerr<<"OWL: ("<<ownLeft->getPositionInRelativeCoordinates().x<<", "<<ownLeft->getPositionInRelativeCoordinates().y<<")"<<endl;
	cerr<<"OWR: ("<<ownRight->getPositionInRelativeCoordinates().x<<", "<<ownRight->getPositionInRelativeCoordinates().y<<")"<<endl;
	 */
}


