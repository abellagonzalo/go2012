#include "AttentionQ.h"
#include "Detector.h"
#include "GoalDetector.h"
#include "BallDetector.h"
#include "Horizon.h"
#include "../lib/vision/VisionDefinitions.h"

AttentionQ::AttentionQ() {
	_Head = Head::getInstance();
	_Kinematics = Kinematics::getInstance();

	ap_at_image = false;
	ap_actual = new tapoint;
}

AttentionQ::~AttentionQ() {

}

void AttentionQ::stepcode(void) {

	string maxPriority = "None";
	map<string, tbufferapoint*>::iterator iter;

	//cout << "\t"<<(getCurrentTime()/1000 - (timeInit));
	for (iter = abuffers.begin(); iter != abuffers.end(); ++iter) {

		percentageReduce((*iter).second);
		updateQuality((*iter).second);
		(*iter).second->last_set = getCurrentTime()/1000;
		maxPriority = priorityCalculate((*iter).second,maxPriority);

		//cerr << "\tP:" << ((int)((*iter).second->percentages*1000))/1000.0;
		//cerr << "\tQ:" << ((int)((*iter).second->quality*1000))/1000.0;
	}
	//cout << "\t" << maxPriority;

	if( !(maxPriority.compare("None") == 0) ){
		abuffers[maxPriority]->client->getAP(&ap_actual);
		if(APReached(ap_actual))
			abuffers[maxPriority]->client->atAP();
	}

	float pang, tiltg;

	//cout << "\t" <<"["<<ap_actual->x<<","<<ap_actual->y <<","<<ap_actual->z<<"]APQ";

	_Kinematics->getNeckAngles(ap_actual->x, ap_actual->y, ap_actual->z, pang, tiltg);

//	cout <<"\tp"<<pang<<"\tt"<<tiltg;

	_Head->setTiltPos(tiltg, 0.4);
	_Head->setPanPos(pang, 0.4);

//	cout << endl;
}

void AttentionQ::step(void) {
	_Head->step();
	_Kinematics->step();

	if (isTime2Run()) {
		stepcode();
	}
	_Head->step();
}

void AttentionQ::init(const string newName, AL::ALPtr<AL::ALBroker> parentBroker) {

	Component::init(newName, parentBroker);

	this->setFreqTime(SHORT_RATE);
	itabuffer = abuffers.begin();
	timeInit = getCurrentTime()/1000;
	ap_actual->x=1000.0;
	ap_actual->y=0.0;
	ap_actual->z=0.0;
	upQual = false;
	kP = 1.0/2000;
	kQ = 1.0/2000;
}

void AttentionQ::setPercentage(string id, float percentage) {

	if (abuffers.count(id)<=0) {
		return;
	}

	tbufferapoint *aux = abuffers[id];

	if (percentage >= aux->percentages){
		aux->percentages = percentage;
		aux->last_perc = getCurrentTime()/1000;
	}
}

void AttentionQ::regist(string id, AttentionClient *client){

	tbufferapoint *aux = new tbufferapoint;

	aux->client = client;
	aux->time = 0.0;
	aux->percentages = 0.0;
	aux->id = id;
	aux->enabled = false;
//	aux->last_set = getCurrentTime();

	aux->last_set = getCurrentTime()/1000;
	aux->last_perc = getCurrentTime()/1000;
	aux->quality = getMyQuality(id);

	abuffers[id] = aux;

//	cerr << "resgistQ ="<<id<< endl;
}


float AttentionQ::getMyQuality(string id){


	float p0LeftG, p0RightG, p0CenterG;
	float p1LeftG, p1RightG, p1CenterG;
	float qP0, qP1;

	BallDetector *_BallDetector = BallDetector::getInstance();
	GoalDetector *_GoalDetector = GoalDetector::getInstance();


	if (id.compare("Ball") == 0){

		return _BallDetector->ballmodel->estimate.getQuality();

	}else if (id.compare("Goal") == 0){

		p0LeftG = _GoalDetector->goals->p0LeftEstimate.getQuality();
		p0RightG = _GoalDetector->goals->p0RightEstimate.getQuality();
		qP0 = (p0LeftG + p0RightG)/2.0;

		p1LeftG = _GoalDetector->goals->p1LeftEstimate.getQuality();
		p1RightG = _GoalDetector->goals->p1RightEstimate.getQuality();
		qP1 = (p1LeftG + p1RightG)/2.0;

		if(qP0 > qP1)
			return qP0;
		else
			return qP1;
	}else{
		return 0.0;
	}
}

void AttentionQ::percentageReduce(tbufferapoint* aux){

	long now;
	float newPerc;

	now = getCurrentTime()/1000;
	if((now - aux->last_perc)>500){
		newPerc = aux->percentages - ((now - aux->last_set)* kP);
		if(newPerc>0){
			aux->percentages = newPerc;
		}else{
			aux->percentages = 0.0;
		}
	}
}

string AttentionQ::priorityCalculate(tbufferapoint* aux, string maxPriority){

	float diffA,diffB;

	if (aux->percentages > 0.0){

		if(maxPriority.compare("None") == 0){
			return aux->id;
		}else{

			diffA = (aux->quality)-(aux->percentages);
			diffB = (abuffers[maxPriority]->quality)-(abuffers[maxPriority]->percentages);

			if (diffA < diffB){
				return aux->id;
			}else{
				return maxPriority;
			}
		}

	}else{
		return maxPriority;
	}
}

bool AttentionQ::APReached(tapoint *tap)
{

	HPoint3D p1, p2, p3, p4;
	HPoint2D p1i, p2i, p3i, p4i;
	Point2D aux1, aux2;
	float hor;

	Horizon *_Horizon = Horizon::getInstance();
	hor = (float)_Horizon->getHorizonRow();

	aux1.x = (float)ImageInput::IMG_WIDTH * 0.25f;
	aux1.y = (float)ImageInput::IMG_HEIGHT * 0.25f;
	aux1.h = 1.0f;
	aux2.x = (float)ImageInput::IMG_WIDTH * 0.75f;
	aux2.y = (float)ImageInput::IMG_HEIGHT * 0.25f;
	aux2.h = 1.0f;

	if(aux1.y < hor){
		aux1.y = hor-20;
	}
	if(aux2.y < hor){
		aux2.y = hor-20;
	}

//	cout<<"\thorizonte: "<<hor;

	p1i.x = aux1.x;
	p1i.y = aux1.y;
	p1i.h = aux1.h;

	p2i.x = aux2.x;
	p2i.y = aux2.y;
	p2i.h = aux2.h;

	p3i.x = (float)ImageInput::IMG_WIDTH * 0.25f;
	p3i.y = (float)ImageInput::IMG_HEIGHT;
	p3i.h = 1.0f;

	p4i.x = (float)ImageInput::IMG_WIDTH * 0.75f;
	p4i.y = (float)ImageInput::IMG_HEIGHT;
	p4i.h = 1.0f;

	_Kinematics->get3DPositionZ(p1, p1i, 0.0);
	_Kinematics->get3DPositionZ(p2, p2i, 0.0);
	_Kinematics->get3DPositionZ(p3, p3i, 0.0);
	_Kinematics->get3DPositionZ(p4, p4i, 0.0);
/*
	cout<<"\tP1["<<p1i.x<<","<<p1i.y<<"]["<<p1.X<<","<<p1.Y<<","<<p1.Z<<"]";
	cout<<" P2["<<p2i.x<<","<<p2i.y<<"]["<<p2.X<<","<<p2.Y<<","<<p2.Z<<"]";
	cout<<" P3["<<p3i.x<<","<<p3i.y<<"]["<<p3.X<<","<<p3.Y<<","<<p3.Z<<"]";
	cout<<" P4["<<p4i.x<<","<<p4i.y<<"]["<<p4.X<<","<<p4.Y<<","<<p4.Z<<"]";
	cout<< endl;
*/
	float px[4], py[4];

	px[0] = p1.X / p1.H;
	px[1] = p2.X / p2.H;
	px[2] = p3.X / p3.H;
	px[3] = p4.X / p4.H;
	py[0] = p1.Y / p1.H;
	py[1] = p2.Y / p2.H;
	py[2] = p3.Y / p3.H;
	py[3] = p4.Y / p4.H;

	return (_Kinematics->pointInPolygon(4, px, py, tap->x, tap->y));
}

void AttentionQ::nextElement() {

}

void AttentionQ::setStrategy(string type)
{
	map<string, tbufferapoint*>::iterator iter;

	for (iter = abuffers.begin(); iter != abuffers.end(); ++iter) {

		(*iter).second->client->setStrategy(type);
	}
}

void AttentionQ::updateQuality(tbufferapoint* aux){

	long now;
	float newQual, refQual;

	refQual = getMyQuality(aux->id);
	now = getCurrentTime()/1000;

	if(aux->quality > refQual){
		kQ = 1.0 / 2000;
		upQual = false;
		newQual = aux->quality - ((now - aux->last_set)* kQ);
		if(newQual>0){
			aux->quality = newQual;
		}else{
			aux->quality = 0.0;
		}
	}else if(aux->quality < refQual){
/*
		if(!upQual){
			kQ = (refQual - aux->quality)/ 500.0;
			upQual = true;
		}
*/
		newQual = aux->quality + ((now - aux->last_set)* kQ);
		if(newQual<1){
			aux->quality = newQual;
		}else{
			aux->quality = 1.0;
		}
	}


	//version antigua
/*
	long now;
	float newQual;

	now = getCurrentTime()/1000;
	newQual = aux->quality - ((now - aux->last_set)* 1.0/2000);
	if(newQual>0){
		aux->quality = newQual;
	}else{
		aux->quality = 0.0;
	}

	newQual = getMyQuality(aux->id);
	if(newQual > aux->quality){
		aux->quality = newQual;
	}
*/
}
