#include "StrategyAttentionClientW3D.h"
#include "BallDetector.h"

StrategyAttentionClientW3D::StrategyAttentionClientW3D()
{

}

StrategyAttentionClientW3D::~StrategyAttentionClientW3D()
{
}

void
StrategyAttentionClientW3D::getAP(tapoint **ap){

	BallDetector *_BallDetector = BallDetector::getInstance();

	static int cont=0;
	//if(ballmodel->estimate.getReliability()>=SingleObjectState::MOST_RELIABLE)
	if(_BallDetector->ballmodel->elapsedTimeSinceLastObs<ObjectState::LONG_TIME)
	{


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
		//cerr<<"BOLA PERDIDA!!!"<<endl;
		long inpos = getCurrentTimeBis()-(*it_search_aps)->ts;
		inpos = (*it_search_aps)->time;

		//cerr<<"["<<inpos<<" = "<<(*it_search_aps)->x<<","<<(*it_search_aps)->y<<"]"<<endl;

		if((((*it_search_aps)->ap_at_image) && (inpos>500000)) || (inpos>2000000))
		{

			if(cont++>2) //Cada tres AP veces volvemos a la última estimación
			{
				Vector2<double> pos = _BallDetector->ballmodel->estimate.getPositionInRelativeCoordinates();
				ap_actual.x = pos[0];
				ap_actual.y = pos[1];
				ap_actual.z = 0.0;
				ap_actual.ts = getCurrentTimeBis();
				ap_actual.its = getCurrentTimeBis();
				ap_actual.time = 0;

				*ap = &ap_actual;
				cont = 0;
			}else
			{
				it_search_aps++;
				if(it_search_aps == search_aps.end())
					it_search_aps = search_aps.begin();

				(*it_search_aps)->ts = getCurrentTimeBis();
				(*it_search_aps)->its = getCurrentTimeBis();
				(*it_search_aps)->time = 0;
			}
		}

		//if(ap_actual.x>0.0)
		*ap = (*it_search_aps);
		//else
		//{
		//	cerr<<"**********************"<<endl;
		//	*ap = NULL;
		//}
	}

}
void
StrategyAttentionClientW3D::atAP(){

}

void
StrategyAttentionClientW3D::initListAP(){

//	cout<<"initListaAP W3D:"<<endl;

	search_aps.clear();

	addAP(200.0, 0.0, 0.0);
	addAP(300.0, 200.0, 0.0);
	addAP(300.0, -200.0, 0.0);
	addAP(1500.0, -1200.0, 0.0);
	addAP(1500.0, 0.0, 0.0);
	addAP(1500.0, 1200.0, 0.0);
	addAP(3500.0, 3000.0, 0.0);
	addAP(3500.0, 0.0, 0.0);
	addAP(3500.0, -3000.0, 0.0);

	it_search_aps = search_aps.begin();
}
void
StrategyAttentionClientW3D::APnotValid(){

}
void
StrategyAttentionClientW3D::updateAP()
{

}
