#ifndef FollowBall_H
#define FollowBall_H

#include "Component.h"
#include "BallDetector.h"
#include "Body.h"
#include "Singleton.h"

//BUILDER COMMENT. DO NOT REMOVE. auxinclude begin
//BUILDER COMMENT. DO NOT REMOVE. auxinclude end

class FollowBall : public Component, public Singleton<FollowBall>
{
public:

	FollowBall();
	~FollowBall();

	void step();
private:

	static const int Initial	= 0;
	static const int NotSeeingBall	= 1;
	static const int SeeingBall	= 2;
	int state;

	BallDetector *_BallDetector;
	Body *_Body;

	void Initial_state_code(void);
	void NotSeeingBall_state_code(void);
	void SeeingBall_state_code(void);
	bool BallInitiallyNotSeen_transition_code(void);
	bool BallLost_transition_code(void);
	bool BallFound_transition_code(void);
	bool BallInitiallySeen_transition_code(void);
//BUILDER COMMENT. DO NOT REMOVE. auxcode begin
public:
	void init(const string newName, AL::ALPtr<AL::ALBroker> parentBroker);

private:
	PIDController *ctrlW, *ctrlL, *ctrlV;

//BUILDER COMMENT. DO NOT REMOVE. auxcode end
};

#endif

