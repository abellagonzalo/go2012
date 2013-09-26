#ifndef Recorder_H
#define Recorder_H

#include "Component.h"
#include "GoalDetector.h"
#include "GTLocalization.h"
#include "Singleton.h"

//BUILDER COMMENT. DO NOT REMOVE. auxinclude begin
//BUILDER COMMENT. DO NOT REMOVE. auxinclude end

class Recorder : public Component, public Singleton<Recorder>
{
public:

	Recorder();
	~Recorder();

	void step();
private:

	static const int Initial	= 0;
	static const int Record	= 1;
	int state;

	GoalDetector *_GoalDetector;
	GTLocalization *_GTLocalization;

	void Initial_state_code(void);
	void Record_state_code(void);
	bool Initial2Record0_transition_code(void);
//BUILDER COMMENT. DO NOT REMOVE. auxcode begin
public:
	void init(const string newName, AL::ALPtr<AL::ALBroker> parentBroker);
private:
	long lastts, initts;
	FILE *fout;
	AL::ALPtr<AL::ALMotionProxy> pmotion;
	vector<float> lastpos;
//BUILDER COMMENT. DO NOT REMOVE. auxcode end
};

#endif

