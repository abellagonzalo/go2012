#ifndef AttentionRR_H
#define AttentionRR_H

#include "AbstractAttention.h"

#include <list>

#define TIME_TO_DISABLE_CLIENT	1000000
#define TIME_CYCLE				8000000


class AttentionRR : public AbstractAttention, public Singleton<AttentionRR>
{
public:

	AttentionRR();
	~AttentionRR();

	void step();

	void init(const string newName, AL::ALPtr<AL::ALBroker> parentBroker);
	void setPercentage(string id, float percentage);
	void setStrategy(string type);
	void regist(string id, AttentionClient *client);
	void nextElement();
	//void trackAndActualizeObject(ObjectState *object);
	//void nextAttentionPoint();

	tapoint *ap_actual;

private:

	void stepcode();

	void recalculateTimes();

	bool isObservable(tapoint *tap);
	bool APReached(tapoint *tap);

	long time_at_ap;
	bool ap_at_image;

	long cycle;
	long last_jump;
};

#endif

