#ifndef ABSTRACTATTENTION_H
#define ABSTRACTATTENTION_H

#include "Component.h"
#include "ObjectState.h"

#include "Head.h"
#include "Kinematics.h"
#include "AttentionClient.h"
#include <map>


typedef struct
{
	AttentionClient *client;
	long time;
	float percentages;
	long last_set;
	string id;
	bool enabled;

	float quality;
	long last_perc;
}tbufferapoint;


class AbstractAttention : public Component
{
public:

	AbstractAttention();
	~AbstractAttention();

	virtual void init(const string newName, AL::ALPtr<AL::ALBroker> parentBroker) = 0;
	virtual void step() = 0;

	virtual void setPercentage(string id, float percentage) = 0;
	virtual void regist(string id, AttentionClient *client) = 0;
	virtual void setStrategy(string type) = 0;
	virtual void nextElement() = 0;
	//virtual void trackAndActualizeObject(ObjectState *object) = 0;
	//virtual void nextAttentionPoint() = 0;

protected:

	Head *_Head;
	Kinematics *_Kinematics;

	map<string,tbufferapoint*> abuffers;
	map<string,tbufferapoint*>::iterator itabuffer;
};

#endif
