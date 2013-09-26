#ifndef MatchPlayer_H
#define MatchPlayer_H

#include "Component.h"
#include "Localization.h"
#include "Body.h"
#include "Head.h"
#include "GoalDetector.h"
#include "BallDetector.h"
#include "WorldState.h"
#include "Attention.h"
#include "Go2gpos.h"
#include "AuxRole.h"
#include "AuxPenalty.h"
#include "Singleton.h"

//BUILDER COMMENT. DO NOT REMOVE. auxinclude begin

#include "GameController.h"
#include "AbstractLocalization.h"

//BUILDER COMMENT. DO NOT REMOVE. auxinclude end

class MatchPlayer : public Component, public Singleton<MatchPlayer>
{
public:

	MatchPlayer();
	~MatchPlayer();

	void step();
private:

	static const int Initial	= 0;
	static const int Initial_GS	= 1;
	static const int Ready_GS	= 2;
	static const int Set_GS	= 3;
	static const int Playing_GS	= 4;
	static const int Penalty_GS	= 5;
	static const int Penalised	= 6;
	static const int Finished_GS	= 7;
	int state;

	Localization *_Localization;
	Body *_Body;
	Head *_Head;
	GoalDetector *_GoalDetector;
	BallDetector *_BallDetector;
	WorldState *_WorldState;
	Attention *_Attention;
	Go2gpos *_Go2gpos;
	AuxRole *_AuxRole;
	AuxPenalty *_AuxPenalty;

	void Initial_state_code(void);
	void Initial_GS_state_code(void);
	void Ready_GS_state_code(void);
	void Set_GS_state_code(void);
	void Playing_GS_state_code(void);
	void Penalty_GS_state_code(void);
	void Penalised_state_code(void);
	void Finished_GS_state_code(void);
	bool Initial_GS2Initial0_transition_code(void);
	bool Initial2Initial_GS0_transition_code(void);
	bool Ready_GS2Initial0_transition_code(void);
	bool Set_GS2Initial0_transition_code(void);
	bool Playing_GS2Initial0_transition_code(void);
	bool Penalty_GS2Initial0_transition_code(void);
	bool Penalised2Playing_GS1_transition_code(void);
	bool Finished_GS2Initial0_transition_code(void);
	bool Initial2Ready_GS0_transition_code(void);
	bool Initial2Set_GS0_transition_code(void);
	bool Initial_GS2Penalised1_transition_code(void);
	bool Playing_GS2Penalised1_transition_code(void);
	bool Initial2Playing_GS0_transition_code(void);
	bool Initial2Penalty_GS0_transition_code(void);
	bool Initial2Finished_GS0_transition_code(void);
//BUILDER COMMENT. DO NOT REMOVE. auxcode begin
public:
	void init(const string newName, AL::ALPtr<AL::ALBroker> parentBroker);
//BUILDER COMMENT. DO NOT REMOVE. auxcode end
};

#endif

