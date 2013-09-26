#ifndef STRATEGYATTENTIONCLIENTGOALRR_H
#define STRATEGYATTENTIONCLIENTGOALRR_H


#include "StrategyAttentionClient.h"

class StrategyAttentionClientGoalRR :	public StrategyAttentionClient
{
public:

	StrategyAttentionClientGoalRR();
	~StrategyAttentionClientGoalRR();

    void getAP(tapoint **ap);
    void atAP();
    void initListAP();
    void APnotValid();

    void updateAP();
protected:
	int post;
	bool nextelem;
	int skip;
	bool forcesearch;

};
#endif
