#include "Speech.h"

Speech::Speech() {
}

Speech::~Speech() {
}

void
Speech::init(const string newName, AL::ALPtr<AL::ALBroker> parentBroker)
{
	try{
		this->paudio = AL::ALPtr<AL::ALTextToSpeechProxy>(new AL::ALTextToSpeechProxy(parentBroker));
	}catch( AL::ALError& e)
	{
		cerr<<"[Speech::init] : "<<e.toString()<<endl;
	}
}

void
Speech::say(string msg)
{
	this->paudio->say(msg);
}

