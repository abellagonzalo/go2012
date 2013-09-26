#ifndef Attention_H
#define Attention_H

#include "Component.h"
#include "ObjectState.h"
#include "AbstractAttention.h"
#include "AttentionRR.h"
#include "AttentionQ.h"
#include "AttentionP.h"

class Attention : public Component, public Singleton<Attention>
{
public:

	Attention();
	~Attention();

	void step();
	void init(const string newName, AL::ALPtr<AL::ALBroker> parentBroker);

	/*Set the localization algorithm to use*/
	void setAttention(string type);

	void setPercentage(string id, float percentage);
	void regist(string id, AttentionClient *client);

	void nextElement();
	//void trackAndActualizeObject(ObjectState *object);
	//void nextAttentionPoint();

private:

	AttentionRR *_AttentionRR;
	AttentionQ *_AttentionQ;
	AttentionP *_AttentionP;

	AbstractAttention *currentAttention;
};

#endif

