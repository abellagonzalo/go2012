#ifndef ATTENTIONCLIENT_H
#define ATTENTIONCLIENT_H


#include "ObjectState.h"



typedef struct
{
	float x, y, z;
	long ts;
	long time;
	long its;
	bool ap_at_image;

	float salience;
	long last_time;
	bool dynamic;
}tapoint;


class Attention;

class AttentionClient{
public:
	AttentionClient();
	~AttentionClient();

	virtual void initClient() = 0;

	virtual void getAP(tapoint **) = 0;
	virtual void APnotValid() = 0;
	virtual void atAP() = 0;
    virtual void setStrategy(string sel) = 0;

protected:
	Attention *attention;

};

#endif
