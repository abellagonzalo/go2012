#ifndef STRATEGYATTENTIONCLIENTBALLQ_H
#define STRATEGYATTENTIONCLIENTBALLQ_H


#include "StrategyAttentionClient.h"

class StrategyAttentionClientBallQ :	public StrategyAttentionClient
{
public:

	StrategyAttentionClientBallQ();
	~StrategyAttentionClientBallQ();

    void getAP(tapoint **ap);
    void atAP();
    void initListAP();
    void APnotValid();

    void updateAP();

private:
	list<tapoint*>::iterator ap_pointer;
	tapoint* ap_ball;

};
#endif
