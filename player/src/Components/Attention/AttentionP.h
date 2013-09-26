#ifndef AttentionP_H
#define AttentionP_H


#include "AbstractAttention.h"

#include <list>

#define TIME_TO_DISABLE_CLIENT	1000000
#define MAX_TIME_TO_POINT	4000000

class AttentionP : public AbstractAttention, public Singleton<AttentionP>
{
public:

	AttentionP();
	~AttentionP();

	void step();

	void init(const string newName, AL::ALPtr<AL::ALBroker> parentBroker);
	void setPercentage(string id, float percentage);
	void regist(string id, AttentionClient *client);
	void setStrategy(string type);

	tapoint *ap_actual;

	bool ap_at_image;

	void nextElement();

private:

	Head *_Head;
	Kinematics *_Kinematics;

	float kP,kQ;
	bool upQual;
	long timeInit;

	void stepcode();

	bool APReached(HPoint3D p);
	void initScanPoints();
	void checkStimuli();
	list<HPoint3D> scanpoints;
	list<HPoint3D>::iterator itscanpoints;

	HPoint3D actual;

	bool inited;
	long inittime;
};

#endif

