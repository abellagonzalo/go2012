#ifndef STRATEGYATTENTIONCLIENT_H
#define STRATEGYATTENTIONCLIENT_H

#include "AttentionClient.h"
#include "progeo.h"



class StrategyAttentionClient
{
public:

	StrategyAttentionClient();
	~StrategyAttentionClient();

	virtual void getAP(tapoint **) = 0;
	virtual void atAP() = 0;
	virtual void APnotValid() = 0;
	virtual void initListAP() = 0;
	virtual void updateAP() = 0;

protected:

	void addAP(float px, float py, float pz);
	bool APReachedQ(tapoint *tap);
	int refreshPosition(float posA, float posB);

	static const float PI = 3.141592;


	tapoint ap_actual;
	list<tapoint*> search_aps;
	list<tapoint*>::iterator it_search_aps;

	struct timeval 	currentTime;
	inline long getCurrentTimeBis() {
		gettimeofday(&currentTime, NULL);
		return currentTime.tv_sec * 1000000 + currentTime.tv_usec;
	}
};

#endif


