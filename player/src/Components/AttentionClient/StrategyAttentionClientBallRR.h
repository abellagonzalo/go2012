#ifndef STRATEGYATTENTIONCLIENTBALLRR_H
#define STRATEGYATTENTIONCLIENTBALLRR_H


#include "StrategyAttentionClient.h"

class StrategyAttentionClientBallRR :	public StrategyAttentionClient
{
public:

	StrategyAttentionClientBallRR();
	~StrategyAttentionClientBallRR();

    void getAP(tapoint **ap);
    void atAP();
    void initListAP();
    void APnotValid();

    void updateAP();
};
#endif
