#include "PlayerSession.h"

//BUILDER COMMENT. DO NOT REMOVE. auxinclude begin
//BUILDER COMMENT. DO NOT REMOVE. auxinclude end

PlayerSession::PlayerSession()
{
	_Localization = Localization::getInstance();
	state = Initial;

	fout = NULL;
	fsession = NULL;
	fin = NULL;
}

PlayerSession::~PlayerSession()
{

}

void PlayerSession::Initial_state_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Initial_state_code
//BUILDER COMMENT. DO NOT REMOVE. end Initial_state_code
}

void PlayerSession::Play_state_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Play_state_code
	MotionInfo _motion;
	PerceptionInfo _perception;
	float xpos, ypos, tpos;
	long time;
	vector<tRobotHipothesis> poses;

	float errorpos1, errorpos2, errorang1, errorang2;
	float errorpos, errorang;

	if((getCurrentTime() - lastts)>5000000) //5 secs
	{
		restartSession();
		return;
	}


	int ret;
	ret = fscanf(fin, "%ld\t%f\t%f\t%f\t%f\t%f\t%f\t%ld\t%ld\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\n", &time,
			&(xpos), &(ypos), &(tpos),
			&(_motion.movx), &(_motion.movy), &(_motion.movt),
			&(_perception.p0ElapsedTimeSinceLastObs), &(_perception.p1ElapsedTimeSinceLastObs),
			&(_perception.p0r.reliability), &(_perception.p0r.x), &(_perception.p0r.y),
			&(_perception.p0l.reliability), &(_perception.p0l.x), &(_perception.p0l.y),
			&(_perception.p1r.reliability), &(_perception.p1r.x), &(_perception.p1r.y),
			&(_perception.p1l.reliability), &(_perception.p1l.x), &(_perception.p1l.y));
	cerr<<"["<<ret<<"]"<<endl;

	if(ret == EOF)
	{
		cerr<<"EOF"<<endl;
		finishedSession = true;
		return;
	}

	_Localization->setInfo(_motion, _perception);
	_Localization->run();
	_Localization->getPosition(poses);

	errorpos1 = sqrt((xpos - poses[0].x)*(xpos - poses[0].x)+(ypos - poses[0].y)*(ypos - poses[0].y));
	errorpos2 = sqrt((xpos - poses[1].x)*(xpos - poses[1].x)+(ypos - poses[1].y)*(ypos - poses[1].y));

	errorang1 = fabs(normalizePi(poses[0].t - tpos));
	errorang2 = fabs(normalizePi(poses[1].t - tpos));

	int sel;

	if(errorpos1 > errorpos2)
	{
		errorpos = errorpos2;
		errorang = errorang2;
		sel = 0;
	}
	else
	{
		errorpos = errorpos1;
		errorang = errorang1;
		sel = 1;
	}

	if((errorpos1 <500.0) && (errorpos2 <500.0))
	{
		if(errorang1<errorang2)
		{
			errorpos = errorpos1;
			errorang = errorang1;
			sel = 0;
		}else
		{
			errorpos = errorpos2;
			errorang = errorang2;
			sel = 1;
		}

	}

	fprintf(fout, "%ld\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t\n", time, errorpos, errorang, poses[sel].x, poses[sel].y, poses[sel].t, xpos, ypos, tpos);

	lastts = getCurrentTime();
//BUILDER COMMENT. DO NOT REMOVE. end Play_state_code
}

void PlayerSession::PreSession_state_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin PreSession_state_code

	if(((getCurrentTime() - lastts)>5000000)|| (fsession==NULL)) //5 secs
		restartSession();

	if(fscanf( fsession, "%s %s", locali, foutname)!=EOF)
	{
		newSession = true;
		cerr<<"["<<locali<<"]"<<endl;

		_Localization->setLocalization(locali);

		startSessionTS = getCurrentTime();
		lastts = getCurrentTime();

	}else
	{
		cerr<<"No more sessions"<<endl;
	}

	lastts = getCurrentTime();


//BUILDER COMMENT. DO NOT REMOVE. end PreSession_state_code
}

bool PlayerSession::Initial2PreSession0_transition_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Initial2PreSession0_transition_code
return true;
//BUILDER COMMENT. DO NOT REMOVE. end Initial2PreSession0_transition_code
}

bool PlayerSession::PreSession2Play0_transition_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin PreSession2Play0_transition_code
if(newSession)
{
	newSession = false;
	finishedSession = false;

	fout = fopen(foutname, "w");
	fin = fopen("/tmp/log", "r");


	cerr<<"Starting new session"<<endl;

	return true;
}else
	return false;
//BUILDER COMMENT. DO NOT REMOVE. end PreSession2Play0_transition_code
}

bool PlayerSession::Play2PreSession0_transition_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Play2PreSession0_transition_code
// if session restarted OR finished session
	if(finishedSession)
	{
		fclose(fout);

		return true;
	}

	return false;

//BUILDER COMMENT. DO NOT REMOVE. end Play2PreSession0_transition_code
}

void
PlayerSession::step(void)
{
	switch (state)
	{
	case Initial:

		if (isTime2Run()) {
			Initial_state_code();

			if (Initial2PreSession0_transition_code()) {
				state = PreSession;
			}
		}

		break;
	case Play:

		if (isTime2Run()) {
			Play_state_code();

			if (Play2PreSession0_transition_code()) {
				state = PreSession;
			}
		}

		//_Localization->step();
		break;
	case PreSession:

		if (isTime2Run()) {
			PreSession_state_code();

			if (PreSession2Play0_transition_code()) {
				state = Play;
			}
		}

		break;
	default:
		cout << "[PlayerSession::step()] Invalid state\n";
	}
}

//BUILDER COMMENT. DO NOT REMOVE. auxcode begin

void
PlayerSession::init(const string newName, AL::ALPtr<AL::ALBroker> parentBroker)
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
void
PlayerSession::restartSession()
{
	newSession = false;
	finishedSession = true;

	if(fsession!=NULL) fclose(fsession);
	fsession = fopen("/tmp/logsession.in", "r");

}
//BUILDER COMMENT. DO NOT REMOVE. auxcode end

