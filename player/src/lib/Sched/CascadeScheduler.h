#ifndef CascadeScheduler_H
#define CascadeScheduler_H

#include "SchedulerBase.h"

#include "Component.h"
#include "Head.h"
#include "Body.h"

#include "pthread.h"
#include <vector>

#include <IceE/IceE.h>
#include <schedulerManager.h>

using namespace std;
using namespace bica;

class CascadeScheduler: public Singleton<CascadeScheduler>, public SchedulerBase, public SchedulerManager
{
public:

	CascadeScheduler();
	~CascadeScheduler();

	void init();
	void run(const string component = "Default");
	void stop(const string component = "Default");
	vector<Component*> getComponents() { return components;};

	virtual void run( const string& component, const Ice::Current& c);
	virtual void stop( const string& component, const Ice::Current& c);


private:
	vector<Component*>  components;

	void removeFromSched(Component *comp);
	void addToSched(Component *comp);

	Body *body;
	Head *head;

	static void* runThread(void *data);
	static pthread_t t1;
	static pthread_mutex_t mutex;
	static bool running;
};

#endif
