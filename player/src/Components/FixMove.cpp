#include "FixMove.h"
#include "Body.h"
#include "almath.h"
#include <unistd.h>
#include "Striker.h"
#include "FaceBall.h"

FixMove::FixMove()
{

	state = STOPPING;

	movs.clear();

	ifstream infile;
	string line;

	infile.open ("/var/lib/chaos/movs/movs.index", ifstream::in);

	getline(infile, line);
	while (infile.good())
	{
		if(line.length()>0)
		{
			loadMove(line+string(".mov"));
		}
		getline(infile, line);
	}

	infile.close();

	state = STOPPING;
	completed = 0;
	desp_side = 0.0;
	desp_walk = 0.0;
	desp_turn = 0.0;


}

FixMove::~FixMove()
{

}

void
FixMove::init(const string newName, AL::ALPtr<AL::ALBroker> parentBroker)
{
	Component::init(newName, parentBroker);

	try{
		pmotion = getParentBroker()->getMotionProxy();
	}catch( ALError& e)
	{
		cerr<<"FixMove::init : "<<e.toString()<<endl;
	}
}

void
FixMove::step(void)
{
	if (!isTime2Run())
		return;
	//ostringstream s;
	int new_state;
	struct timeval now;
	static long duration = 0;
	long elapsed;
	tmov move;

	switch(state)
	{
	case STOPPING:
		//s<<"FixMove::STOPPING"<<endl;
		if(!pmotion->walkIsActive())
		{
			new_state = INIT_ALIGN;
		}else
		{
			pmotion->clearFootsteps();
			new_state = STOPPING;
		}
		break;
	case INIT_ALIGN:
		//s<<"FixMove::INIT_ALIGN"<<endl;
		//if(fabs(desp_side) >= 0.02)
		//{
			//cerr<<"SIDE: "<<desp_side<<endl;
			pmotion->walkSideways(desp_side, Body::NSPS+10);
		//}
		//if(fabs(desp_walk) >= 0.02)
		//{
			//cerr<<"WALK: "<<desp_walk<<endl;
			pmotion->walkStraight(desp_walk, Body::NSPS+10);
		//}


		/*if(fabs(desp_turn) > 0.0)
		{
			//pmotion->addWalkSideways(-0.05, Body::NSPS);
			//cerr<<"TURN: "<<(desp_turn*180.0/3.1415)<<endl;
			pmotion->addTurn(desp_turn, Body::NSPS);
			if(desp_turn<0.0)
				pmotion->addWalkSideways(0.02, Body::NSPS+10);
			else
				pmotion->addWalkSideways(-0.02, Body::NSPS+10);
		}
*/
		//if((fabs(desp_walk) >=0.2) || (fabs(desp_side) >=0.2))

		//pmotion->post.walk();
		//usleep(10000);

		new_state = ALIGNINIG;
		break;
	case ALIGNINIG:

		if(!pmotion->walkIsActive())
		{
			initPos = pmotion->getBodyAngles();
			pmotion->post.setBalanceMode(0);
			new_state = INIT_MOVE;
		}
		else
		{
			//cerr<<"ALIGNINIG"<<endl;
			new_state = ALIGNINIG;
		}
		break;

	case INIT_MOVE:
		move = movs[movement];

		pmotion->post.doMove(move.joints, move. angles, move.times, 0);

		{
			ALValue times = move.times[0];
			int idx = times.getSize() - 1;
			float maxtime = (float)(times[idx]);
			duration = maxtime*1000;
		}

		gettimeofday(&start, NULL);

		new_state = MOVING;

		break;

	case MOVING:
		gettimeofday(&now, NULL);

		elapsed = (now.tv_sec - start.tv_sec) * 1000000 + (now.tv_usec - start.tv_usec);

		if((elapsed/1000) > duration)
			new_state = FINISHED;
		else
			new_state = MOVING;
		break;

	case FINISHED:
		//s<<"FixMove::FINISHED"<<endl;
		//cerr<<"FINISHED"<<endl;
		//cerr<<"FIX: FINISHED"<<endl;
		pmotion->post.setBalanceMode(1);
		completed = 1.0;
		new_state = FINISHED;
		break;
	};

	//Striker::getInstance()->writeLog(s.str());

	state = new_state;
}


void
FixMove::loadMove(string MoveFile)
{
	ifstream infile;
	string line;
	char MoveFilename[64];
	int num_joints, num_line = 1;
	int *num_angles, *num_times;

	tmov *aux = new tmov;

	infile.open (string(("/var/lib/chaos/movs/") + MoveFile).c_str(), ifstream::in);

	getline(infile, line);

	sscanf(line.c_str(), "%s", MoveFilename);

	//Steps
	//cerr<<"Loading Move File: "<<MoveFilename<<endl;
	//Joints
	//cerr<<"Joints: ";
	if(infile.good())
		getline(infile, line);
	TokenizeS(line, aux->joints);

	num_joints = aux->joints.getSize();
	num_line++;

	num_angles = new int[num_joints];
	num_times = new int[num_joints];

	//cerr<<" ["<< aux->joints.getSize()<<"]"<<endl;
	//Angles
	for(int i=0; i<aux->joints.getSize(); i++)
	{
		//cerr<<"angle "<<i<<": ";
		ALValue *auxal = new ALValue;
		auxal->clear();
		if(infile.good())
			getline(infile, line);
		Tokenize(line, *auxal);
		//cerr<<endl;
		aux->angles.arrayPush(*auxal);
		num_angles[i] = auxal->getSize();
		num_line++;
	}

	if(aux->angles.getSize() != num_joints)
		cerr<<"Error in movement: "<<MoveFilename<<" line: "<<num_line
		<<" num angles ("<<aux->angles.getSize()<<") != joints("<<num_joints<<")"<<endl;


	//Angles
	for(int i=0; i<aux->joints.getSize(); i++)
	{
		//cerr<<"angle "<<i<<": ";
		ALValue *auxal = new ALValue;
		auxal->clear();
		if(infile.good())
			getline(infile, line);
		Tokenize(line, *auxal);
		//cerr<<endl;
		aux->times.arrayPush(*auxal);
		num_times[i] = auxal->getSize();

		if(num_times[i] != num_angles[i] )
			cerr<<"Error in movement: "<<MoveFilename<<" line: "<<num_line
			<<" num times ("<<num_times[i]<<") != num angles("<<num_angles[i]<<")"<<endl;

		num_line++;
	}

	if(aux->times.getSize() != num_joints)
		cerr<<"Error in movement: "<<MoveFilename<<" line: "<<num_line
		<<" num times ("<<aux->times.getSize()<<") != joints"<<num_joints<<")"<<endl;


	delete [] num_times;
	delete [] num_angles;

	movs[string(MoveFilename)] = *aux;

	infile.close();


}

void
FixMove::setPreSide(float desp_side)
{
	this->desp_side = desp_side;
}

void
FixMove::setPreWalk(float desp_walk)
{
	this->desp_walk = desp_walk;
}

void
FixMove::setPreTurn(float desp_turn)
{
	this->desp_turn = desp_turn;
}


void
FixMove::TokenizeS(const string& str, ALValue& tokens, const string& delimiters)
{
	// Skip delimiters at beginning.
	string::size_type lastPos = str.find_first_not_of(delimiters, 0);
	// Find first "non-delimiter".
	string::size_type pos     = str.find_first_of(delimiters, lastPos);

	while (string::npos != pos || string::npos != lastPos)
	{
		// Found a token, add it to the vector.
		tokens.arrayPush(str.substr(lastPos, pos - lastPos));
		//cerr<<" "<<str.substr(lastPos, pos - lastPos);
		// Skip delimiters.  Note the "not_of"
		lastPos = str.find_first_not_of(delimiters, pos);
		// Find next "non-delimiter"
		pos = str.find_first_of(delimiters, lastPos);
	}
}

void
FixMove::Tokenize(const string& str, ALValue& tokens, const string& delimiters)
{
	// Skip delimiters at beginning.
	string::size_type lastPos = str.find_first_not_of(delimiters, 0);
	// Find first "non-delimiter".
	string::size_type pos     = str.find_first_of(delimiters, lastPos);

	while (string::npos != pos || string::npos != lastPos)
	{
		// Found a token, add it to the vector.
		tokens.arrayPush(atof(str.substr(lastPos, pos - lastPos).c_str()));
		//cerr<<" "<<atof(str.substr(lastPos, pos - lastPos).c_str());
		// Skip delimiters.  Note the "not_of"
		lastPos = str.find_first_not_of(delimiters, pos);
		// Find next "non-delimiter"
		pos = str.find_first_of(delimiters, lastPos);
	}
}

list<string> Movie::getTokens(char *data) {
	list<string> ret;
	string msg = data;
	istringstream iss(msg);
	string token;

	while (getline(iss, token, ':'))
		ret.push_back(token);
	return ret;
}
