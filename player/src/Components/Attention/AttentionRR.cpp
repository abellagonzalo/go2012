#include "AttentionRR.h"
#include "Detector.h"

AttentionRR::AttentionRR():AbstractAttention() {


	_Head = Head::getInstance();
	_Kinematics = Kinematics::getInstance();

	last_jump = getCurrentTime();
	ap_at_image = false;
}

AttentionRR::~AttentionRR() {

}

void AttentionRR::stepcode(void) {

	//remove objects not refreshed by two seconds
	map<string, tbufferapoint*>::iterator iter;
	bool anyactive = false;
	for (iter = abuffers.begin(); iter != abuffers.end(); ++iter)
		if ((*iter).second->enabled) {
			anyactive = true;
			break;
		}

	if (!anyactive)
		return;

	bool removed = false;

	for (iter = abuffers.begin(); iter != abuffers.end(); ++iter) {
		if ((*iter).second->enabled && ((getCurrentTime()
				- (*iter).second->last_set) > TIME_TO_DISABLE_CLIENT)) {
			removed = true;
			(*iter).second->enabled = false;
			(*iter).second->percentages = 0.0;
		}
	}

	if (removed)
		recalculateTimes();

	//cerr<<"1 ["<<(*itabuffer).second->id<<"]"<<endl;
	(*itabuffer).second->client->getAP(&ap_actual);

	long elapsed;

	elapsed = getCurrentTime() - last_jump;

	if ((elapsed > (*itabuffer).second->time) ||  (ap_actual==NULL)){

		if(ap_actual!=NULL)
			ap_actual->time = ap_actual->time + (getCurrentTime()-ap_actual->its);

		nextElement();
		//cerr<<"2 ["<<(*itabuffer).second->id<<"]"<<endl;
		(*itabuffer).second->client->getAP(&ap_actual);
		if(ap_actual==NULL) return;

		ap_actual->its = getCurrentTime();
		last_jump = getCurrentTime();

	}

	/*
	 for (iter = abuffers.begin(); iter != abuffers.end(); ++iter) {
			if ((*iter).second->enabled)
				cerr<<"("<<(*iter).second->id<<":"<<"("<<(*iter).second->percentages<<":"<<(*iter).second->time<<") "<<endl;
	}
	 */
	//cerr<<"  --> ["<<(*itabuffer).second->id<<"] ("<<ap_actual->x<<", "<<ap_actual->y<<")"<<endl;


	ap_actual->time = ap_actual->time + (getCurrentTime()-ap_actual->its);
	ap_actual->its = getCurrentTime();


	double ballpl, goalpl;

	ballpl = 0.0;
	goalpl = 0.0;


	if(!isObservable(ap_actual))
	{
		(*itabuffer).second->client->APnotValid();
		//cerr<<"3 ["<<(*itabuffer).second->id<<"]"<<endl;
		(*itabuffer).second->client->getAP(&ap_actual);

		if(ap_actual==NULL) return;

		ap_actual->its = getCurrentTime();
		last_jump = getCurrentTime();
		ap_actual->time = ap_actual->time + (getCurrentTime()-ap_actual->its);
		ap_actual->its = getCurrentTime();
	}

	if(APReached(ap_actual))
		(*itabuffer).second->client->atAP();

	float pang, tiltg;

	_Kinematics->getNeckAngles(ap_actual->x, ap_actual->y, ap_actual->z, pang, tiltg);

	_Head->setTiltPos(tiltg, 0.6);
	_Head->setPanPos(pang, 0.6);

}

void AttentionRR::step(void) {
	_Head->step();
	_Kinematics->step();

	if (isTime2Run()) {
		startDebugInfo();
		stepcode();
		endDebugInfo();
	}

	_Head->step();
}

void AttentionRR::init(const string newName, AL::ALPtr<AL::ALBroker> parentBroker) {
	Component::init(newName, parentBroker);

	this->setFreqTime(SHORT_RATE);

	cycle = TIME_CYCLE;

	itabuffer = abuffers.begin();
}

void AttentionRR::regist(string id, AttentionClient *client) {
	tbufferapoint *aux = new tbufferapoint;


	aux->client = client;
	aux->time = 0.0;
	aux->percentages = 0.0;
	aux->id = id;
	aux->enabled = false;
	aux->last_set = getCurrentTime();

	abuffers[id] = aux;

	//cout << "resgistRR ="<<id<< endl;

}

void AttentionRR::setPercentage(string id, float percentage) {

	map<string, tbufferapoint*>::iterator iter = abuffers.find(id);
	if (iter == abuffers.end()) {
		return;
	}

	tbufferapoint *aux = abuffers[id];

	if ((aux->percentages != percentage) || (!aux->enabled)) {
		aux->percentages = percentage;

		if (aux->percentages == 0.0)
			aux->enabled = false;
		else
			aux->enabled = true;

		recalculateTimes();
	}

	aux->last_set = getCurrentTime();
}

void AttentionRR::recalculateTimes() {
	float sumpercen = 0.0;
	map<string, tbufferapoint*>::iterator iter;

	for (iter = abuffers.begin(); iter != abuffers.end(); ++iter) {
		if ((*iter).second->enabled) {
			sumpercen = sumpercen + (*iter).second->percentages;
		}
	}

	if (sumpercen > 0.0)
		for (iter = abuffers.begin(); iter != abuffers.end(); ++iter) {
			if ((*iter).second->enabled) {
				(*iter).second->time = (long) ((float) cycle
						* ((*iter).second->percentages / sumpercen));
			}
		}

	if(!(*itabuffer).second->enabled)
		last_jump = getCurrentTime();
}

void AttentionRR::nextElement() {

	unsigned int cont = 0;
	do {
		itabuffer++;
		cont++;
		if (itabuffer == abuffers.end())
			itabuffer = abuffers.begin();
	} while (!(*itabuffer).second->enabled || (cont > abuffers.size()));

}

bool AttentionRR::isObservable(tapoint *tap)
{
	if((tap->x<=-120.0f))
	{

		return false;

		static int p1_num = 8;
		float p1_x[] = {-120.0f, -200.0f, -980.0f, -7840.0f, -7840.0f, -980.0f, -200.0f, -120.0f};
		float p1_y[] = {140.0f, 50.0f, 700.0f, 4480.0f, -4480.0f, -700.0f, -50.0f, -140.0f};

		if(_Kinematics->pointInPolygon(p1_num, p1_x, p1_y, tap->x, tap->y)) return false;
	}

	if((tap->y>100.0f) && (tap->y<1830.0f) && (tap->x<450.0f) && (tap->x>-220.0f))
	{

		static int p2_num = 7;
		float p2_x[] = {160.0f, 190.0f, 450.0f, 350.0f, 110.0f, -220.0f, -120.0f};
		float p2_y[] = {100.0f, 310.0f, 610.0f, 1150.0f, 1830.0f, 450.0f, 140.0f};

		if(_Kinematics->pointInPolygon(p2_num, p2_x, p2_y, tap->x, tap->y)) return false;
	}

	if((tap->y<-100.0f) && (tap->y>-1830.0f) && (tap->x<450.0f) && (tap->x>-220.0f))
	{
		static int p3_num = 7;
		float p3_x[] = {160.0f, 190.0f, 450.0f, 350.0f, 110.0f, -220.0f, -120.0f};
		float p3_y[] = {-100.0f, -310.0f, -610.0f, -1150.0f, -1830.0f, -450.0f, -140.0f};

		if(_Kinematics->pointInPolygon(p3_num, p3_x, p3_y, tap->x, tap->y)) return false;
	}

	if((tap->y<140.0f) && (tap->y>-140.0f) && (tap->x<130.0f) && (tap->x>-120.0f))
	{
		static int p4_num = 4;
		float p4_x[] = {160.0f, -120.0f, -120.0f, 160.0f};
		float p4_y[] = {100.0f, 140.0f, -140.0f, -100.0f};

		if(_Kinematics->pointInPolygon(p4_num, p4_x, p4_y, tap->x, tap->y)) return false;
	}

	return true;
}

bool AttentionRR::APReached(tapoint *tap)
{
	HPoint3D p;

	p.X = tap->x;
	p.Y = tap->y;
	p.Z = 0.0;

	if(_Kinematics->groundPointInImage(&p))
	{
		if(!tap->ap_at_image)
			tap->ts = getCurrentTime();
		tap->ap_at_image = true;
		return true;
	}else{
		if(tap->ap_at_image)
			tap->ts = getCurrentTime();
		tap->ap_at_image = false;
		return false;
	}
}

void AttentionRR::setStrategy(string type)
{
	map<string, tbufferapoint*>::iterator iter;


	for (iter = abuffers.begin(); iter != abuffers.end(); ++iter) {

		(*iter).second->client->setStrategy(type);
	}
}
