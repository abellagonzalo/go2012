#ifndef SearchBall_H
#define SearchBall_H

#include "Component.h"
#include "BallDetector.h"
#include "Body.h"
#include "Attention.h"
#include "Singleton.h"

//BUILDER COMMENT. DO NOT REMOVE. auxinclude begin
//BUILDER COMMENT. DO NOT REMOVE. auxinclude end

class SearchBall : public Component, public Singleton<SearchBall>
{
public:

	SearchBall();
	~SearchBall();

	void step();
private:

	static const int Initial	= 0;
	static const int RotateBody	= 1;
	static const int Stopped	= 2;
	int state;

	BallDetector *_BallDetector;
	Body *_Body;
	Attention *_Attention;

	void Initial_state_code(void);
	void RotateBody_state_code(void);
	void Stopped_state_code(void);
	bool Stopped2RotateBody0_transition_code(void);
	bool Initial2Stopped0_transition_code(void);
	bool RotateBody2Stopped0_transition_code(void);
//BUILDER COMMENT. DO NOT REMOVE. auxcode begin
public:
	void init(const string newName, AL::ALPtr<AL::ALBroker> parentBroker);

	void restartSearch();

private:
	float w;
//BUILDER COMMENT. DO NOT REMOVE. auxcode end
};

#endif

