/*
 * Name: VisionAdapterI.h
 * @Author: Gonzalo Abella (abellagonzalo@gmail.com)
 * Updated: 25/01/2011 by Gonzalo Abella
 *
 */

#ifndef VisionAdapterI_M
#define VisionAdatperI_M

#include "VisionAdapter.h"
#include <Ice/Ice.h>
#include <iostream>
#include "Adapter.h"
#include "localizationI.h"
#include "componentsI.h"
#include "UpdateLocalizationTimerTask.h"

class VisionAdapterI : public bica::VisionAdapter {

public:
	VisionAdapterI();
	void start(int freq, const Ice::Current&);
	void stop(const Ice::Current&);
	void addRobot(const bica::GTRobot& robot, const Ice::Current&);
	void removeRobot(const bica::GTRobot& robot, const Ice::Current&);
	bica::GTRobots getRobots(const Ice::Current&);

private:
	AdapterPtr						_adapter;
	IceUtil::TimerPtr				_timer;
	UpdateLocalizationTimerTaskPtr	_timerTask;
	bool							_running;

};

typedef IceUtil::Handle<VisionAdapterI> VisionAdapterIPtr;



class VisionAdapterServer : public Ice::Application {
public:

	int run(int argc, char*argv[]) {
		
		// Create object adapter
		Ice::ObjectAdapterPtr adapter = communicator()->createObjectAdapterWithEndpoints("VisionAdapterAdapter", "tcp -p 11111");
		adapter->activate();

		// Add VisionAdapter servant
		VisionAdapterIPtr visionAdapter = new VisionAdapterI();
		adapter->add(visionAdapter, communicator()->stringToIdentity("VisionAdapter"));

		// Catch signals
		shutdownOnInterrupt();
		communicator()->waitForShutdown();
		if (interrupted()) {
            std::cerr << appName() << ": terminating" << std::endl;
		}
	}

};

#endif
