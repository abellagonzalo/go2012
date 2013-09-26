#ifndef Stratego_H
#define Stratego_H

#include "Component.h"
#include "StrategyRoles.h"
#include "Striker2.h"
#include "BallDetector.h"
#include "Body.h"
#include "Attention.h"
#include "Go2gpos.h"
#include "Singleton.h"

//BUILDER COMMENT. DO NOT REMOVE. auxinclude begin
#include "StrategyRoles.h"
//BUILDER COMMENT. DO NOT REMOVE. auxinclude end

class Stratego : public Component, public Singleton<Stratego>
{
public:

	Stratego();
	~Stratego();

	void step();
private:

	static const int Initial	= 0;
	static const int StrikerRol	= 1;
	static const int AttackerRol	= 2;
	static const int DefenderRol	= 3;
	static const int RealInitial	= 4;
	int state;

	StrategyRoles *_StrategyRoles;
	Striker2 *_Striker2;
	BallDetector *_BallDetector;
	Body *_Body;
	Attention *_Attention;
	Go2gpos *_Go2gpos;

	void Initial_state_code(void);
	void StrikerRol_state_code(void);
	void AttackerRol_state_code(void);
	void DefenderRol_state_code(void);
	void RealInitial_state_code(void);
	bool StrikerRol2DefenderRol0_transition_code(void);
	bool DefenderRol2StrikerRol0_transition_code(void);
	bool AttackerRol2StrikerRol0_transition_code(void);
	bool RealInitial2StrikerRol0_transition_code(void);
	bool Initial2RealInitial0_transition_code(void);
	bool StrikerRol2AttackerRol0_transition_code(void);
	bool DefenderRol2AttackerRol0_transition_code(void);
	bool AttackerRol2DefenderRol0_transition_code(void);
	bool RealInitial2AttackerRol0_transition_code(void);
	bool RealInitial2DefenderRol0_transition_code(void);
//BUILDER COMMENT. DO NOT REMOVE. auxcode begin
public:
	void init(const string newName, AL::ALPtr<AL::ALBroker> parentBroker);
private:
	Leds *leds;
//BUILDER COMMENT. DO NOT REMOVE. auxcode end
};

#endif

