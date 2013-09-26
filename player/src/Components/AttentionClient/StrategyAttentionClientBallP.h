#ifndef STRATEGYATTENTIONCLIENTBALLP_H
#define STRATEGYATTENTIONCLIENTBALLP_H


#include "StrategyAttentionClient.h"

class StrategyAttentionClientBallP :	public StrategyAttentionClient
{
public:

	StrategyAttentionClientBallP();
	~StrategyAttentionClientBallP();

    void getAP(tapoint **ap);
    void atAP();
    void initListAP();
    void APnotValid();

    void updateAP();
};
#endif
