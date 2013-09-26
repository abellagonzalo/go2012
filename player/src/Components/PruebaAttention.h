#ifndef PruebaAttention_H
#define PruebaAttention_H

#include "Component.h"
#include "GoalDetector.h"
#include "Attention.h"
#include "BallDetector.h"
#include "Body.h"
#include "Singleton.h"

//BUILDER COMMENT. DO NOT REMOVE. auxinclude begin
//BUILDER COMMENT. DO NOT REMOVE. auxinclude end

class PruebaAttention : public Component, public Singleton<PruebaAttention>
{
public:

	PruebaAttention();
	~PruebaAttention();

	void step();
private:

	static const int Initial	= 0;
	static const int Paso1	= 1;
	static const int paso2	= 2;
	int state;

	Attention *_Attention;
	BallDetector *_BallDetector;
	GoalDetector *_GoalDetector;
	Body *_Body;

	void Initial_state_code(void);
	void Paso1_state_code(void);
	void paso2_state_code(void);
	bool Initial2Visualizando0_transition_code(void);
	bool Paso12paso20_transition_code(void);
	bool paso22Paso10_transition_code(void);
//BUILDER COMMENT. DO NOT REMOVE. auxcode begin
//BUILDER COMMENT. DO NOT REMOVE. auxcode end
};

#endif

