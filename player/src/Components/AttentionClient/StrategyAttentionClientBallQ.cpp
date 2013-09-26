#include "StrategyAttentionClientBallQ.h"
#include "BallDetector.h"
#include "Component.h"

StrategyAttentionClientBallQ::StrategyAttentionClientBallQ()
{

	ap_ball = new tapoint;
}

StrategyAttentionClientBallQ::~StrategyAttentionClientBallQ()
{
}

void
StrategyAttentionClientBallQ::getAP(tapoint **ap){

	long now,diff;
	float aux,aux2;

	if((*ap_pointer)->salience > 1 || (*ap_pointer)->x < 0.1){
		cout<<"FAIL";
		cout<<"["<<(*ap_pointer)->x<<","<<(*ap_pointer)->y<<","<<(*ap_pointer)->z<<"]S:";
		cout<<((int)((*ap_pointer)->salience*1000))/1000.0;
		cout<<endl;
		(*ap_pointer)->salience = 0.0;
		ap_pointer = search_aps.begin();

	}
	if((*ap_pointer)->salience > 0.1){
		*ap = (*ap_pointer);
//		cout<<"B";
	}else{
//		cout<<"C";
		ap_pointer++;
		if(ap_pointer == search_aps.end()){
			ap_pointer = search_aps.begin();
		}

		for(it_search_aps=search_aps.begin();it_search_aps!=search_aps.end();it_search_aps++){
			aux = (*it_search_aps)->salience;
			aux2 = (*ap_pointer)->salience;
			if(aux > aux2 && (*it_search_aps)->x > 10){
				ap_pointer = it_search_aps;
			}
		}
		it_search_aps=search_aps.begin();
		*ap = (*ap_pointer);
	}
//	cout<<"\tAPBALL["<<(*ap_pointer)->x<<","<<(*ap_pointer)->y<<","<<(*ap_pointer)->z<<"]S:";
//	cout<<((int)((*ap_pointer)->salience*1000))/1000.0;
}
void
StrategyAttentionClientBallQ::atAP(){

}

void
StrategyAttentionClientBallQ::initListAP(){

//	cout<<"initListaAP BallQ:"<<endl;

	search_aps.clear();

	addAP(300.0, 0.0, 0.0);
	addAP(300.0, 200.0, 0.0);
	addAP(300.0, -200.0, 0.0);
	addAP(1500.0, -1200.0, 0.0);
	addAP(1500.0, 0.0, 0.0);
	addAP(1500.0, 1200.0, 0.0);
	addAP(3500.0, 3000.0, 0.0);
	addAP(3500.0, 0.0, 0.0);
	addAP(3500.0, -3000.0, 0.0);


	ap_ball->x = 0.0;
	ap_ball->y = 0.0;
	ap_ball->z = 0.0;
	ap_ball->ts = getCurrentTimeBis();
	ap_ball->its = getCurrentTimeBis();
	ap_ball->time = 0;
	ap_ball->ap_at_image = false;
	ap_ball->salience = 0.0;
	ap_ball->last_time = getCurrentTimeBis()/1000;
	ap_ball->dynamic = true;

	search_aps.push_back(ap_ball);

	it_search_aps = search_aps.begin();
	ap_pointer = search_aps.begin();
}

void
StrategyAttentionClientBallQ::APnotValid(){

}
void
StrategyAttentionClientBallQ::updateAP()
{
	long now;
	float newS;

	for(it_search_aps = search_aps.begin(); it_search_aps != search_aps.end();++it_search_aps){
		now = getCurrentTimeBis()/1000;
		if((now - (*it_search_aps)->last_time) > 5.0 * Component::SHORT_RATE){
			(*it_search_aps)->last_time = now;
		}

		if(APReachedQ(*it_search_aps)){
			newS = (*it_search_aps)->salience - ((now - (*it_search_aps)->last_time)*(1.0/700));
			if(newS > 0){
				(*it_search_aps)->salience = newS;
			}else{
				(*it_search_aps)->salience = 0.0;
			}

		}else{
			if((*ap_pointer)==(*it_search_aps)){
				newS = (*it_search_aps)->salience - ((now - (*it_search_aps)->last_time)*(1.0/2000));
				if(newS > 0){
					(*it_search_aps)->salience = newS;
				}else{
					(*it_search_aps)->salience = 0.0;
				}
			}else{
				newS = (*it_search_aps)->salience + ((now - (*it_search_aps)->last_time)*(1.0/1000));
				if(newS < 1){
					(*it_search_aps)->salience = newS;
				}else{
					(*it_search_aps)->salience = 1.0;
				}
			}
		}
//		cout << "\t" << ((int)((*it_search_aps)->salience*1000))/1000.0;
		(*it_search_aps)->last_time = getCurrentTimeBis()/1000;
	}


	BallDetector *_BallDetector = BallDetector::getInstance();

	Vector2<double> pos = _BallDetector->ballmodel->estimate.getPositionInRelativeCoordinates();
	ap_ball->x = refreshPosition(ap_ball->x,(float)pos[0]);
	ap_ball->y = refreshPosition(ap_ball->x,(float)pos[1]);
	ap_ball->z = 0.0;

	if(ap_ball->x < 10){
		ap_ball->salience = 0.0;
	}
	if(_BallDetector->ballmodel->estimate.getReliability()>=ObjectState::MOST_RELIABLE)
	{
		ap_ball->salience = 1.0;
		ap_ball->last_time = getCurrentTimeBis()/1000;

		for(it_search_aps=search_aps.begin();it_search_aps!=search_aps.end();it_search_aps++){
			if(!(*it_search_aps)->dynamic){
				(*it_search_aps)->salience = 0.0;
			}
		}
		it_search_aps=search_aps.begin();
//		cerr<<"A";
	}
//	cout<<"\tBall("<<ap_ball->x<<","<<ap_ball->y<<","<<ap_ball->z<<")";
//	cout<< "S:" << ((int)(ap_ball->salience*1000))/1000.0;
//	cout <<endl;
	it_search_aps = search_aps.begin();
}
