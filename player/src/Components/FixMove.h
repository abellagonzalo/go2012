#ifndef FixMove_H
#define FixMove_H

#include "Component.h"
#include "alptr.h"
#include "almotionproxy.h"
#include "Singleton.h"

#include <fstream>
#include <sstream>
#include <map>
#include <list>
#include <vector>

using namespace std;

typedef struct
{
	ALValue joints;
	ALValue angles;
	ALValue times;
}tmov;


class FixMove : public Component, public Singleton<FixMove>
{
public:

	FixMove();
	~FixMove();

	void step();
	void init(const string newName, AL::ALPtr<AL::ALBroker> parentBroker);
	inline void restart(){state = STOPPING; completed = 0; desp_side = 0.0; desp_walk = 0.0; desp_turn = 0.0;};
	inline void setMove(string move){movement = move;};
	inline float getCompleted(){return completed;};

	void setPreSide(float desp_side);
	void setPreWalk(float desp_walk);
	void setPreTurn(float desp_turn);
	
private:

	void loadMove(string);
	void Tokenize(const string&, ALValue&, const string& = " ");
	void TokenizeS(const string&, ALValue&, const string& = " ");

	//States for the state machine
	static const int STOPPING		=-1;
	static const int INIT_ALIGN		= 0;
	static const int ALIGNINIG		= 1;
	static const int INIT_MOVE		= 2;
	static const int MOVING			= 3;
	static const int FINISHED		= 4;


	map<string, tmov> movs;
	vector<float> initPos;
	string movement;
	AL::ALPtr<AL::ALMotionProxy> pmotion; //Proxy to stm. Used to insert or read a data in the stm module.
	bool active;
	struct timeval start;
	float completed;
	int state;
	FILE *fdlog;
	
	float desp_side;
	float desp_walk;
	float desp_turn;
};

#endif
