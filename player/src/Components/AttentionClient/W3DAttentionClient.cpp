#include "W3DAttentionClient.h"

W3DAttentionClient::W3DAttentionClient()
{

	attention = Attention::getInstance();
	strategy = new StrategyAttentionClientW3D;
}

W3DAttentionClient::~W3DAttentionClient(){
}

void W3DAttentionClient::getAP(tapoint **ap)
{
	strategy->getAP(ap);
}
void W3DAttentionClient::atAP()
{
   strategy->atAP();
}


void W3DAttentionClient::initClient(){
	attention->regist("Wave", this);
	strategy->initListAP();
}

void W3DAttentionClient::APnotValid(){
	strategy->APnotValid();
}
void W3DAttentionClient::setStrategy(string sel)
{

}
