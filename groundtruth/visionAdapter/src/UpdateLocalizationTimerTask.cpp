/*
 * Name: UpdateLocalizationTimerTask.cpp
 * @Author: Gonzalo Abella (abellagonzalo@gmail.com)
 * Updated: 25/01/2011 by Gonzalo Abella
 *
 */

#include "UpdateLocalizationTimerTask.h"
#include "localizationI.h"
#include "componentsI.h"

void 
UpdateLocalizationTimerTask::runTimerTask() {
	IceUtil::Mutex::Lock lock(_mutex);
	const SSL_DetectionBall& ball = _adapter->getBall();
	bica::GTRobots robotsToRemove;
	
	for (int i=0; i<robots.size(); i++) {
		try {
			// Set ball position
			if (robots[i].ballDetector != NULL) {
				robots[i].ballDetector->setGTBallAbs(ball.x()+1000, ball.y());
			}

			// Set blue robot position
			if ((robots[i].team == "blue") && (_adapter->hasBlueRobot(robots[i].patternId))) {
				const SSL_DetectionRobot& robot = _adapter->getBlueRobot(robots[i].patternId);
				if (robots[i].localization != NULL) {
					robots[i].localization->setRealPosition(robot.x()+1000, 
															-robot.y(), 
															robot.orientation());
				}

			// Set yellow robot position
			} else if ((robots[i].team == "yellow") && (_adapter->hasYellowRobot(robots[i].patternId))) {
				const SSL_DetectionRobot& robot = _adapter->getYellowRobot(robots[i].patternId);
				if (robots[i].localization != NULL) {
					robots[i].localization->setRealPosition(robot.x()+1000, 
															-robot.y(), 
															robot.orientation());
				}
			}

		} catch (const Ice::Exception& ex) {
			cout << "VisionAdapter: robot " << robots[i].robot << " is removed because " << ex << endl;
			robotsToRemove.push_back(robots[i]);
		}
	}

	// Remove robots who has had an exception
	while (!robotsToRemove.empty()) {
		bica::GTRobot robot = robotsToRemove.back();
		robotsToRemove.pop_back();
		for (int i=0; i<robots.size(); i++) {
			if ((robot.patternId == robots[i].patternId) &&
				(robot.team == robots[i].team)) {
				robots.erase(robots.begin()+i);

				return;
			}
		}
	}
}


void 
UpdateLocalizationTimerTask::addRobot(bica::GTRobot robot) {
	IceUtil::Mutex::Lock lock(_mutex);
	robots.push_back(robot);
	cout << "VisionAdapter: added robot " << robot.robot << endl;
}


void 
UpdateLocalizationTimerTask::removeRobot(bica::GTRobot robot) {
	IceUtil::Mutex::Lock lock(_mutex);
	for (int i=0; i<robots.size(); i++) {
		if ((robot.patternId == robots[i].patternId) &&
			(robot.team == robots[i].team)) {
			robots.erase(robots.begin()+i);
			cout << "VisionAdapter: removed robot " << robot.robot << endl;
			return;
		}
	}
}


bica::GTRobots 
UpdateLocalizationTimerTask::getRobots() {
	IceUtil::Mutex::Lock lock(_mutex);
	return robots;
}


