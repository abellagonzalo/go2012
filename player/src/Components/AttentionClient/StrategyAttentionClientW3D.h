#ifndef STRATEGYATTENTIONCLIENTW3D_H
#define STRATEGYATTENTIONCLIENTW3D_H


#include "StrategyAttentionClient.h"

class StrategyAttentionClientW3D :	public StrategyAttentionClient
{
public:

	StrategyAttentionClientW3D();
	~StrategyAttentionClientW3D();

    void getAP(tapoint **ap);
    void atAP();
    void initListAP();
    void APnotValid();

    void updateAP();
};
#endif
