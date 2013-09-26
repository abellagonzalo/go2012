#include "StrategyAttentionClientBallP.h"
#include "BallDetector.h"

StrategyAttentionClientBallP::StrategyAttentionClientBallP()
{

}

StrategyAttentionClientBallP::~StrategyAttentionClientBallP()
{
}

void
StrategyAttentionClientBallP::getAP(tapoint **ap){

	BallDetector *_BallDetector = BallDetector::getInstance();

	if(_BallDetector->ballmodel->elapsedTimeSinceLastObs<ObjectState::LONG_TIME)
	{
		//cerr<<"Que veo la bola"<<endl;
		Vector2<double> pos = _BallDetector->ballmodel->estimate.getPositionInRelativeCoordinates();
		ap_actual.x = pos[0];
		ap_actual.y = pos[1];
		ap_actual.z = 0.0;
		ap_actual.ts = getCurrentTimeBis();
		ap_actual.its = getCurrentTimeBis();
		ap_actual.time = 0;

		*ap = &ap_actual;
	}else
	{
		//cerr<<"que no la veo"<<endl;
		*ap = NULL; //unknown
	}

}
void
StrategyAttentionClientBallP::atAP(){

}

void
StrategyAttentionClientBallP::initListAP(){

//	cout<<"initListaAP BallRR:"<<endl;

	search_aps.clear();

	addAP(200.0, 0.0, 0.0);
	addAP(300.0, 200.0, 0.0);
	addAP(300.0, -200.0, 0.0);
	addAP(1000.0, -1200.0, 0.0);
	addAP(1000.0, 0.0, 0.0);
	addAP(1000.0, 1200.0, 0.0);
	addAP(2000.0, 2000.0, 0.0);
	addAP(2000.0, 0.0, 0.0);
	addAP(2000.0, -2000.0, 0.0);

	it_search_aps = search_aps.begin();
}
void StrategyAttentionClientBallP::APnotValid(){

}
void
StrategyAttentionClientBallP::updateAP()
{

}
