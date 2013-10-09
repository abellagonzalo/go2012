/*
 * Name: GoalDetector.h
 * @Author: Eduardo Perdices (eperdices@gsyc.es)
 * 
 * Description: Class to detect goals from the GoalSegments
 *
 * Created on: 31/05/2010
 *
 * Copyright (C) Universidad Rey Juan Carlos
 * All Rights Reserved.
 *
 */

#ifndef GOALDETECTOR_H_
#define GOALDETECTOR_H_

#include "Detector.h"
#include "../../lib/estimation/ObjectState.h"
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
#include "GoalAttentionClient.h"


using namespace goalDescription;

class Perception;

class GoalDetector: public Component, public Singleton<GoalDetector>,
public DebugIRel, public Detector, public GoalAttentionClient, public GoalDetectorManager, public LoggableI {

public:
	GoalDetector();
	~GoalDetector();

	void init(const string newName, AL::ALPtr<AL::ALBroker> parentBroker);
	void step();

	void detect(bool debug);

	GoalSample * getGoal() {return &(this->goal);};

	void drawValidRegions(IplImage* src);
	void drawDetected(IplImage* src);	

	//ToDo: A fuego portería azul. Comprobar para qué se usa este método.
	inline GoalsModel* getObj() { return goals; };
	bool isSeen();


	float getObjX();
	float getObjY();

	bica::ShapeList getGrDebugRel();

	virtual VisualMemoryObjPtr getVisualMemoryObject(const string &obj, const Ice::Current& c);
	virtual void predictionUpdate(const Ice::Current& c);
	virtual void measurementUpdate(const Ice::Current& c);

	// Ground truth
	void setGTActivated();
	void getGTOppNetCenter( double &x, double &y );
	void getGTOppNetLeftPost( double &x, double &y );
	void getGTOppNetRightPost( double &x, double &y );
	void getGTOwnNetCenter( double &x, double &y );
	void getGTOwnNetLeftPost( double &x, double &y );
	void getGTOwnNetRightPost( double &x, double &y );

	GoalsModel 		*goals;


private:

	/*Search the goal of a color and return the type of goal found*/
	int searchGoal(int color, GoalPostSample * posts);
	/*Check if a post is valid*/
	bool checkGoalPost(GoalPostSample &post);
	/*Calc the position of the post in 3D*/
	void calcPostPosition(GoalPostSample &post);
	/*Get the expected width of the post*/
	int getExpectedPostWidth(GoalPostSample &post);
	/*Get the expected height of the post*/
	int getExpectedPostHeight(GoalPostSample &post);
	/*Check the green down the post*/
	int hasPostGreen(GoalPostSample &post, int expWidth);
	/*Search crossbar and return the type*/
	int hasCrossbar(GoalPostSample &post);

	/*Save the goal from the posts*/
	void saveGoal(GoalPostSample * post1, GoalPostSample * post2);

	/*Clear the samples already created*/
	void clearDetected();

	static const int MAX_SEGMENT_SIZE_GOAL = 100;	
	static const int MIN_GOAL_ACCS = 3;
	static const int MIN_POSTS_DISTANCE = 70;
	static const int MAX_ROWS_WITHOUT_POST = 3;
	static const float MIN_POST_DENSITY;
	static const float MIN_ACCEPTED_WIDTH_DENSITY;
	static const int TOP_POST_FOUND_MARGIN = 5;
	static const int NET_MIN_DIST_BASE_TO_HORIZON = 10;
	static const float POST_HEIGHT;
	static const float POST_RADIUS;
	static const int MAX_GREEN_POST_DIST = 15;
	static const float MAX_POST_DISTANCE;

	static const int CROSSBAR_MARGIN_Y = 30;
	static const int CROSSBAR_WIDTH_JUMPS = 3;
	static const int MAX_CROSSBAR_SEGMENT_SIZE = 50;	

	GoalPostSample postsBlue[2];
	GoalPostSample postsYellow[2];

	GoalSample goal;

	Segmentation * _Segmentation;
	Horizon * _Horizon;

	Perception		*perception;
	Kinematics 		*kinematics;
	Body			*body;
	GTLocalization *_GTLocalization;

	HPoint3D 		lastposp3d;

	float			lastBodyX, lastBodyY, lastBodyTh;
	long 			lastBallTimestamp;

	int color2;

	bool 			_gtReceived;

	FILE *goalVMlog;
	long vmIters;
	long double vmMeanCycleTime;
};

#endif /* GOALDETECTOR_H_ */
