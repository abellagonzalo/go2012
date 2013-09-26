#ifndef GameArbitrator_H
#define GameArbitrator_H

#include "Component.h"
#include "Fallen.h"
#include "Body.h"
#include "Head.h"
#include "Go2gpos.h"
#include "Attention.h"
#include "Striker.h"
#include "Singleton.h"

//BUILDER COMMENT. DO NOT REMOVE. auxinclude begin

#include "GameController.h"

//BUILDER COMMENT. DO NOT REMOVE. auxinclude end

class GameArbitrator : public Component, public Singleton<GameArbitrator>
{
public:

	GameArbitrator();
	~GameArbitrator();

	void step();
private:

	static const int Initial	= 0;
	static const int Penalished	= 1;
	static const int Set	= 2;
	static const int Ready	= 3;
	static const int Play	= 4;
	static const int Finish	= 5;
	static const int RecoveringFromFall	= 6;
	int state;

	Fallen *_Fallen;
	Body *_Body;
	Head *_Head;
	Go2gpos *_Go2gpos;
	Attention *_Attention;
	Striker *_Striker;

	void Initial_state_code(void);
	void Penalished_state_code(void);
	void Set_state_code(void);
	void Ready_state_code(void);
	void Play_state_code(void);
	void Finish_state_code(void);
	void RecoveringFromFall_state_code(void);
	bool Finish2Initial0_transition_code(void);
	bool Initial2Play0_transition_code(void);
	bool Play2Initial0_transition_code(void);
	bool Penalished2Initial0_transition_code(void);
	bool Set2RecoveringFromFall0_transition_code(void);
	bool RecoveringFromFall2Initial0_transition_code(void);
	bool Set2Initial0_transition_code(void);
	bool Initial2Ready0_transition_code(void);
	bool Ready2Initial0_transition_code(void);
	bool Play2Penalished0_transition_code(void);
	bool Penalished2Play0_transition_code(void);
	bool Ready2RecoveringFromFall0_transition_code(void);
	bool Initial2Set0_transition_code(void);
	bool Play2RecoveringFromFall0_transition_code(void);
	bool Initial2Finish0_transition_code(void);
	bool Initial2RecoveringFromFall0_transition_code(void);
//BUILDER COMMENT. DO NOT REMOVE. auxcode begin
public:
	void init(const string newName, AL::ALPtr<AL::ALBroker> parentBroker);
	GameController *getRGC();

private:
	GameController *gc;
	RoboCupGameControlData *rgc;
//BUILDER COMMENT. DO NOT REMOVE. auxcode end
};

#endif

