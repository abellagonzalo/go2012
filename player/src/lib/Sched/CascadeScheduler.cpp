#include "CascadeScheduler.h"

pthread_t CascadeScheduler::t1;
pthread_mutex_t CascadeScheduler::mutex;
bool CascadeScheduler::running;

void*
CascadeScheduler::runThread(void *data) {
	vector<Component*>::iterator it;
	vector<Component*> *comps = (vector<Component*>*) data;

	while (running) {
		pthread_mutex_lock(&mutex);

		if (comps->size() == 0)
			running = false;

		for (it = comps->begin(); it != comps->end(); it++) {
			(*it)->step();
		}
		pthread_mutex_unlock(&mutex);
	}

	pthread_exit( NULL);
}

CascadeScheduler::CascadeScheduler() {
	running = false;
	head = Head::getInstance();
	body = Body::getInstance();

	pthread_mutex_init(&mutex, NULL);
	running = false;

}

CascadeScheduler::~CascadeScheduler() {
	components.clear();
	running = false;
}

void CascadeScheduler::init() {

}

void
CascadeScheduler::run( const string& component, const Ice::Current& c)
{
	run (component);
}

void
CascadeScheduler::stop( const string& component, const Ice::Current& c)
{
	stop (component);
}

void CascadeScheduler::run(const string component) {
	pthread_mutex_lock(&mutex);

	cerr << "Se desea activar: [" << component << "]" << endl;

	Component *comp = Register::getInstance()->getComponent(component);
	if(comp!=NULL)
		addToSched(comp);

	if (!running) {
		running = true;
		pthread_create(&t1, NULL, CascadeScheduler::runThread, (void*) &(this->components));
	}

	pthread_mutex_unlock(&mutex);
}

void CascadeScheduler::stop(const string component) {
	cerr << "Se desea desactivar: [" << component << "]" << endl;
	pthread_mutex_lock(&mutex);

	Component *comp = Register::getInstance()->getComponent(component);
	if(comp!=NULL)
		removeFromSched(comp);
	else
		cerr << "El componente " << component << " no esta activo" << endl;
	
	pthread_mutex_unlock(&mutex);
}

void CascadeScheduler::removeFromSched(Component *comp) {
	vector<Component*>::iterator it;
	for (it = components.begin(); it != components.end(); it++) {
		if ((*it) == comp) {
			if (components.size() == 1) {
				body->stop();
				head->stop();
				//usleep(2000000);
				//body->step();
				//head->step();
				components.clear();
				break;
			} else {
				components.erase(it);
				break;
			}
		}
	}
}

void CascadeScheduler::addToSched(Component *comp) {
	bool esta = false;
	vector<Component*>::iterator it;
	for (it = components.begin(); it != components.end(); it++)
		if ((*it) == comp)
			esta = true;

	if (!esta)
		components.push_back(comp);

}

