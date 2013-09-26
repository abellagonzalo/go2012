/*
 * Name: RobotUSDetector.h
 * @Author: Alejandro Aporta (a.aporta@alumnos.urjc.es)
 * 
 * 
 * Description: Class to detect other robots
 *
 * Created on: 04/11/2011
 *
 * Copyright (C) Universidad Rey Juan Carlos
 * All Rights Reserved.
 *
 */

#ifndef RobotUSDetector_H_
#define RobotUSDetector_H_

#include "Component.h"
#include "Singleton.h"
#include "Attention.h"
#include "Body.h"
#include "BallDetector.h"
#include "alproxies/almotionproxy.h"
#include "alproxies/almemoryproxy.h"
#include "Matrix.h"
//#include "Detector.h"
/*#include "../../lib/estimation/ObjectState.h"
#include "../../lib/estimation/GoalsModel.h"
#include "Body.h"
#include "Attention.h"
#include "Segmentation.h"
#include "Horizon.h"
#include "Perception.h"
#include "../../lib/vision/ImageInput.h"
#include <componentsI.h>
#include "RobotLog.h"
#include "GTLocalization.h"
*/


class RobotUSDetector: public Component, public Singleton<RobotUSDetector>{
public:
	RobotUSDetector();
	~RobotUSDetector();

	void init(const string newName, AL::ALPtr<AL::ALBroker> parentBroker);
	void step();
	//void getObsVector_USSonar2(double &xpos, double &ypos, double &zpos);
	void dowork(void);

	void getValues(HPoint2D &rleft, HPoint2D &rright);

private:

	static const int INITIAL_STATE	= 0;
	static const int TEST_STATE = 1;

	double sonar_value_right;
	double sonar_value_left;

	Body			*body;
	Attention		*attention;
	BallDetector	*ballDetector;
	//Determina el estado actual de la máquina de estados
	int current_state;

	void initial_state_code();
	void test_state_code();
/*	void detect(bool debug);


	virtual void drawValidRegions(IplImage* src) = 0;
	virtual void drawDetected(IplImage* src) = 0;	
*/

	AL::ALPtr<AL::ALMemoryProxy> pmemory;
	AL::ALPtr<AL::ALMotionProxy> pmotion;
	
	HPoint2D left, right;

	vector<float> torsoRT;
};

#endif /* RobotUSDetector_H_ */

