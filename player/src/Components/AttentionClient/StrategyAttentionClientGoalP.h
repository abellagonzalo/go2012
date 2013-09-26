#ifndef STRATEGYATTENTIONCLIENTGOALP_H
#define STRATEGYATTENTIONCLIENTGOALP_H


#include "StrategyAttentionClient.h"

class StrategyAttentionClientGoalP :	public StrategyAttentionClient
{
public:

	StrategyAttentionClientGoalP();
	~StrategyAttentionClientGoalP();

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
