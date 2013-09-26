/*
 * Name: RobotDetector.h
 * @Author: Gonzalo Abella (abellagonzalo@gmail.com)
 * 
 * Description: Class to detect robots from the regions
 *
 * Created on: 02/03/2012
 *
 * Copyright (C) Universidad Rey Juan Carlos
 * All Rights Reserved.
 *
 */

#ifndef ROBOTDETECTOR_H_
#define ROBOTDETECTOR_H_

#include "Detector.h"
#include "Body.h"
#include "Perception.h"
#include "RobotModel.h"
#include "GameController.h"

class RobotDetector: public Component, public Singleton<RobotDetector>, public Detector {
public:
	RobotDetector();
	~RobotDetector();

	void init(const string newName, AL::ALPtr<AL::ALBroker> parentBroker);
	void step();

	void detect(bool debug);
	void drawValidRegions(IplImage* src);
	void drawDetected(IplImage* src);	

	list<AbstractSample> *getTeammates() { return &(this->teammates); };
	list<AbstractSample> *getOpponents() { return &(this->opponents); };

	RobotModel 		*robots;

private:

	int _teammateColor;
	int _opponentColor;

	list<AbstractSample> teammates;
	list<AbstractSample> opponents;

	static const float MIN_MARKER_HEIGHT_MM = 100;//200;
	static const float MAX_MARKER_HEIGHT_MM = 400;//300
	static const int ROBOT_HEIGHT = 500;
	static const int ROBOT_WIDTH = 200;

	bool checkMarkerHeight(HPoint3D groundPoint, HPoint2D massCenter);
	void drawRobot(IplImage* src, HPoint3D center, int color);

	Body			*body;
	Perception		*perception;
	Kinematics 		*kinematics;
};

#endif /* ROBOTDETECTOR_H_ */
