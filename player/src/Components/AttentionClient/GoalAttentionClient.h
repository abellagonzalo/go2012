#ifndef GOALATTENTIONCLIENT_H
#define GOALATTENTIONCLIENT_H

#include "Component.h"
#include "ObjectState.h"
#include "Attention.h"
#include "StrategyAttentionClient.h"
#include "StrategyAttentionClientGoalQ.h"
#include "StrategyAttentionClientGoalRR.h"
#include "StrategyAttentionClientGoalP.h"

class GoalAttentionClient : public AttentionClient{
public:
	GoalAttentionClient();
	~GoalAttentionClient();

	void initClient();

	void getAP(tapoint **ap);
	void APnotValid();
	void atAP();
    void setStrategy(string sel);

    void updateAP();

private:
    StrategyAttentionClient *strategy;
    StrategyAttentionClientGoalRR *sRR;
    StrategyAttentionClientGoalQ *sQ;
    StrategyAttentionClientGoalP *sP;

    bool begin;
};

#endif
