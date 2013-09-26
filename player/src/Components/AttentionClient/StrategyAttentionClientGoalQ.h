#ifndef STRATEGYATTENTIONCLIENTGOALQ_H
#define STRATEGYATTENTIONCLIENTGOALQ_H


#include "StrategyAttentionClient.h"

class StrategyAttentionClientGoalQ : public StrategyAttentionClient
{
public:

	StrategyAttentionClientGoalQ();
	~StrategyAttentionClientGoalQ();

    void getAP(tapoint **ap);
    void atAP();
    void initListAP();
    void APnotValid();

    void updateAP();
private:
    tapoint* ap_pLeft;
    tapoint* ap_pRight;
    tapoint* ap_pAux;
	list<tapoint*>::iterator ap_pointer;
	string lastPostSeen;
	bool seenR, seenL, toAux;
	static const int POST_DIST = 1400.0;
};
#endif
