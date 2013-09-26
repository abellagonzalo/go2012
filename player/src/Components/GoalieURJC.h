#ifndef GoalieURJC_H
#define GoalieURJC_H

#include "Component.h"
#include "BallDetector.h"
#include "GoalDetector.h"
#include "WorldState.h"
#include "Attention.h"
#include "Singleton.h"

//BUILDER COMMENT. DO NOT REMOVE. auxinclude begin
//BUILDER COMMENT. DO NOT REMOVE. auxinclude end

class GoalieURJC : public Component, public Singleton<GoalieURJC>
{
public:

	GoalieURJC();
	~GoalieURJC();

	void step();
private:

	static const int Initial	= 0;
	static const int Block	= 1;
	static const int Despeje	= 2;
	int state;

	BallDetector *_BallDetector;
	GoalDetector *_GoalDetector;
	WorldState *_WorldState;
	Attention *_Attention;

	void Initial_state_code(void);
	void Block_state_code(void);
	void Despeje_state_code(void);
	bool Initial2Block0_transition_code(void);
	bool Block2Despeje0_transition_code(void);
	bool Despeje2Block0_transition_code(void);
//BUILDER COMMENT. DO NOT REMOVE. auxcode begin
private:
	void calcVector2D(HPoint2D p1, HPoint2D p2, HPoint3D &v);
	void calIntersectionCircleVector(HPoint3D v, HPoint3D p_c, double r, HPoint3D &int1, HPoint3D &int2);

//BUILDER COMMENT. DO NOT REMOVE. auxcode end
};

#endif

