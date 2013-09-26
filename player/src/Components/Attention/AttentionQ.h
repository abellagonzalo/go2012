#ifndef AttentionQ_H
#define AttentionQ_H


#include "AbstractAttention.h"

#include <list>


class AttentionQ : public AbstractAttention, public Singleton<AttentionQ>
{
public:

	AttentionQ();
	~AttentionQ();

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
	bool APReached(tapoint *tap);

	float getMyQuality(string id);
	void updateQuality(tbufferapoint* aux);
	void percentageReduce(tbufferapoint* aux);
	string priorityCalculate(tbufferapoint* aux, string maxPriority);

};

#endif

