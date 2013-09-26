#include "Register.h"

Register::Register()
{
	reg.clear();
}

Register::~Register()
{
	
}

void 
Register::regist(string id, Component *instance)
{
	reg[id] = instance;
}

Component *
Register::getComponent(string id)
{
	return reg[id];
}

string
Register::getDebugData(const string& component, const Ice::Current& c)
{
	Component *comp = getComponent(component);

	std::stringstream out;
	//ToDo: Trigger exception if the component is not found
	if (comp != NULL) {
		out << comp->getRealFreq() << ":" << comp->getMeanCycleTime() << ":"
			<< comp->getMaxCycleTime() << ":" << comp->getMinCycleTime() << ":" << endl;
	}
	return out.str();
}
