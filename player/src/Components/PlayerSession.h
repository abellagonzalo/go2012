#ifndef PlayerSession_H
#define PlayerSession_H

#include "Component.h"
#include "Localization.h"
#include "Singleton.h"

//BUILDER COMMENT. DO NOT REMOVE. auxinclude begin
//BUILDER COMMENT. DO NOT REMOVE. auxinclude end

class PlayerSession : public Component, public Singleton<PlayerSession>
{
public:

	PlayerSession();
	~PlayerSession();

	void step();
private:

	static const int Initial	= 0;
	static const int Play	= 1;
	static const int PreSession	= 2;
	int state;

	Localization *_Localization;

	void Initial_state_code(void);
	void Play_state_code(void);
	void PreSession_state_code(void);
	bool PreSession2Play0_transition_code(void);
	bool Play2PreSession0_transition_code(void);
	bool Initial2PreSession0_transition_code(void);
//BUILDER COMMENT. DO NOT REMOVE. auxcode begin
public:
	void init(const string newName, AL::ALPtr<AL::ALBroker> parentBroker);
private:

	void restartSession();

	long lastts, initts;
	FILE *fout, *fsession, *fin;
	AL::ALPtr<AL::ALMotionProxy> pmotion;
	vector<float> lastpos;

	long startSessionTS;

	bool newSession, finishedSession;

	char locali[1024];
	char foutname[1024];
	char finname[1024];

//BUILDER COMMENT. DO NOT REMOVE. auxcode end
};

#endif

