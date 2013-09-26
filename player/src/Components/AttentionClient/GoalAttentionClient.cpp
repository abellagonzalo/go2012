#include "GoalAttentionClient.h"

GoalAttentionClient::GoalAttentionClient()
{
	attention = Attention::getInstance();

	  sRR = new StrategyAttentionClientGoalRR;
	  sQ = new StrategyAttentionClientGoalQ;
	  sP = new StrategyAttentionClientGoalP;

	  this->strategy = sRR;
}

GoalAttentionClient::~GoalAttentionClient(){
}

void GoalAttentionClient::setStrategy(string sel)
{
   if (sel=="RR") this->strategy = sRR;
   if (sel=="Q") this->strategy = sQ;
   if (sel=="P") this->strategy = sP;

   strategy->initListAP();

}
void GoalAttentionClient::getAP(tapoint **ap)
{
   if(!begin)
	   begin = true;
   strategy->getAP(ap);
}
void GoalAttentionClient::atAP()
{
   strategy->atAP();
}

void GoalAttentionClient::initClient(){

	attention->regist("Goal", this);
	strategy->initListAP();

}

void GoalAttentionClient::APnotValid(){
	strategy->APnotValid();

}
void
GoalAttentionClient::updateAP()
{
	if(begin){
		strategy->updateAP();
	}
}
