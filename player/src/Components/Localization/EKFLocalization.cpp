#include "EKFLocalization.h"

#define GOAL_POST_VISIBLE_CONF 0.5
#define GOAL_POST_DISTANCE_MIN_STD_DEV 300
#define GOAL_POST_DISTANCE_MAX_STD_DEV 1000
#define GOAL_POST_ANGLE_MIN_STD_DEV 10*M_PI/180
#define GOAL_POST_ANGLE_MAX_STD_DEV 15*M_PI/180
#define GOAL_POST_MAX_STD_DEV_DISTANCE 4000

#define MOTION_UNIT_STD_DEV_X 0.3
#define MOTION_UNIT_STD_DEV_Y 0.3
// error in degrees per cm
#define MOTION_UNIT_STD_DEV_ANGLE_PER_Y (1)
// error per degree
#define MOTION_UNIT_STD_DEV_ANGLE (0.4*M_PI/180)

//Cell grid size
#define MAX_FILTERS 6

typedef struct
{
	wmLine *line;
	float points;
}LineCandidate;



EKFLocalization::EKFLocalization()
{

	_MovementModel = MovementModel::getInstance();
	_WorldModel = WorldModel::getInstance();

	_GoalDetector = GoalDetector::getInstance();
	_LineDetector = LineDetector::getInstance();

	pthread_mutex_init(&mutex, NULL);

	id = 0;

	GameEvent = -1;
}

EKFLocalization::~EKFLocalization() {

}

void EKFLocalization::init(const string newName,
		AL::ALPtr<AL::ALBroker> parentBroker) {
	Component::init(newName, parentBroker);

	setFreqTime(MEDIUM_RATE);

	ekf.filter = new EKF_3D();

	MatrixCM sinit, Pinit;
	sinit.resize(3, 1);
	sinit.sete(0,0,0.0);
	sinit.sete(1,0,0.0);
	sinit.sete(2,0,0.0);

	Pinit.identity(3);
	Pinit.sete(0, 0, 3000.0*3000.0);
	Pinit.sete(1, 1, 2000.0*2000.0);
	Pinit.sete(2, 2, M_PI * M_PI);

	ekf.filter->restart(sinit, Pinit);

	mgrid = new MarkovGrid();

	try
	{
		pmotion = parentBroker->getMotionProxy();
		lastpos = pmotion->getRobotPosition(USE_SENSOR);
	} catch (AL::ALError& e) {
		lastpos.push_back(0.0);
		lastpos.push_back(0.0);
		lastpos.push_back(0.0);
		lastpos.push_back(0.0);
		lastpos.push_back(0.0);
		lastpos.push_back(0.0);
		cerr << "EKFLocalization::EKFLocalization [motion]" << e.toString() << endl;
	}

	// lastpos = pmotion->getRobotPosition(USE_SENSOR);
	//_MovementModel->initMovement();
}

float EKFLocalization::getReliability() {
	return last_quality;
}


void
EKFLocalization::removeFilters()
{
}

void
EKFLocalization::resetUniform()
{
	mgrid->resetUniform();

	MatrixCM sinit, Pinit;
	sinit.resize(3, 1);
	sinit.sete(0,0,0.0);
	sinit.sete(1,0,0.0);
	sinit.sete(2,0,0.0);

	Pinit.identity(3);
	Pinit.sete(0, 0, 3000.0*3000.0);
	Pinit.sete(1, 1, 2000.0*2000.0);
	Pinit.sete(2, 2, M_PI * M_PI);

	ekf.filter->restart(sinit, Pinit);
}

void EKFLocalization::addPosition(float x, float y, float theta)
{

	MatrixCM sinit, Pinit;
	sinit.resize(3, 1);
	sinit.sete(0,0,x);
	sinit.sete(1,0,y);
	sinit.sete(2,0,theta);

	Pinit.identity(3);
	Pinit.sete(0, 0, 1000.0*1000.0);
	Pinit.sete(1, 1, 1000.0*1000.0);
	Pinit.sete(2, 2, 1.0 * 1.0);

	ekf.filter->restart(sinit, Pinit);

	mgrid->addPosition(x,y,theta);
}


tEKF EKFLocalization::getBestEKF()
{

	return ekf;
}

void EKFLocalization::predict() {

	float movx, movy, movt;
	float diffx, diffy, difft;

	bool apply;

	vector<float> pos = pmotion->getRobotPosition(USE_SENSOR);

	diffx = pos[0] *1000.0 - lastpos[0]*1000.0;
	diffy = pos[1] *1000.0 - lastpos[1]*1000.0;
	difft = normalizePi(pos[2] - lastpos[2]);

	movx = diffx*cos(-lastpos[2]) - diffy*sin(-lastpos[2]);
	movy = diffx*sin(-lastpos[2]) + diffy*cos(-lastpos[2]);
	movt = difft;

	lastpos = pos;

	if((movx>0.0001) || (movy>0.0001) || (movt>0.001))
	{
		mgrid->predict(movx, movy, movt);
		ekf.filter->predict(movx, movy, movt);
	}
}

void EKFLocalization::correct()
{
	float prob = 1.0;
	float p;
	bool changed = false;
	bool ambiguous = true;

	HPoint3D *lmbl,*lmbr,*lmyl,*lmyr;
	lmbl = &(_WorldModel->getPoint("p49")->p);
	lmbr = &(_WorldModel->getPoint("p51")->p);
	lmyl = &(_WorldModel->getPoint("p55")->p);
	lmyr = &(_WorldModel->getPoint("p53")->p);

	//cerr<<"["<<_GoalDetector->getObj()->p0ElapsedTimeSinceLastObs<<","<<_GoalDetector->getObj()->p1ElapsedTimeSinceLastObs<<"]"<<endl;

	if(_perception.p0ElapsedTimeSinceLastObs < ObjectState::LONG_TIME)
	{
		//cerr<<"P0";
		HPoint2D official_pos;
		HPoint2D perceived_pos;

		if(_perception.p0l.reliability >= ObjectState::HIGH_RELIABLE)
		{
			//cerr<<"L";
			updateLm(lmbl, _perception.p0l, ambiguous);
			changed = true;
		}


		if(_perception.p0r.reliability >= ObjectState::HIGH_RELIABLE)
		{
			//cerr<<"R";
			updateLm(lmbr, _perception.p0r, ambiguous);
			changed = true;
		}
	}
	if(_perception.p1ElapsedTimeSinceLastObs < ObjectState::LONG_TIME)
	{
		//cerr<<"P1 ["<<goalp1l->getReliability()<<","<<goalp1r->getReliability()<<"]"<<endl;
		if(_perception.p1l.reliability >= ObjectState::HIGH_RELIABLE)
		{
			//cerr<<"L";
			updateLm(lmyl, _perception.p1l, ambiguous);
			changed = true;
		}

		if(_perception.p1r.reliability >= ObjectState::HIGH_RELIABLE)
		{
			//cerr<<"R";
			updateLm(lmyr, _perception.p1r, ambiguous);
			changed = true;
		}
	}

	/*CircleSample *circle = _LineDetector->getCircle();

	if(false && circle->found)
	{
		HPoint3D *lm = new HPoint3D();
		lm->X = 0.0;
		lm->Y = 0.0;
		lm->Z = 0.0;
		updateCircle(lm, circle);
		changed = true;
	}*/

	if(changed)
		mgrid->normalize();
}


void
EKFLocalization::updateLm(HPoint3D *wmlm, PostInfo &perception, bool ambiguous)
{

	//cerr<<"LM"<<endl;
	HPoint2D official_pos,  official_pos2;
	HPoint2D perceived_pos,  perceived_pos2;
	float angStdev, disStdev;

	perceived_pos.x = perception.x;
	perceived_pos.y = perception.y;
	official_pos.x = wmlm->X;
	official_pos.y = wmlm->Y;
	official_pos2.x = -wmlm->X;
	official_pos2.y = -wmlm->Y;

	angStdev = getGoalPostAngleStdDev(perceived_pos);
	disStdev = getGoalPostDistanceStdDev(perceived_pos);

	ApplyLandmarkObs(ekf.filter, perceived_pos, official_pos, angStdev, disStdev);
	ApplyLandmarkObs(ekf.filter, perceived_pos, official_pos2, angStdev, disStdev);

	mgrid->correct(perceived_pos, official_pos, angStdev, disStdev, ambiguous);

}

void
EKFLocalization::updateCircle(HPoint3D *wmlm, CircleSample *circle)
{
	HPoint2D official_pos;
	HPoint2D perceived_pos;
	float angStdev, disStdev;

	perceived_pos.x = circle->p3D.X;
	perceived_pos.y = circle->p3D.Y;
	official_pos.x = wmlm->X;
	official_pos.y = wmlm->Y;

	angStdev = getGoalPostAngleStdDev(perceived_pos);
	disStdev = getGoalPostDistanceStdDev(perceived_pos);

	ApplyLandmarkObs(ekf.filter, perceived_pos, official_pos, angStdev, disStdev);

	mgrid->correct(perceived_pos, official_pos, angStdev, disStdev);
}

void
EKFLocalization::ApplyLandmarkObs(EKF_3D *filter, HPoint2D obs,
		HPoint2D lm, float ang_std_dev, float dist_std_dev)
{

	MatrixCM R(2, 2);
	R.identity(2);

	R.sete(0,0,dist_std_dev*dist_std_dev);
	R.sete(1,1,ang_std_dev*ang_std_dev);

	HPoint3D lma, obsa;

	lma.X = lm.x;
	lma.Y = lm.y;
	lma.Z = 0.0;
	lma.H = 1.0;

	obsa.X = obs.x;
	obsa.Y = obs.y;
	obsa.Z = 0.0;
	obsa.H = 1.0;

	filter->correct(lma, obsa, R);

}

void
EKFLocalization::WTDreset2Initial(RoboCupGameControlData *gc, float MT, float MO)
{

	mgrid->resetClear();
	removeFilters();

	switch(GameController::getInstance()->getPlayerNumber()){
	case 1:
		//addPosition(-2000.0, -1900.0, pi_2);
		addPosition(-2000.0, 1900.0, -pi_2);
		break;
	case 2:
		//addPosition(-1500.0, -1900.0, pi_2);
		addPosition(-1500.0, 1900.0, -pi_2);
		break;
	case 3:
		addPosition(-1000.0, -1900.0, pi_2);
		//addPosition(-1000.0, 1900.0, -pi_2);
		break;
	case 4:
		addPosition(-500.0, -1900.0, pi_2);
		//addPosition(-500.0, 1900.0, -pi_2);
		break;
	};

	mgrid->normalize();
}

void
EKFLocalization::WTDreset2Ready(RoboCupGameControlData *gc, float MT, float MO)
{
	mgrid->resetClear();
	removeFilters();

	switch(GameController::getInstance()->getPlayerNumber()){
	case 1:
		addPosition(-2000.0, -1900.0, 0.0);
		break;
	case 2:
		addPosition(-1500.0, -1900.0, 0.0);
		break;
	case 3:
		addPosition(-1000.0, -1900.0, 0.0);
		break;
	case 4:
		addPosition(-500.0, -1900.0, 0.0);
		break;
	};

	mgrid->normalize();
}

void
EKFLocalization::WTDreset2Set(RoboCupGameControlData *gc, float MT, float MO)
{
	mgrid->resetClear();
	removeFilters();

	switch(GameController::getInstance()->getPlayerNumber())
	{
	case 1:
		addPosition(-2500.0, 0.0, 0.0);
		break;
	case 2:
		if(GameController::getInstance()->isMyKickOff())
			addPosition(-1500.0, 1500.0, 0.0);
		else
			addPosition(-2000.0, 1500.0, 0.0);
		break;
	case 3:
		if(GameController::getInstance()->isMyKickOff())
			addPosition(-1000.0, 0.0, 0.0);
		else
			addPosition(-2000.0, 250.0, 0.0);
		break;
	case 4:
		if(GameController::getInstance()->isMyKickOff())
			addPosition(-1500.0, -1500.0, 0.0);
		else
			addPosition(-2000.0, -1500.0, 0.0);
		break;
	};

	mgrid->normalize();
}

void
EKFLocalization::WTDreset2Playing(RoboCupGameControlData *gc, float MT, float MO)
{
	mgrid->resetClear();
	removeFilters();

	switch(GameController::getInstance()->getPlayerNumber())
	{
	case 1:
		addPosition(-2500.0, 0.0, 0.0);
		break;
	case 2:
		if(GameController::getInstance()->isMyKickOff())
			addPosition(-1500.0, 1500.0, 0.0);
		else
			addPosition(-2000.0, 1500.0, 0.0);
		break;
	case 3:
		if(GameController::getInstance()->isMyKickOff())
			addPosition(-1000.0, 0.0, 0.0);
		else
			addPosition(-2000.0, 250.0, 0.0);
		break;
	case 4:
		if(GameController::getInstance()->isMyKickOff())
			addPosition(-1500.0, -1500.0, 0.0);
		else
			addPosition(-2000.0, -1500.0, 0.0);
		break;
	};

	mgrid->normalize();
}

void
EKFLocalization::WTDreset2Penalty(RoboCupGameControlData *gc, float MT, float MO)
{
	mgrid->resetClear();
	removeFilters();

	switch(GameController::getInstance()->getPlayerNumber())
	{
	case 1:
		addPosition(-3000.0, 0.0, 0.0);
		break;
	case 2:
	case 3:
	case 4:
		addPosition(700.0, 0.0, 0.0);
		break;
	};

	mgrid->normalize();
}

void
EKFLocalization::WTDresetFromPenalised(RoboCupGameControlData *gc, float MT, float MO)
{
	mgrid->resetClear();
	removeFilters();

	addPosition(-1500.0, -2000.0, pi_2);
	addPosition(-1500.0, 2000.0, -pi_2);

	mgrid->normalize();
}

void
EKFLocalization::resetToPosition(int state)
{
	GameEvent = state;
}

void EKFLocalization::testGameEvents()
{
	//cerr<<"EKFLocalization::testGameEvents()"<<endl;
	if(GameEvent==-1)
		return;

	RoboCupGameControlData *gc = GameController::getInstance()->getRgc();
	//Vamos a usar modificadores, porque internamente siempre trabajamos
	//sin tener en cuenta el color del equipo (X positivo hacia portería azul).
	//En estas funciones ponemos coordenadas "coloreadas"

	float MT, MO;
	if (GameController::getInstance()->getMyColor() == TEAM_RED){
		MT = -1.0; MO = M_PI;
	}else{
		MT = 0.0;MO = M_PI;
	}

	switch(GameEvent)
	{
	case reset2Initial: 		WTDreset2Initial(gc, MT, MO); break;
	case reset2Ready: 			WTDreset2Ready(gc, MT, MO); break;
	case reset2Set: 			WTDreset2Set(gc, MT, MO); break;
	case reset2Playing: 		WTDreset2Playing(gc, MT, MO); break;
	case resetFromPenalised: 	WTDresetFromPenalised(gc, MT, MO); break;
	case reset2Penalty: 		WTDreset2Penalty(gc, MT, MO); break;
	}

	GameEvent=-1;
}

float EKFLocalization::getQ(EKF_3D *ekf)
{
	float q1 = mgrid->evaluate(ekf->x(),ekf->y(), ekf->t());
	float q2 = (((1000.0 - ekf->Px())/1000.0) + ((1000.0 - ekf->Py())/1000.0))/2.0;

	//cerr<<"\t"<<q1<<"\t"<<q2<<endl;
	if(q2<0.0) q2 = 0.0;
	if(q2>1.0) q2 = 1.0;
	if(q1>1.0) q1 = 1.0;

	return (q1+q2)/2.0;
}

void EKFLocalization::run()
{

	testGameEvents();

	predict();
	correct();


	float q =  getQ(ekf.filter);

	//cerr<<"Q = "<<q<<"\t";
	if(q<0.5)
	{
		list<tcellcand> cands;
		list<tcellcand>::iterator itcands, mayor;
		float qmayor=0.0;
		cands.clear();

		mgrid->getGridPos(cands);

		if(cands.size()>0)
			for(itcands=cands.begin(); itcands!=cands.end();++itcands)
				if((*itcands).prob>qmayor)
				{
					mayor = itcands;
					qmayor = (*itcands).prob;
				}
		//cerr<<"mayor = "<<qmayor<<"\t";
		if(qmayor>0.35)
		{
			MatrixCM s(3,1);
			s.sete(0,0,(*mayor).x);
			s.sete(1,0,(*mayor).y);
			s.sete(2,0,(*mayor).t);

			MatrixCM Ppos(3);
			Ppos.identity(3);
			Ppos.sete(0,0,(*mayor).uxy);
			Ppos.sete(1,1,(*mayor).uxy);
			Ppos.sete(2,2,(*mayor).ut);

			ekf.filter->restart(s, Ppos);
		}

	}
	//cerr<<endl;
}

void EKFLocalization::UpdateInfo()
{
	//motion
	float diffx, diffy, difft;

	bool apply;

	vector<float> pos = pmotion->getRobotPosition(USE_SENSOR);

	diffx = pos[0] *1000.0 - lastpos[0]*1000.0;
	diffy = pos[1] *1000.0 - lastpos[1]*1000.0;
	difft = normalizePi(pos[2] - lastpos[2]);

	_motion.movx = diffx*cos(-lastpos[2]) - diffy*sin(-lastpos[2]);
	_motion.movy = diffx*sin(-lastpos[2]) + diffy*cos(-lastpos[2]);
	_motion.movt = difft;

	lastpos = pos;

	//perception info
	ObjectState* goalp0r = &(_GoalDetector->goals->p0RightEstimate);
	ObjectState* goalp0l = &(_GoalDetector->goals->p0LeftEstimate);
	ObjectState* goalp1r = &(_GoalDetector->goals->p1RightEstimate);
	ObjectState* goalp1l = &(_GoalDetector->goals->p1LeftEstimate);

	_perception.p0ElapsedTimeSinceLastObs = _GoalDetector->getObj()->p0ElapsedTimeSinceLastObs;
	_perception.p1ElapsedTimeSinceLastObs = _GoalDetector->getObj()->p1ElapsedTimeSinceLastObs;

	_perception.p0r.reliability = goalp0r->getReliability();
	_perception.p0r.x = goalp0r->getPositionInRelativeCoordinates().x;
	_perception.p0r.y = goalp0r->getPositionInRelativeCoordinates().y;

	_perception.p0l.reliability = goalp0l->getReliability();
	_perception.p0l.x = goalp0l->getPositionInRelativeCoordinates().x;
	_perception.p0l.y = goalp0l->getPositionInRelativeCoordinates().y;

	_perception.p1r.reliability = goalp1r->getReliability();
	_perception.p1r.x = goalp1r->getPositionInRelativeCoordinates().x;
	_perception.p1r.y = goalp1r->getPositionInRelativeCoordinates().y;

	_perception.p1l.reliability = goalp1l->getReliability();
	_perception.p1l.x = goalp1l->getPositionInRelativeCoordinates().x;
	_perception.p1l.y = goalp1l->getPositionInRelativeCoordinates().y;

}

void EKFLocalization::setInfo(MotionInfo &motion, PerceptionInfo &perception)
{
	memcpy(&_motion, &motion, sizeof(motion));
	memcpy(&_perception, &perception, sizeof(perception));
}

void EKFLocalization::step(void) {


	_GoalDetector->step();
	_LineDetector->step();

	if (isTime2Run()) {
		startDebugInfo();
		UpdateInfo();
		run();
		endDebugInfo();

	}
}

void
EKFLocalization::getPosition(vector<tRobotHipothesis> &hipotheses)
{
	hipotheses.clear();

			tRobotHipothesis aux;

			aux.x = ekf.filter->x();
			aux.y = ekf.filter->y();
			aux.t = ekf.filter->t();
			aux.prob = getQ(ekf.filter);
			aux.id = 0;
			hipotheses.push_back(aux);

			aux.x = -ekf.filter->x();
			aux.y = -ekf.filter->y();
			aux.t = normalizePi(ekf.filter->t()+M_PI);
			aux.id = 1;
			hipotheses.push_back(aux);


}

void EKFLocalization::getPosition(double &xpos, double &ypos, double &thetapos) {

	tEKF best = getBestEKF();

	xpos = best.filter->x();
	ypos = best.filter->y();
	thetapos = best.filter->t();
}

float EKFLocalization::getQuality()
{
	float Q;
	Q = last_quality/100.0;
	if(Q>1.0) Q = 1.0;


	return Q;
}

ImageDataPtr EKFLocalization::getImgDebug() {
	return 0;
}

string EKFLocalization::getInfo() {
	return getDbgInfo();
}

string EKFLocalization::getDbgInfo() {
	//DoWork();
	int mayor = -2;



			mayor = ekf.filter->getObsCounter();

	std::stringstream out;

	out << ":"<<ekf.filter->getDbgInfo()<<":";

	out << endl;
	string s = out.str();

	return s;
}

void EKFLocalization::initEKFtoGrid(EKF_3D &resekf)
{

	/*if(contang>0)
		angle = angle / (float)contang;
	else
		angle = 0.0;

	MatrixCM sinit, Pinit;
	sinit.resize(3, 1);
	sinit.sete(0,0,gridpos.x);
	sinit.sete(1,0,gridpos.y);
	sinit.sete(2,0,angle);

	Pinit.identity(3);
	Pinit.sete(0, 0, ((Muncx+(CELL_SIZE/2.0))-(muncx-(CELL_SIZE/2.0))) * ((Muncx+(CELL_SIZE/2.0))-(muncx-(CELL_SIZE/2.0))));
	Pinit.sete(1, 1, ((Muncy+(CELL_SIZE/2.0))-(muncy-(CELL_SIZE/2.0))) * ((Muncy+(CELL_SIZE/2.0))-(muncy-(CELL_SIZE/2.0))));
	Pinit.sete(2, 2, 1.0 * 1.0);

	resekf.filter->restart(sinit, Pinit);*/


}


/*void EKFLocalization::reset(Localization::ResetCase resetCase) {
	resetUniform();

	if (resetCase == Localization::ourPenaltyBox) {
    resetPenaltyBox(true);
  } else if (resetCase == Localization::opponentPenaltyBox) {
    resetPenaltyBox(false);
  } else if (resetCase == Localization::goalieStartInPenaltyBox) {
    resetGoalieForKickoff();
  } else if (resetCase == Localization::opponentGoalieStartInPenaltyBox) {
    resetOpponentGoalieForKickoff();
  } else if (resetCase == Localization::backFromPenalty) {
    resetFromPenalty();
  } else if (resetCase == Localization::fall) {
    resetFromFall();
  } else if (resetCase == Localization::lifted) {
    resetLifted();
  }
  else if (resetCase == Localization::dribblerPosition) {
    resetDribbler();
  }
  else if (resetCase == Localization::goaliePenaltyKick) {
    resetGoaliePenaltyKick();
  }
  else if (resetCase == Localization::attackerPenaltyKick) {
    resetAttackerPenaltyKick();
  }
}*/

float EKFLocalization::getGoalCircleProb(float x, float y)
{
	float prob = 1.0;

	/*
	 float p;

	CircleSample *circle = _LineDetector->getCircle();

	if(circle->found)
	{
		HPoint2D official_pos;
		HPoint2D perceived_pos;

		perceived_pos.x = circle->p3D.X;
		perceived_pos.y = circle->p3D.Y;

		official_pos.x = 0.0;
		official_pos.y = 0.0;

		p = getLandmarkProb(x, y, perceived_pos, official_pos, getGoalPostDistanceStdDev(perceived_pos));
		prob *= p;

	}
	 */
	return prob;
}

Pose2D EKFLocalization::getIdealPose(Pose2D &robotPose, LineSample &detline, wmLine *wmline)
{

	Pose2D ret;

	Vector2<double> pd, p;
	float angle_p, dist_p;

	pd.x = (detline.ps3D.X+detline.pe3D.X)/2.0;
	pd.y = (detline.ps3D.Y+detline.pe3D.Y)/2.0;


	p = Pose2D::relative2FieldCoord(robotPose, pd.x, pd.y);

	//cerr<<"("<<pd.x<<","<<pd.y<<") -> ("<<p.x<<","<<p.y<<")"<<endl;


	angle_p = atan2(p.y - robotPose.translation.y, p.x - robotPose.translation.x);
	dist_p = sqrt(pd.x*pd.x+pd.y*pd.y);


	//cerr<<"Dist = "<<dist_p<<"\tangle = "<<angle_p<<endl;

	Vector2<double> vd;
	vd.x = cos(angle_p);
	vd.y = sin(angle_p);

	double A,B,C;

	getEqLine(robotPose.translation, vd, A, B, C);

	Vector2<double> i;
	getInterseccLines (A, B, C, wmline->A, wmline->B, wmline->C, i);

	//cerr<<"intersecction in ("<<i.x<<","<<i.y<<")"<<endl;

	ret.translation.x = i.x + (-vd.x * dist_p);
	ret.translation.y = i.y + (-vd.y * dist_p);

	//Rotación
	//el ángulo de la línea es wmline->angle
	//yo lo veo con un ángulo angle_z
	// mi orientación será wm_line - angle_z (o dado la vuelta);

	Vector2<double> ps3Da, pe3Da;
	ps3Da = Pose2D::relative2FieldCoord(robotPose, detline.ps3D.X, detline.ps3D.Y);
	pe3Da = Pose2D::relative2FieldCoord(robotPose, detline.pe3D.X, detline.pe3D.Y);

	double angle_z = atan2(detline.ps3D.Y-detline.pe3D.Y, detline.ps3D.X-detline.pe3D.X);
	double angle_za = atan2(ps3Da.y-pe3Da.y, ps3Da.x-pe3Da.x);
	double diff;


	/*diff = normalize(wmline->angle - angle_z);
	if(fabs(diff) > (pi_2))
		diff = normalize(wmline->angle + angle_z);

	ret.rotation = robotPose.rotation - diff;

	 */
	//if(fabs(angle_z) > (pi_2))
	//	angle_z = normalize(angle_z+M_PI);

	ret.rotation = normalizePi(wmline->angle - angle_z);

	if(fabs(ret.rotation-robotPose.rotation)>(pi_2))
		ret.rotation = normalizePi(ret.rotation+M_PI);
	return ret;

	/*
	//Obtenemos la recta que une la posición del robot con la línea del mundo a la que dice que está
	float Apwm, Bpwm, Cpwm;

	Apwm = normalize(wmline->angle + (pi_2));
	Bpwm = -1.0;
	Cpwm = (-Apwm*robotPose.x + robotPose.y);

	//Obtención del punto de intersección entre ambas rectas
	float D, E, F;
	float ix, iy;

	D = wmline->A;
	E = wmline->B;
	F = wmline->C;

	if(A!=0.0)
	{
		iy = (((D*C)/A)- F )/(((-DB)/A) + E);
		ix = ((-B*iy)-C)/A;
	}else
	{
		iy = (((A*F)/D)- C )/(((-EA)/D) + B);
		ix = ((-E*iy)-F)/D;
	}

	float rx, ry;

	rx = robotPose.translation.x;
	ry = robotPose.translation.y;


	//Y ahora la recta detectada
	Vector2<double> ps3D, pe3D;
	ps3D = Pose2D::relative2FieldCoord(robotPose, detline.ps3D.X/detline.ps3D.H, detline.ps3D.Y/detline.ps3D.H);
	pe3D = Pose2D::relative2FieldCoord(robotPose, detline.pe3D.X/detline.pe3D.H, detline.pe3D.Y/detline.pe3D.H);
	float Adet, Bdet, Cdet;

	Adet = normalize(atan2(ps3D.y-pe3D.y, ps3D.x-pe3D.x));
	Bdet = -1.0;
	Cdet = (-Adet*robotPose.x + robotPose.y);

	//Obtenemos la distancia del robot a la línea observada

	float distDet = fabs(Adet*rx+Bdet*yr+Cdet)/sqrt(Adet*Adet+Bdet*Bdet);



/*	Vector2<double> ps3D, pe3D;
	HPoint3D p1, p2;

	ps3D = Pose2D::relative2FieldCoord(robotPose, detline.ps3D.X/detline.ps3D.H, detline.ps3D.Y/detline.ps3D.H);
	pe3D = Pose2D::relative2FieldCoord(robotPose, detline.pe3D.X/detline.pe3D.H, detline.pe3D.Y/detline.pe3D.H);


	float diffAngle;
	float detangle;

	detangle = atan2(ps3D.y-pe3D.y, ps3D.x-pe3D.x);
	diffangle = normalize(detangle - wmline->angle);

	if(diffangle>(pi_2))
		diffangle =  normalize(diffangle - (pi_2));
	if(diffangle<(-pi_2))
		diffangle =  normalize(diffangle + (pi_2));

	 */

}

void EKFLocalization::correctLines(EKF_3D &filter)
{
	Pose2D robotPose;

	robotPose.translation.x = filter.x();
	robotPose.translation.y = filter.y();
	robotPose.rotation = filter.t();


	list<LineSample> *detlines = _LineDetector->getLines();
	list<LineSample>::iterator itdet;

	pthread_mutex_lock(&mutex);

	linesdbg.clear();


	for(list<LineSample>::iterator itdet = detlines->begin(); itdet != detlines->end(); ++itdet)
	{
		HPoint3D ps3D, pe3D;

		ps3D = (*itdet).ps3D;
		pe3D = (*itdet).pe3D;

		wmLine *line = getMatchingWorldLine((*itdet), robotPose);

		linesdbg.push_back(_WorldModel->getPoint((*line).p1)->p);
		linesdbg.push_back(_WorldModel->getPoint((*line).p2)->p);

		Pose2D idealRobotPose;

		idealRobotPose = getIdealPose(robotPose, (*itdet), line);

		posdbg.estimada = robotPose;
		posdbg.instantanea = idealRobotPose;

		MatrixCM R;
		HPoint3D z;

		z.X = idealRobotPose.translation.x;
		z.Y = idealRobotPose.translation.y;
		z.Z = idealRobotPose.rotation;

		R.identity(3);

		double stdvx =  fabs(_WorldModel->getPoint((*line).p1)->p.X - _WorldModel->getPoint((*line).p2)->p.X);
		double stdvy =  fabs(_WorldModel->getPoint((*line).p1)->p.Y - _WorldModel->getPoint((*line).p2)->p.Y);

		if(stdvx < 20.0) stdvx = 20.0;
		if(stdvy < 20.0) stdvy = 20.0;

		R.sete(0,0,stdvx*stdvx);
		R.sete(1,1,stdvy*stdvy);

		R.sete(2,2, 0.5 * 0.5);

		filter.correct(z, R);

		//posdbg.push_back(poses);

		/*Vector2<double> ps3D2, pe3D2;

		ps3D2 = Pose2D::relative2FieldCoord(robotPose, (*itdet).ps3D.X/(*itdet).ps3D.H, (*itdet).ps3D.Y/(*itdet).ps3D.H);
		pe3D2 = Pose2D::relative2FieldCoord(robotPose, (*itdet).pe3D.X/(*itdet).pe3D.H, (*itdet).pe3D.Y/(*itdet).pe3D.H);

		Vector2<double> p1w0, p2w0;
		Vector2<double> p1w1, p2w1;

		p1w0.x = _WorldModel->getPoint((*line).p1)->p.X;
		p1w0.y = _WorldModel->getPoint((*line).p1)->p.Y;
		p2w0.x = _WorldModel->getPoint((*line).p2)->p.X;
		p2w0.y = _WorldModel->getPoint((*line).p2)->p.Y;


		cerr<<"Line detected: ("<<ps3D2.x<<","<<ps3D2.y<<")-("<<pe3D2.x<<","<<pe3D2.y<<") matchs to ("<<p1w0.x<<","<<p1w0.y<<")-("<<p2w0.x<<","<<p2w0.y<<")"<<endl;*/
	}
	pthread_mutex_unlock(&mutex);

}

float EKFLocalization::getGoalPostDistanceStdDev(HPoint2D & rel)
{
	float length = sqrt(rel.x*rel.x+rel.y*rel.y);

	return GOAL_POST_DISTANCE_MIN_STD_DEV + (length / GOAL_POST_MAX_STD_DEV_DISTANCE) *
			(GOAL_POST_DISTANCE_MAX_STD_DEV - GOAL_POST_DISTANCE_MIN_STD_DEV);
}

float EKFLocalization::getGoalPostAngleStdDev(HPoint2D & rel)
{
	float length = sqrt(rel.x*rel.x+rel.y*rel.y);

	return GOAL_POST_ANGLE_MIN_STD_DEV + (length / GOAL_POST_MAX_STD_DEV_DISTANCE) *
			(GOAL_POST_ANGLE_MAX_STD_DEV - GOAL_POST_ANGLE_MIN_STD_DEV);
}

void EKFLocalization::getEqLine(Vector2<double> p, Vector2<double> vd, double &A, double &B, double &C)
{
	double c;

	c = -vd.y*p.x + vd.x*p.y;

	if(c!=0.0)
	{
		A = vd.y / c;
		B = -vd.x / c;
		C = 1.0;
	}else
	{
		A = vd.y;
		B = -vd.x;
		C = 0.0;
	}
}

bool EKFLocalization::getInterseccLines (double A, double B, double C, double D, double E, double F, Vector2<double> &i)
{
	if(A!=0.0)
	{
		i.y = (((D*C)/A)- F )/(((-D*B)/A) + E);
		i.x = ((-B*i.y)-C)/A;
	}else
		if(D!=0.0)
		{
			i.y = (((A*F)/D)- C )/(((-E*A)/D) + B);
			i.x = ((-E*i.y)-F)/D;
		}else
			return false;

	return true;
}

float EKFLocalization::getDistance(Vector2<double> p1, Vector2<double> p2)
{
	return sqrt((p1.x-p2.x)*(p1.x-p2.x) + (p1.y-p2.y)*(p1.y-p2.y));
}

bool EKFLocalization::inSegment(HPoint3D p1, HPoint3D p2, Vector2<double> pt)
{
	double dp1p2 = sqrt((p1.X-p2.X)*(p1.X-p2.X)+(p1.Y-p2.Y)*(p1.Y-p2.Y));

	double dp1pt = sqrt((p1.X-pt.x)*(p1.X-pt.x)+(p1.Y-pt.y)*(p1.Y-pt.y));
	double dp2pt = sqrt((p2.X-pt.x)*(p2.X-pt.x)+(p2.Y-pt.y)*(p2.Y-pt.y));

	return ((dp1pt<=dp1p2) && (dp2pt<=dp1p2));

}

float EKFLocalization::getDistance(wmLine* wmline, Vector2<double> ps3D, Vector2<double> pe3D)
{
	double lenght = sqrt(((ps3D.x-pe3D.x)*(ps3D.x-pe3D.x)) + ((ps3D.y-pe3D.y)*(ps3D.y-pe3D.y)));

	int divs = 10;
	Vector2<double> vd, vod;
	vd.x = (ps3D.x-pe3D.x)/(double)divs;
	vd.y = (ps3D.y-pe3D.y)/(double)divs;
	vod.x = -vd.y;
	vod.y = vd.x;

	float min = 10000.0;

	Vector2<double> pw1, pw2;

	pw1.x = _WorldModel->getPoint(wmline->p1)->p.X;
	pw1.y = _WorldModel->getPoint(wmline->p1)->p.Y;
	pw2.x = _WorldModel->getPoint(wmline->p2)->p.X;
	pw2.y = _WorldModel->getPoint(wmline->p2)->p.Y;

	float aux;
	aux = sqrt((pw1.x-ps3D.x)*(pw1.x-ps3D.x)+(pw1.y-ps3D.y)*(pw1.y-ps3D.y));
	if(aux<min) min=aux;
	aux = sqrt((pw2.x-ps3D.x)*(pw2.x-ps3D.x)+(pw2.y-ps3D.y)*(pw2.y-ps3D.y));
	if(aux<min) min=aux;
	aux = sqrt((pw1.x-pe3D.x)*(pw1.x-pe3D.x)+(pw1.y-pe3D.y)*(pw1.y-pe3D.y));
	if(aux<min) min=aux;
	aux = sqrt((pw2.x-pe3D.x)*(pw2.x-pe3D.x)+(pw2.y-pe3D.y)*(pw2.y-pe3D.y));
	if(aux<min) min=aux;

	float s = 0.0;
	for(int i=0; i<divs; i++)
	{
		float dist;
		Vector2<double> pd;

		//		s=s+(lenght/(float)divs);

		pd.x = pe3D.x + (double)i * vd.x;
		pd.y = pe3D.y + (double)i * vd.y;
		//
		double A,B,C;
		getEqLine(pd, vod, A, B, C);

		Vector2<double> iwm;
		if(getInterseccLines(A, B, C, wmline->A, wmline->B, wmline->C, iwm))
		{
			//cerr<<"\t("<<iwm.x<<","<<iwm.y<<") -> ("<<pd.x<<","<<pd.y<<")  ["<<iwm.x<<","<<iwm.y<<"] ";

			if((fabs(iwm.x)>10000.0) || (fabs(iwm.y)>10000.0))
				continue;
			//cerr<<" = ";
			//cerr<<"i = ("<<iwm.x<<","<<iwm.y<<")"<<endl;
			HPoint3D ps3DH, pe3DH;

			ps3DH.X = ps3D.x;
			ps3DH.Y = ps3D.y;
			pe3DH.X = pe3D.x;
			pe3DH.Y = pe3D.y;

			if(inSegment(_WorldModel->getPoint(wmline->p1)->p, _WorldModel->getPoint(wmline->p2)->p, iwm))
			{
				dist = getDistance(iwm, pd);
				//cerr<<dist<<endl;
			}
			else
				continue;

			if((dist<min) && (dist>1.0))
				min=dist;
		}
	}

	//cerr<<"("<<_WorldModel->getPoint(wmline->p1)->p.X<<","<<_WorldModel->getPoint(wmline->p1)->p.Y<<") -> ";
	//cerr<<"("<<_WorldModel->getPoint(wmline->p2)->p.X<<","<<_WorldModel->getPoint(wmline->p2)->p.Y<<") = "<<min<<endl;


	return min;

}


/*float EKFLocalization::getDistance(wmLine* wmline, Vector2<double> ps3D, Vector2<double> pe3D)
{
	//Divividir la línea detectada en 10 puntos y quedarse con la distancia más pequeña

	float divs = 10.0;
	float min = 1000.0;

	Vector2<double> vd;
	double lenght = sqrt(((ps3D.x-pe3D.x)*(ps3D.x-pe3D.x)) + ((ps3D.y-pe3D.y)*(ps3D.y-pe3D.y)));

	vd.x = (ps3D.x-pe3D.x)/lenght;
	vd.y = (ps3D.y-pe3D.y)/lenght;

	Vector2<double> vwm;

	vwm.x = -wmline->B/wmline->C;
	vwm.y = wmline->A/wmline->C;

	float s = 0.0;
	for(int i=0; i<10; i++)//float s=0.0; s<lenght; s=s+(lenght/divs))
	{
		Vector2<double> p;
		Vector2<double> i;
		double dist;
		double A,B,C;

		s=s+(lenght/divs);

		p.x = pe3D.x + s * vd.x;
		p.y = pe3D.y + s * vd.y;



		getEqLine(p, vwm, A, B, C);


		getInterseccLines(A, B, C, wmline->A, wmline->B, wmline->C, i);
		cerr<<"interseccion: ["<<A<<","<<B<<","<<C<<"] ["<<wmline->A<<","<<wmline->B<<","<<wmline->C<<"] = ("<<i.x<<","<<i.y<<")"<<endl;

		cerr<<"lenght = "<<lenght<<endl;
		cerr<<"distance ps3D, i ("<<ps3D.x<<","<<ps3D.y<<") - ("<<i.x<<","<<i.y<<") = "<<getDistance(ps3D,i)<<endl;
		cerr<<"distance pe3D, i ("<<pe3D.x<<","<<pe3D.y<<") - ("<<i.x<<","<<i.y<<") = "<<getDistance(ps3D,i)<<endl;
		cerr<<"distance ps3D, p ("<<ps3D.x<<","<<ps3D.y<<") - ("<<p.x<<","<<p.y<<") = "<<getDistance(ps3D,i)<<endl;
		cerr<<"distance pe3D, p ("<<pe3D.x<<","<<pe3D.y<<") - ("<<p.x<<","<<p.y<<") = "<<getDistance(ps3D,i)<<endl;
		cerr<<"distance p, i ("<<p.x<<","<<p.y<<") - ("<<i.x<<","<<i.y<<") = "<<getDistance(ps3D,i)<<endl;

		if((getDistance(ps3D,i)>lenght) || (getDistance(pe3D,i)>lenght))
		{
			double daux;
			dist = getDistance(ps3D,p);
			daux = getDistance(pe3D,p);

			if(dist>daux) dist = daux;
		}else
		{
			dist = getDistance(p,i);
		}

		if(dist<min)
			min=dist;
	}

	return min;

}*/



float EKFLocalization::getDiffLong(wmLine* wmline, Vector2<double> ps3D, Vector2<double> pe3D)
{

	double zlenght = sqrt(((ps3D.x-pe3D.x)*(ps3D.x-pe3D.x)) + ((ps3D.y-pe3D.y)*(ps3D.y-pe3D.y)));
	float diff;

	diff = zlenght - wmline->lenght;

	if(diff<0.0)
		diff = 0.0;
	else
		if(diff>1000.0)
			diff = 1000.0;

	return diff;

}
wmLine *EKFLocalization::getMatchingWorldLine(LineSample detline, Pose2D &robotPose)
{
	Vector2<double> ps3D, pe3D;
	HPoint3D p1, p2;


	ps3D = Pose2D::relative2FieldCoord(robotPose, detline.ps3D.X/detline.ps3D.H, detline.ps3D.Y/detline.ps3D.H);
	pe3D = Pose2D::relative2FieldCoord(robotPose, detline.pe3D.X/detline.pe3D.H, detline.pe3D.Y/detline.pe3D.H);

	float detangle;

	detangle = atan2(ps3D.y-pe3D.y, ps3D.x-pe3D.x);

	list<LineCandidate> candidates;
	candidates.clear();

	list<wmLine*> *wmlines = _WorldModel->getLines();

	//cerr<<"{("<<ps3D.x<<","<<ps3D.y<<") - ("<<pe3D.x<<","<<pe3D.y<<")}"<<endl;

	list<wmLine*>::iterator itlwm;
	for(list<wmLine*>::iterator itlwm = wmlines->begin(); itlwm != wmlines->end(); ++itlwm)
	{

		float f1, f2, f3;
		float dif1, dif2;

		f1 = 0.4;
		f2 = 0.4;
		f3 = 0.2;

		LineCandidate tcand;

		tcand.line = (*itlwm);
		tcand.points = 0.0;

		dif1 = fabs(normalizePi(detangle - (*itlwm)->angle));
		dif2 = fabs(normalizePi(-detangle - (*itlwm)->angle));

		if(dif1<dif2)
		{
			if(dif1>(M_PI/4.0))
				continue;
			tcand.points = tcand.points +  (fabs(M_PI-dif1)/(M_PI)) * f1;
		}
		else
		{
			if(dif2>(M_PI/4.0))
				continue;
			tcand.points = tcand.points +  (fabs(M_PI-dif2)/(M_PI)) * f1;
		}



		float dist = getDistance((*itlwm), ps3D, pe3D);

		tcand.points = tcand.points + ((3000.0-dist)/3000.0) * f2;

		float diffLong = getDiffLong((*itlwm), ps3D, pe3D);

		tcand.points = tcand.points + ((1000.0-diffLong)/1000.0) * f3;

		p1 = _WorldModel->getPoint(tcand.line->p1)->p;
		p2 = _WorldModel->getPoint(tcand.line->p2)->p;

		candidates.push_back(tcand);

	}

	wmLine* winner=NULL;
	float maxpoint = 0.0;
	for(list<LineCandidate>::iterator itc = candidates.begin(); itc != candidates.end(); ++itc)
	{

		p1 = _WorldModel->getPoint((*itc).line->p1)->p;
		p2 = _WorldModel->getPoint((*itc).line->p2)->p;

		if(maxpoint<(*itc).points)
		{
			maxpoint = (*itc).points;
			winner = (*itc).line;
		}
	}

	if(winner != NULL)
	{
		p1 = _WorldModel->getPoint(winner->p1)->p;
		p2 = _WorldModel->getPoint(winner->p2)->p;
	}


	return winner;
	//		//mirar a ver su ángulo
	//		float dif1, dif2;
	//		float A, B, C;
	//		float x1, y1, x2, y2;
	//		float dist, dist1, dist2;
	//		float xmedz_, ymedz_;
	//
	//		x1 = _WorldModel->getPoint((*itlwm)->p1)->p.X;
	//		y1 = _WorldModel->getPoint((*itlwm)->p1)->p.Y;
	//		x2 = _WorldModel->getPoint((*itlwm)->p2)->p.X;
	//		y2 = _WorldModel->getPoint((*itlwm)->p2)->p.Y;
	//
	//		A = (y2-y1)/(x2-x1);
	//		B = -1.0;
	//		C = y1 - x1 * K;
	//
	//		xmedz_ = (p1z_->x + p2z_->x) / 2.0;
	//		ymedz_ = (p1z_->y + p2z_->y) / 2.0;
	//
	//		dist = fabs(A*xmedz_+B*ymedz_+C)/sqrt(A*A+B*B);
	//		dist1 = sqrt((x1 - xmedz_)*(x1 - xmedz_)+(y1 - ymedz_)*(y1 - ymedz_));
	//		dist2 = sqrt((x2 - xmedz_)*(x2 - xmedz_)+(y2 - ymedz_)*(y2 - ymedz_));
	//
	//		bool cond1, cond2, cond3;
	//
	//		cond1 = (dif1<toRadians(30.0)) || (dif2<toRadians(30.0));
	//		cond2 = dist < 500.0;
	//		cond3 = (dist1 < (*itlwm)->lenght) && (dist2 < (*itlwm)->lenght);





}


bica::ShapeList
EKFLocalization::getGrDebugRel()
{


	pthread_mutex_lock(&mutex);

	shapeListRel.clear();

	list<HPoint3D>::iterator itdet;

	for(list<HPoint3D>::iterator itdet = linesdbg.begin(); itdet != linesdbg.end(); ++itdet)
	{

		//Se pinta la que se ve
		bica::Point3DPtr p1z_(new bica::Point3D);
		bica::Point3DPtr p2z_(new bica::Point3D);
		bica::LinePtr linez_(new bica::Line);

		HPoint3D ps3D, pe3D;

		ps3D = (*itdet);
		++itdet;
		pe3D = (*itdet);



		p1z_->x=ps3D.X/ps3D.H;
		p1z_->y=ps3D.Y/ps3D.H;
		p1z_->z=ps3D.Z/ps3D.H + 10.0;
		p2z_->x=pe3D.X/pe3D.H;
		p2z_->y=pe3D.Y/pe3D.H;
		p2z_->z=pe3D.Z/pe3D.H + 10.0;

		//cerr<<"["<<p1z_->x<<","<<p1z_->y<<"] -> ["<<p2z_->x<<","<<p2z_->y<<"]"<<endl;


		linez_->p1 = p1z_;
		linez_->p2 = p2z_;
		linez_->color = bica::RED;
		linez_->filled = true;
		linez_->opacity = 255.0;
		linez_->accKey = "z";
		linez_->label = "Ẑ";
		shapeListRel.push_back(linez_);
	}
	/*
	Pose2D robotPose;

	EKF *pos=getBestEKF();

	robotPose.translation.x = pos->x();
	robotPose.translation.y = pos->y();
	robotPose.rotation = pos->t();

	list<LineSample> *detlines = _LineDetector->getLines();
	shapeListRel.clear();

	list<LineSample>::iterator itdet;
	pthread_mutex_lock(&mutex);
	for(list<LineSample>::iterator itdet = detlines->begin(); itdet != detlines->end(); ++itdet)
	{

		//Se pinta la que se ve
		bica::Point3DPtr p1z_(new bica::Point3D);
		bica::Point3DPtr p2z_(new bica::Point3D);
		bica::LinePtr linez_(new bica::Line);

		HPoint3D ps3D, pe3D;

		ps3D = (*itdet).ps3D;
		pe3D = (*itdet).pe3D;


		p1z_->x=ps3D.X/ps3D.H;
		p1z_->y=ps3D.Y/ps3D.H;
		p1z_->z=ps3D.Z/ps3D.H;
		p2z_->x=pe3D.X/pe3D.H;
		p2z_->y=pe3D.Y/pe3D.H;
		p2z_->z=pe3D.Z/pe3D.H;

		linez_->p1 = p1z_;
		linez_->p2 = p2z_;
		linez_->color = bica::RED;
		linez_->filled = true;
		linez_->opacity = 255.0;
		linez_->accKey = "z";
		linez_->label = "Ẑ";
		shapeListRel.push_back(linez_);

		/*wmLine *line = getMatchingWorldLine((*itdet), robotPose);

		//Se pinta la que se ve
		bica::Point3DPtr p1w_(new bica::Point3D);
		bica::Point3DPtr p2w_(new bica::Point3D);
		bica::LinePtr linew_(new bica::Line);

		Vector2<double> p1w0, p2w0;
		Vector2<double> p1w1, p2w1;

		p1w0.x = _WorldModel->getPoint((*line).p1)->p.X;
		p1w0.y = _WorldModel->getPoint((*line).p1)->p.Y;
		p2w0.x = _WorldModel->getPoint((*line).p2)->p.X;
		p2w0.y = _WorldModel->getPoint((*line).p2)->p.Y;

		p1w1 = Pose2D::fieldCoord2Relative(robotPose, p1w0);
		p2w1 = Pose2D::fieldCoord2Relative(robotPose, p2w0);

		p1w_->x=p1w1.x;
		p1w_->y=p1w1.y;
		p1w_->z=0.0;
		p2w_->x=p2w1.x;
		p2w_->y=p2w1.y;
		p2w_->z=0.0;

		linew_->p1 = p1w_;
		linew_->p2 = p2w_;
		linew_->color = bica::BLUE;
		linew_->filled = true;
		linew_->opacity = 255.0;
		linew_->accKey = "w";
		linew_->label = "h(Z)";

		shapeListRel.push_back(linew_);
	}*/

	pthread_mutex_unlock(&mutex);

	return shapeListRel;
}

bica::ShapeList
EKFLocalization::getGrDebugAbs()
{

	shapeListAbs.clear();
	pthread_mutex_lock(&mutex);

{
		Point2D p2d, p22;

		p2d.x = (float)ekf.filter->x();
		p2d.y = (float)ekf.filter->y();

		p22.x = (float)ekf.filter->x() + 600.0 * cos(ekf.filter->t());
		p22.y = (float)ekf.filter->y() + 600.0 * sin(ekf.filter->t());

				bica::Point3DPtr src(new bica::Point3D);
			bica::Point3DPtr dst(new bica::Point3D);
			bica::ArrowPtr a(new bica::Arrow);


			//cerr<<"---> ("<<(*it)->x()<<", "<<(*it)->y()<<", "<<(*it)->t()<<")"<<endl;
			src->x = (float)ekf.filter->x();
			src->y = (float)ekf.filter->y();
			src->z = 600.0;
			dst->x = (float)ekf.filter->x() + 600.0 * cos(ekf.filter->t());
			dst->y = (float)ekf.filter->y() + 600.0 * sin(ekf.filter->t());
			dst->z = 600.0;

			a->src = src;
			a->dst = dst;
			a->width = 10.0;
			a->color = bica::BLUE;
			a->filled = true;
			a->opacity = 255;
			a->accKey = "c";
			a->label = "KFE";
			shapeListAbs.push_back(a);

		bica::EllipsePtr e(new bica::Ellipse);
		bica::Point3DPtr c(new bica::Point3D);

		c->x = (float)ekf.filter->x();
		c->y = (float)ekf.filter->y();
		c->z = 550;

		e->center = c;

		GaussianDistribution2D distrib;

		distrib.mean[0] = ekf.filter->x();
		distrib.mean[1] = ekf.filter->y();
		distrib.covariance[0][0] = ekf.filter->get_P()->e(0, 0);
		distrib.covariance[0][1] = ekf.filter->get_P()->e(0, 1);
		distrib.covariance[1][0] = ekf.filter->get_P()->e(1, 0);
		distrib.covariance[1][1] = ekf.filter->get_P()->e(1, 1);

		double va1, va2;
		Vector2D_BH<double> ve1, ve2;
		double x,y;

		distrib.getEigenVectorsAndEigenValues(ve1, ve2, va1, va2);

		double angle;
		angle = atan2(ve1[1], ve1[0]);

		if(ekf.filter->get_P()->e(0, 0)<ekf.filter->get_P()->e(1, 1))
			angle = normalizePi(angle+pi_2);

		e->width = (float)sqrt(va1);
		e->length = (float)sqrt(va2);
		e->angle = (float)angle;
		e->color = bica::WHITE;
		e->filled = true;
		e->opacity = 125;
		e->accKey = "c";
		e->label = "KFEE";
		shapeListAbs.push_back(e);

	}

	//cerr<<endl;

	for(int i=0; i<mgrid->getRows(); i++)
		for(int j=0; j<mgrid->getColumns(); j++)
		{
			bica::RectanglePtr a(new bica::Rectangle);
			bica::Point3DPtr p2d(new bica::Point3D);
			bica::Point3DPtr p22(new bica::Point3D);

			p2d->x=(i*CELL_SIZE)-3000.0;
			p2d->y=(j*CELL_SIZE)-2000.0;
			p2d->z=500.0;

			p22->x = ((i+1)*CELL_SIZE)-3000.0;
			p22->y = ((j+1)*CELL_SIZE)-2000.0;
			p22->z=500.0;


			float media;

			media = 1.0/((float)mgrid->getRows()*(float)mgrid->getColumns());

			float value = (((mgrid->getProbIJ(i,j)/media)*255.0))/2.0;

			float p = mgrid->getProbIJ(i,j);

			if (p>0.01)
				value = (p/0.02)*255.0;
			else
				value = 0.0;
			//value = mgrid->getProbIJ(i,j);

			if (value>255.0f)
				value = 255.0f;
			//cerr<<"("<<p2d->x<<", "<<p2d->y<<"," <<value<<") ";


			a->p1 = p2d;
			a->p2 = p22;
			a->color = bica::RED;
			a->filled = true;
			a->opacity = value;
			a->accKey = "c";
			a->label = "KF";
			shapeListAbs.push_back(a);


			bica::Point3DPtr src(new bica::Point3D);
			bica::Point3DPtr dst(new bica::Point3D);
			bica::ArrowPtr r(new bica::Arrow);

			float cx, cy;

			cx = (((((float)i)*CELL_SIZE)-3000.0) + ((((float)(i+1))*CELL_SIZE)-3000.0))/2.0;
			cy = (((((float)j)*CELL_SIZE)-2000.0) + ((((float)(j+1))*CELL_SIZE)-2000.0))/2.0;



			src->x = cx;
			src->y = cy;
			src->z = 600.0;
			dst->x = cx + (CELL_SIZE/2.0) * cos(mgrid->getThetaIJ(i, j));
			dst->y = cy + (CELL_SIZE/2.0) * sin(mgrid->getThetaIJ(i, j));
			dst->z = 600.0;

			r->src = src;
			r->dst = dst;
			r->width = 10.0;
			r->color = bica::BLUE;
			r->filled = true;
			r->opacity = 255;
			r->accKey = "c";
			r->label = "KFE";
			shapeListAbs.push_back(r);


		}
	//cerr<<endl;

	pthread_mutex_unlock(&mutex);
	return shapeListAbs;
}

void
EKFLocalization::setPos( float x, float y, float t, const Ice::Current& c )
{
	//cerr<<"recv ["<<x<<","<<y<<","<<toDegrees(t*M_PI)<<"]"<<endl;

	dbgx = x;
	dbgy = y;
	dbgt = t*M_PI;

	MatrixCM s(3,1), P(3,3);

	s.sete(0,0,dbgx);
	s.sete(1,0,dbgy);
	s.sete(2,0,dbgt);

	P.identity(3);
	P.sete(0,0, 1000.0*1000.0);
	P.sete(1,1, 1000.0*1000.0);
	P.sete(2,2, 10.0*10.0);
	ekf.filter->restart(s,P);


	_GoalDetector->step();
	_LineDetector->step();

	correct();
}


