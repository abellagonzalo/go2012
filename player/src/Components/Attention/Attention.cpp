#include "Attention.h"
#include "Detector.h"

Attention::Attention() {

	_AttentionRR = AttentionRR::getInstance();
	_AttentionQ = AttentionQ::getInstance();
	_AttentionP = AttentionP::getInstance();

	this->currentAttention = _AttentionRR;
}

Attention::~Attention() {

}

void Attention::setAttention(string type){

	if(type.compare("RR") == 0)
	{
		this->currentAttention = _AttentionRR;

	}

	else if(type.compare("Q") == 0)
	{
		this->currentAttention = _AttentionQ;
	}

	else if(type.compare("P") == 0)
	{
		this->currentAttention = _AttentionP;
	}

	this->currentAttention->setStrategy(type);
	cout<<"estrategia: "<<type<<endl;

}

void Attention::step(void) {

	this->currentAttention->step();

}

void Attention::init(const string newName, AL::ALPtr<AL::ALBroker> parentBroker) {
	Component::init(newName, parentBroker);

	_AttentionRR->init("RR", parentBroker);

	_AttentionQ->init("Q", parentBroker);

	_AttentionP->init("P", parentBroker);

	this->currentAttention = _AttentionRR; //default

}

void Attention::regist(string id, AttentionClient *client) {

	_AttentionQ->regist(id, client);
	_AttentionRR->regist(id, client);
	_AttentionP->regist(id, client);

//	cout<<"registro "<<id<<endl;
}


void Attention::setPercentage(string id, float percentage) {

	this->currentAttention->setPercentage(id, percentage);
}


void Attention::nextElement() {

	this->currentAttention->nextElement();

}


