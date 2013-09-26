#ifndef ApproachBall_H
#define ApproachBall_H

#include "Component.h"
#include "BallDetector.h"
#include "Go2pos.h"
#include "Singleton.h"

//BUILDER COMMENT. DO NOT REMOVE. auxinclude begin
//BUILDER COMMENT. DO NOT REMOVE. auxinclude end

class ApproachBall : public Component, public Singleton<ApproachBall>
{
public:

	ApproachBall();
	~ApproachBall();

	void step();
private:

	static const int Initial	= 0;
	static const int BalNear	= 1;
	static const int BallFar	= 2;
	int state;

	BallDetector *_BallDetector;
	Go2pos *_Go2pos;

	void Initial_state_code(void);
	void BalNear_state_code(void);
	void BallFar_state_code(void);
	bool BallFar22BalNear0_transition_code(void);
	bool BalNear2BallFar20_transition_code(void);
	bool Initial2BallFar20_transition_code(void);
//BUILDER COMMENT. DO NOT REMOVE. auxcode begin
public:
	void init(const string newName, AL::ALPtr<AL::ALBroker> parentBroker);

//BUILDER COMMENT. DO NOT REMOVE. auxcode end
};

#endif

