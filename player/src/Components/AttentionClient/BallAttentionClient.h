#ifndef BALLATTENTIONCLIENT_H
#define BALLATTENTIONCLIENT_H

#include "Component.h"
#include "ObjectState.h"
#include "Attention.h"
#include "StrategyAttentionClient.h"
#include "StrategyAttentionClientBallQ.h"
#include "StrategyAttentionClientBallRR.h"
#include "StrategyAttentionClientBallP.h"

class BallAttentionClient : public AttentionClient{
public:
	BallAttentionClient();
	~BallAttentionClient();

	void initClient();

	void getAP(tapoint **ap);
	void APnotValid();
	void atAP();
    void setStrategy(string sel);

    void updateAP();

private:
    StrategyAttentionClient *strategy;
    StrategyAttentionClientBallRR *sRR;
    StrategyAttentionClientBallQ *sQ;
    StrategyAttentionClientBallP *sP;

    bool begin;
};

#endif
