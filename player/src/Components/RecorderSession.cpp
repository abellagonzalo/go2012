#include "RecorderSession.h"

//BUILDER COMMENT. DO NOT REMOVE. auxinclude begin
//BUILDER COMMENT. DO NOT REMOVE. auxinclude end

RecorderSession::RecorderSession()
{
	_GoalDetector = GoalDetector::getInstance();
	_GTLocalization = GTLocalization::getInstance();
	state = Initial;
}

RecorderSession::~RecorderSession()
{

}

void RecorderSession::Initial_state_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Initial_state_code
//BUILDER COMMENT. DO NOT REMOVE. end Initial_state_code
}

void RecorderSession::Record_state_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Record_state_code

	if((getCurrentTime() - lastts)>5000000) //5 secs
	{
		//restart

		cerr<<"Restarting log"<<endl;
		fout = fopen("/tmp/log", "w");
		lastpos =  pmotion->getRobotPosition(USE_SENSOR);
		initts = getCurrentTime();
	}else
	{
		fout = fopen("/tmp/log", "a");
	}


	MotionInfo _motion;
	PerceptionInfo _perception;

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


	double xpos, ypos, tpos;
	_GTLocalization->getPosition(xpos, ypos, tpos);

	fprintf(fout, "%ld\t%f\t%f\t%f\t%f\t%f\t%f\t%ld\t%ld\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\n", (getCurrentTime() - initts)/1000,
			(float)xpos, (float)ypos, (float)tpos,
			_motion.movx, _motion.movy, _motion.movt,
			_perception.p0ElapsedTimeSinceLastObs, _perception.p1ElapsedTimeSinceLastObs,
			_perception.p0r.reliability, _perception.p0r.x, _perception.p0r.y,
			_perception.p0l.reliability, _perception.p0l.x, _perception.p0l.y,
			_perception.p1r.reliability, _perception.p1r.x, _perception.p1r.y,
			_perception.p1l.reliability, _perception.p1l.x, _perception.p1l.y);

	//cerr<<"["<<getCurrentTime()<<", "<<initts<<"]"<<endl;

	lastts = getCurrentTime();

	fclose(fout);
//BUILDER COMMENT. DO NOT REMOVE. end Record_state_code
}

bool RecorderSession::Initial2Record0_transition_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Initial2Record0_transition_code
return true;
//BUILDER COMMENT. DO NOT REMOVE. end Initial2Record0_transition_code
}

void
RecorderSession::step(void)
{
	switch (state)
	{
	case Initial:

		if (isTime2Run()) {
			Initial_state_code();

			if (Initial2Record0_transition_code()) {
				state = Record;
			}
		}

		break;
	case Record:
		_GoalDetector->step();
		_GTLocalization->step();

		if (isTime2Run()) {
			Record_state_code();

		}

		break;
	default:
		cout << "[RecorderSession::step()] Invalid state\n";
	}
}

//BUILDER COMMENT. DO NOT REMOVE. auxcode begin

void
RecorderSession::init(const string newName, AL::ALPtr<AL::ALBroker> parentBroker)
{
	Component::init(newName, parentBroker);
	setFreqTime(SHORT_RATE);

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
			cerr << "Recorder::NEKFLocalization [motion]" << e.toString() << endl;
		}
	initts = lastts = getCurrentTime();
}

//BUILDER COMMENT. DO NOT REMOVE. auxcode end

