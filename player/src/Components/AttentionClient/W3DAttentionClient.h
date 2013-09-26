#ifndef W3DATTENTIONCLIENT_H
#define W3DATTENTIONCLIENT_H

#include "Component.h"
#include "ObjectState.h"
#include "Attention.h"
#include "StrategyAttentionClientW3D.h"

class W3DAttentionClient : public AttentionClient{
public:
	W3DAttentionClient();
	~W3DAttentionClient();

	void initClient();

	void getAP(tapoint **ap);
	void APnotValid();
	void atAP();
    void setStrategy(string sel);


private:
    StrategyAttentionClientW3D *strategy;
};

#endif
