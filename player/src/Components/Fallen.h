#ifndef Fallen_H
#define Fallen_H

#include "Component.h"
#include "alproxies/almemoryproxy.h"
#include "Singleton.h"

class Fallen : public Component, public Singleton<Fallen>
{
public:

	Fallen();
	~Fallen();

	void init(const string newName, AL::ALPtr<AL::ALBroker> parentBroker);
	void step();
	void setActive(bool act) {active = act;};
	string getFallen();
private:

	static const int FALLT = 4;

	AL::ALPtr<AL::ALMemoryProxy> pmemory;
	int fallcount;
	string pos;
	bool active;
};

#endif
