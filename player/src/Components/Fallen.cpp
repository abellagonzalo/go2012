#include "Fallen.h"

Fallen::Fallen()
{
	fallcount = 0;
	pos = string("Stand");
	active = true;
	//writeLog("Created\n");

}

Fallen::~Fallen()
{

}

void
Fallen::init(const string newName, AL::ALPtr<AL::ALBroker> parentBroker)
{
	Component::init(newName, parentBroker);
	//writeLog("Inited");
	//initLog("fallen");
	setFreqTime(LONG_RATE);
	try
	{
		pmemory = getParentBroker()->getMemoryProxy();
	}catch( AL::ALError& e )
	{
		cerr<<"Fallen::init [pmemory]"<<e.toString()<<endl;
	}
}

string
Fallen::getFallen()
{

	//s<<"AAAA ";
	if((fallcount >= FALLT) && active)
	{
		//cerr<<fallcount<<"--->"<<pos<<endl;

		return pos;
	}
	else
	{
		//cerr<<fallcount<<"--->up"<<endl;

		return string("Stand");
	}
	//writeLog(s.str());

}

void
Fallen::step(void)
{

	if (!isTime2Run())
		return;

	//startDebugInfo();
	//ostringstream s;
	try{
		pos = (string)(pmemory->getData(string("robotPoseChanged"), 1));
	} catch (ALError& e)
	{
		cerr << "Fallen::step(void)" << e.toString() << endl;
	}

	//cerr<<"("<<pos<<") ";

	//Todo: Mirar las posiciones

	if(((pos == string("Belly")) || (pos == string("Back"))) && active)
		fallcount++;
	else
	{
		fallcount = 0;
		/*s<<"Reinit porque pos es "<<pos;
		if(active)
			s<<" y está activo";
		else
			s<<" y está inactivo";*/
	}


	//s<<pos<<": "<<fallcount<<endl;

	//writeLog(s.str());

	//endDebugInfo();
}
