#ifndef Striker2_H
#define Striker2_H

#include "Component.h"
#include "BallDetector.h"
#include "WorldState.h"
#include "SearchBall.h"
#include "Attention.h"
#include "GoalDetector.h"
#include "ApproachBall.h"
#include "Go2pos.h"
#include "Body.h"
#include "Singleton.h"

//BUILDER COMMENT. DO NOT REMOVE. auxinclude begin
//BUILDER COMMENT. DO NOT REMOVE. auxinclude end

class Striker2 : public Component, public Singleton<Striker2>
{
public:

	Striker2();
	~Striker2();

	void step();
private:

	static const int Initial	= 0;
	static const int SearchingBall	= 1;
	static const int Going2Ball	= 2;
	static const int BallNear	= 3;
	static const int Shoot	= 4;
	int state;

	BallDetector *_BallDetector;
	WorldState *_WorldState;
	SearchBall *_SearchBall;
	Attention *_Attention;
	GoalDetector *_GoalDetector;
	ApproachBall *_ApproachBall;
	Go2pos *_Go2pos;
	Body *_Body;

	void Initial_state_code(void);
	void SearchingBall_state_code(void);
	void Going2Ball_state_code(void);
	void BallNear_state_code(void);
	void Shoot_state_code(void);
	bool Initial2SearchingBall0_transition_code(void);
	bool SearchingBall2Going2Ball0_transition_code(void);
	bool Going2Ball2SearchingBall0_transition_code(void);
	bool BallNear2Going2Ball0_transition_code(void);
	bool Shoot2BallNear0_transition_code(void);
	bool Going2Ball2BallNear0_transition_code(void);
	bool BallNear2Shoot0_transition_code(void);
	bool BallNear2SearchingBall0_transition_code(void);
//BUILDER COMMENT. DO NOT REMOVE. auxcode begin
public:
	void init(const string newName, AL::ALPtr<AL::ALBroker> parentBroker);
private:
	float goalMidAngle();
	bool condShoot(int kid);
	float maxx, minx, lminy, lmaxy, rminy, rmaxy;

	void updateGoalAngle();
	void updateKicks();

	float goalAngle;
	bool fr, fl, br, bl, tr, tl;

//BUILDER COMMENT. DO NOT REMOVE. auxcode end
};

#endif

