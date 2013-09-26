#include "AttentionP.h"
#include "Detector.h"
#include "GoalDetector.h"
#include "BallDetector.h"
#include "Horizon.h"
#include "../lib/vision/VisionDefinitions.h"

AttentionP::AttentionP() {
	_Head = Head::getInstance();
	_Kinematics = Kinematics::getInstance();
}

AttentionP::~AttentionP() {

}
void AttentionP::initScanPoints()
{
	HPoint3D aux;

//	aux.X = 2000.0;
//	aux.Y = 0.0;
//	aux.Z = 0.0;
//	aux.H = 1.0;
//	scanpoints.push_back(aux);

	aux.X = 2000.0;
	aux.Y = 2500.0;
	aux.Z = 0.0;
	aux.H = 1.0;
	scanpoints.push_back(aux);

	aux.X = 2000.0;
	aux.Y = -2500.0;
	aux.Z = 0.0;
	aux.H = 1.0;
	scanpoints.push_back(aux);

	aux.X = 300.0;
	aux.Y = 200.0;
	aux.Z = 0.0;
	aux.H = 1.0;
	scanpoints.push_back(aux);

	aux.X = 300.0;
	aux.Y = -200.0;
	aux.Z = 0.0;
	aux.H = 1.0;
	scanpoints.push_back(aux);

	aux.X = 2000.0;
	aux.Y = 2500.0;
	aux.Z = 0.0;
	aux.H = 1.0;
	scanpoints.push_back(aux);

	itscanpoints = scanpoints.begin();

	inittime = getCurrentTime();

}

bool AttentionP::APReached(HPoint3D p)
{
	float pang, tiltg, pana, tilta;

	_Kinematics->getNeckAngles(p.X, p.Y, p.Z, pang, tiltg);

	pana = _Head->getPan();
	tilta = _Head->getTilt();


	//cerr<<"("<<fabs(pana-pang)<<") ("<<fabs(tilta-tiltg)<<")"<<endl;
	return ((fabs(pana-pang)<0.05) && (fabs(tilta-tiltg)<0.05));
}

void AttentionP::stepcode(void) {

	float pang, tiltg;

	if(!inited)
	{
		actual = (*itscanpoints);
		_Kinematics->getNeckAngles((*itscanpoints).X, (*itscanpoints).Y, (*itscanpoints).Z, pang, tiltg);

		_Head->setTiltPos(tiltg, 0.1);
		_Head->setPanPos(pang, 0.1);

		inited = true;
	}
	checkStimuli();

	long elapsed = (getCurrentTime() - inittime);

	if(APReached(actual) || (elapsed > MAX_TIME_TO_POINT))
	{

		//cerr<<"elapsed = "<<elapsed<<endl;
		inittime = getCurrentTime();

		//cerr<<"He llegado"<<endl;
		itscanpoints++;

		tapoint *ap_actual;
		abuffers["Ball"]->client->getAP(&ap_actual);

		//cerr<<"ball = "<<abuffers["Ball"]->percentages<<endl;

		if((itscanpoints==scanpoints.end()) && (abuffers["Ball"]->percentages>0.0)  && (ap_actual!=NULL) && (ap_actual->x>0.0))
		{
			//cerr<<"Voy a mirar la bola";

			actual.X = ap_actual->x;
			actual.Y = ap_actual->y;
			actual.Z = ap_actual->z;
			actual.H = 1.0;

			//cerr<<" en ("<<actual.X<<","<<actual.Y<<")"<<endl;
		 }else
		 {
			 if(itscanpoints==scanpoints.end())
				 itscanpoints = scanpoints.begin();

			 actual = (*itscanpoints);
		 }

		_Kinematics->getNeckAngles(actual.X, actual.Y, actual.Z, pang, tiltg);

		_Head->setTiltPos(tiltg, 0.1);
		_Head->setPanPos(pang, 0.1);
	}
}

void AttentionP::setPercentage(string id, float percentage) {

	map<string, tbufferapoint*>::iterator iter = abuffers.find(id);

	if (iter == abuffers.end()) {
		return;
	}

	tbufferapoint *aux = abuffers[id];

	aux->percentages = percentage;
	aux->last_set = getCurrentTime();
}

void AttentionP::checkStimuli(void)
{
	map<string, tbufferapoint*>::iterator iter;

	for (iter = abuffers.begin(); iter != abuffers.end(); ++iter)
		if ((getCurrentTime() - (*iter).second->last_set) > TIME_TO_DISABLE_CLIENT)
			(*iter).second->percentages = 0.0;
}

void AttentionP::step(void) {
	_Head->step();
	_Kinematics->step();

	if (isTime2Run()) {
		stepcode();
	}
	_Head->step();
}

void AttentionP::init(const string newName, AL::ALPtr<AL::ALBroker> parentBroker) {

	Component::init(newName, parentBroker);

	this->setFreqTime(SHORT_RATE);

	inited = false;

	initScanPoints();
}



void AttentionP::regist(string id, AttentionClient *client){

	tbufferapoint *aux = new tbufferapoint;

	aux->client = client;
	aux->time = 0.0;
	aux->percentages = 0.0;
	aux->id = id;
	aux->enabled = false;
//	aux->last_set = getCurrentTime();

	aux->last_set = getCurrentTime();
	aux->last_perc = getCurrentTime();

	abuffers[id] = aux;

	//cerr << "resgistP ="<<id<< endl;
}



void AttentionP::nextElement() {

}

void AttentionP::setStrategy(string type)
{
	map<string, tbufferapoint*>::iterator iter;

	for (iter = abuffers.begin(); iter != abuffers.end(); ++iter) {

		(*iter).second->client->setStrategy(type);
	}
}


