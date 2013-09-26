/*
 * Name: W3DDetector.h
 * @Author: Francisco Martín Rico (fmartin@gsyc.es)
 * 
 * Description: Wave3D method
 *
 * Created on: 18/07/2011
 *
 * Copyright (C) Universidad Rey Juan Carlos
 * All Rights Reserved.
 *
 */

#ifndef W3DDetector_H_
#define W3DDetector_H_

#include "Detector.h"
#include "../../lib/estimation/ObjectState.h"
#include "../../lib/estimation/SingleObjectModel.h"
#include "Body.h"
#include "Attention.h"
#include "Perception.h"
#include <IceE/IceE.h>
#include <debug.h>
#include "Debug.h"
#include <componentsI.h>
#include "ImageInput.h"
#include "W3DAttentionClient.h"


class Perception;

class BallSampleW3D : public AbstractSample {
public:

	float ratio;
	float radius;
	float radius_expected;

};

class W3DDetector: public Component, public Singleton<W3DDetector>, public Detector,
public W3DAttentionClient, public DebugIRel, public W3DDetectorManager {
public:
	W3DDetector();
	~W3DDetector();

	void init(const string newName, AL::ALPtr<AL::ALBroker> parentBroker);
	void step();

	void detect(bool debug);
	void detect1(bool debug);

	list<BallSampleW3D> * getBalls() {return &(this->balls);};

	BallSampleW3D *getBall() {if (this->balls.size()>0) return &(*(this->balls.begin())); else return NULL;};

	void drawValidRegions(IplImage* src);
	void drawDetected(IplImage* src);	

	inline ObjectState* getObj() { return &(ballmodel->estimate); };
	//bool seen() { return  this->balls.size()>0;};

	//Attention Client methods
//	void initClient();

	bool isSeen();

	bica::ShapeList getGrDebugRel();
	virtual VisualMemoryObjPtr getVisualMemoryObject(const Ice::Current& c);
	virtual void predictionUpdate(const Ice::Current& c);
	virtual void measurementUpdate(const Ice::Current& c);

	BallModel 		*ballmodel;

private:

	void updateFromObservation();
	void updateFromOdometry();
	void predict();

	/*Validate the balls*/
	void validateBalls();

	/*Obtain the radius of the ball if it's possible*/
	bool getBallRadius(BallRegion* ballRegion, BallSampleW3D &ball);

	/*Save the values of the ball if the ratio is correct*/
	bool saveValues(BallRegion* ballRegion, int col, int row, int sum1, int sum2, BallSampleW3D &ball);
	/*Return true if the ratio is valid for the ball*/
	bool checkBallRatio(int size1, int size2, float &ratio);
	/*Get the expected radius of the ball*/
	void getExpectedRadius(BallSampleW3D &ball);
	/*Check if the difference between radius and expected radius is valid*/
	bool checkRadius(BallSampleW3D &ball);

	/*Clear the samples already created*/
	void clearDetected();

	static const int MIN_BALL_SUM = 3;
	static const int MIN_THRESHOLD = 2;
	static const float MIN_VALID_RATIO;
	static const float BALL_RADIUS;
	static const int MIN_THRESHOLD_RADIUS = 3;
	static const float MIN_VALID_RATIO_RADIUS;
	static const int MIN_DIST_BALLS = 6;
	static const long TIME_SLICE;
	static const float DEFAULT_SP_DENSITY;
	CvMemStorage *storage;
	CvSeqWriter writer;

	Body			*body;
	Perception		*perception;
	Kinematics 		*kinematics;
	float			lastBodyX, lastBodyY, lastBodyTh;
	long 			lastBallTimestamp;
	list<BallSampleW3D> balls;
	bool 			measurement;

	float ir;
	float theta;

	void calculateCameraLimits();
	bool InSegment(HPoint3D p1, HPoint3D p2, HPoint3D pt);
	void getIntersection(HPoint3D center, float radius, HPoint3D p1, HPoint3D p2, HPoint3D &res, HPoint3D &res2, int &intersec);
	void addDbgPoint(HPoint3D p);
	void addIntPoint(HPoint3D p);
	void resetDbgPoints();
	void resetIntPoints();
	float getMaxDistance2Vertex(HPoint3D pt);
	float getMinDistance2Vertex(HPoint3D pt);
	float getMaxDistance2Line(HPoint3D pt);
	float getMinDistance2Line(HPoint3D pt);
	float getDistance2Line(HPoint3D pt, HPoint3D p1, HPoint3D p2);

	bool scan3DPos(HPoint3D scan);
	void resetScan();
	void calculateNewIntPoints();

	HPoint3D center;
	HPoint3D camerap1, camerap2, camerap3, camerap4;
	vector<HPoint3D*> debugPoints;
	vector<HPoint3D*> intPoints;
	pthread_mutex_t mutex;

	bool ballDetected;
	long waitdetect;

	long lastImageTs;
};

#endif /* W3DDetector_H_ */
