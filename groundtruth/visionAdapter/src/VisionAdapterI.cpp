/*
 * Name: VisionAdapterI.cpp
 * @Author: Gonzalo Abella (abellagonzalo@gmail.com)
 * Updated: 25/01/2011 by Gonzalo Abella
 *
 */

#include "VisionAdapterI.h"

#include <iostream>

using namespace std;

VisionAdapterI::VisionAdapterI() {

	_running = false;

	AdapterPtr adapter( new Adapter() );
	_adapter = adapter;

	IceUtil::TimerPtr timer(new IceUtil::Timer());
	_timer = timer;

	UpdateLocalizationTimerTaskPtr timerTask(new UpdateLocalizationTimerTask(_adapter));
	_timerTask = timerTask;
}



void VisionAdapterI::start(int freq, const Ice::Current&) {
	if (!_running) {

		_adapter->start();

		IceUtil::Time time = IceUtil::Time::milliSeconds(1000/freq);
		_timer->scheduleRepeated(_timerTask, time);

		cout << "VisionAdapter: started at " << freq << "Hz" << endl;
	}
	_running = true;
}


void 
VisionAdapterI::stop(const Ice::Current&) {
	if (_running) {
		_adapter->stop();
		_timer->cancel(_timerTask);
		
		AdapterPtr adapter( new Adapter() );
		_adapter = adapter;

		UpdateLocalizationTimerTaskPtr timerTask(new UpdateLocalizationTimerTask(_adapter));
		_timerTask = timerTask;

		cout << "VisionAdapter: stopped" << endl;
	}
	_running = false;
}

void 
VisionAdapterI::addRobot(const bica::GTRobot& robot, const Ice::Current&) {
	_timerTask->addRobot(robot);
}

void 
VisionAdapterI::removeRobot(const bica::GTRobot& robot, const Ice::Current&) {
	_timerTask->removeRobot(robot);
}

bica::GTRobots 
VisionAdapterI::getRobots(const Ice::Current&) {
	return _timerTask->getRobots();
}


int
main(int argc, char* argv[]) {
	VisionAdapterServer visionAdapterServer;
	return visionAdapterServer.main(argc, argv);
}

