#include "BallAttentionClient.h"

BallAttentionClient::BallAttentionClient()
{
	attention = Attention::getInstance();

	  sRR = new StrategyAttentionClientBallRR;
	  sQ = new StrategyAttentionClientBallQ;
	  sP = new StrategyAttentionClientBallP;

	  this->strategy = sRR;
	  begin = false;
}


BallAttentionClient::~BallAttentionClient(){


}

void BallAttentionClient::setStrategy(string sel)
{
   if (sel=="RR") this->strategy = sRR;
   if (sel=="Q") this->strategy = sQ;
   if (sel=="P") this->strategy = sP;

   strategy->initListAP();
}

void BallAttentionClient::getAP(tapoint **ap)
{
   if(!begin)
	   begin = true;
   strategy->getAP(ap);
}
void BallAttentionClient::atAP()
{
   strategy->atAP();
}


void BallAttentionClient::initClient(){
	attention->regist("Ball", this);

	strategy->initListAP();
}

void BallAttentionClient::APnotValid(){

	strategy->APnotValid();
}
void
BallAttentionClient::updateAP()
{
	if(begin){
		strategy->updateAP();
	}

}
