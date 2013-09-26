#ifndef REGISTER_H_
#define REGISTER_H_

#include "Singleton.h"
#include "Component.h"
#include <string>

#include <IceE/IceE.h>
#include <componentsI.h>

using namespace bica;

class Component;

using namespace std;

class Register: public Singleton<Register>, public ComponentInfoProvider
{
public:
	Register();
	virtual ~Register();

	void regist(string id, Component *instance);
	Component *getComponent(string id);
	virtual string getDebugData(const string& component, const Ice::Current& c);
private:

	std::map<string, Component*> reg;
};

#endif /* REGISTER_H_ */
