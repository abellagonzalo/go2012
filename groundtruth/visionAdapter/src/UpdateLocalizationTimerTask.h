/*
 * Name: UpdateLocalizationTimerTask.h
 * @Author: Gonzalo Abella (abellagonzalo@gmail.com)
 * Updated: 25/01/2011 by Gonzalo Abella
 *
 */

#ifndef UpdateLocalizationTimerTask_M
#define UpdateLocalizationTimerTask_M

#include <Ice/Ice.h>
#include <IceUtil/Timer.h>
#include <IceUtil/Mutex.h>
#include "VisionAdapter.h"
#include "Adapter.h"
#include <map>

class UpdateLocalizationTimerTask : public IceUtil::TimerTask {
public:
	UpdateLocalizationTimerTask(AdapterPtr adapter) { _adapter = adapter; };
	virtual void runTimerTask();
	void addRobot(bica::GTRobot robot);
	void removeRobot(bica::GTRobot robot);
	bica::GTRobots getRobots();

private:
	AdapterPtr _adapter;
	bica::GTRobots robots;
	IceUtil::Mutex _mutex;
};

typedef IceUtil::Handle<UpdateLocalizationTimerTask> UpdateLocalizationTimerTaskPtr;

#endif
