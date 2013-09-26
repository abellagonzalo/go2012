#ifndef Defender_H
#define Defender_H

#include "Component.h"
#include "FaceBall.h"
#include "Body.h"
#include "BallDetector.h"
#include "SearchBall.h"
#include "Singleton.h"

//BUILDER COMMENT. DO NOT REMOVE. auxinclude begin
//BUILDER COMMENT. DO NOT REMOVE. auxinclude end

class Defender : public Component, public Singleton<Defender>
{
public:

	Defender();
	~Defender();

	void step();
private:

	static const int Initial	= 0;
	static const int StayInPos	= 1;
	static const int SearchingBall	= 2;
	int state;

	FaceBall *_FaceBall;
	Body *_Body;
	BallDetector *_BallDetector;
	SearchBall *_SearchBall;

	void Initial_state_code(void);
	void StayInPos_state_code(void);
	void SearchingBall_state_code(void);
	bool Initial2SearchingBall0_transition_code(void);
	bool SearchingBall2StayInPos0_transition_code(void);
	bool StayInPos2SearchingBall0_transition_code(void);
//BUILDER COMMENT. DO NOT REMOVE. auxcode begin
//BUILDER COMMENT. DO NOT REMOVE. auxcode end
};

#endif

