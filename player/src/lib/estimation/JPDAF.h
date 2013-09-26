/*
 * Name: JPDAF.h
 * @Author: Carlos Agüero (caguero@gsyc.es)
 *
 * Description: Class that implements a probabilistic multi-object estimator. The
 * code is mainly based on the paper "People Tracking with a Mobile Robot Using
 * Sample-based Joint Probabilistic Data Association Filters" written by Dirk Schulz
 * et all. This class uses a fixed set of Kalman Filters (as an alternative to the work
 * shown in the paper, in which uses particle filters) and a probabilistic algorithm
 * for associate observations and objects.
 *
 * Created on: 29/03/2011
 *
 * Copyright (C) Universidad Rey Juan Carlos
 * All Rights Reserved.
 *
 */

#ifndef JPDAF_H
#define JPDAF_H

#include <vector>
#include <algorithm>
#include <numeric>
#include "kalman/Kalman.h"
#include "GaussianDistribution2D.h"
#include "Detector.h"
#include "Body.h"
#include "Vector2.h"
#include "Kinematics.h"
#include "Debug.h"
#include "time.h"


using namespace std;

typedef vector<int> Mask;
typedef vector<Mask> Masks;

class JPDAF : public DebugIRel{
public:

	enum tDynamism {
		STATIC_OBJ,
		DYNAMIC_OBJ
	};

	enum tUncertaintyLevel {
		LOW_UNCERTAINTY,
		MED_UNCERTAINTY,
		HIGH_UNCERTAINTY
	};

	enum tOperation {
		PREDICTION,
		ODOMETRY,
		MEASUREMENT
	};

	// Parameters of each KF
	static const float LOW_UNCERTAINTY_COV		= 1.0f;
	static const float MED_UNCERTAINTY_COV		= 20.0f;
	static const float HIGH_UNCERTAINTY_COV		= 60.0f;
	//static const float PROCESS_NOISE_COV = 1.0f;
	static const float PROCESS_NOISE_VEL_COV = 2.0f;
	static const float MEASUREMENT_NOISE_COV = 150.0f;
	static const float MEASUREMENT_NOISE_VEL_COV = 5.0f;
	static const float INITIAL_ERROR_COV_POST = 500.0f;
	static const int INITIAL_MAX_POS_X = 3000;
	static const int INITIAL_MAX_POS_Y = 2000;

	// Parameters for association
	static const float VIRTUAL_OBS_DIST2MEAN = 300.0f;
	static const double BETA_THRESHOLD_1_OBJECT = 0.97;
	static const double BETA_THRESHOLD_2_OBJECT = 0.48;
	static const double BETA_THRESHOLD_4_OBJECT = 0.10;
	//static const float 	MIN_BETA					= 0.0001f;
	//static const float	MAX_BETA					= 0.9999f;

	/*
	 * JPDAF. Class constructor.
	 * @param numObjects Specify the fixed number of objects to track
	 * @param maxNFeatures Specify the maximum number of features by frame. If more than
	 *  maxNFeatures are received in the correction phase, they will be ignored.
	 */
	JPDAF(tDynamism objType, unsigned numObjects = 1, unsigned maxNFeatures = 5);

	/*
	 * ~JPDAF. Class destructor.
	 */
	~JPDAF();

	/*
	 * predict. The JPDAF is based in the classical prediction / correction updates. This
	 * method implements the prediction step.
	 */
	void predict();

	/*
	 * correct. The JPDAF is based in the classical prediction / correction updates. This
	 * method implements the correction step given a list of features.
	 * @param features List of observations for a given frame.
	 */
	void correct(list<AbstractSample> features);

	void correct(AbstractSample feature, int object);

	/*
	 * correctFromOdometry. This method implements the correction step given the odometry information.
	 * All the estimations are rotated, translated and blurred according to the odometry.
	 */
	void correctFromOdometry();

	bica::ShapeList getGrDebugRel();

	vector<bicaKalman*> objects;
	pthread_mutex_t mutex;

private:
	/*
	 * getProbFeatureGivenObject. Method that returns the probability of an object generates
	 * an observation. Basically, the evaluation of the observation in the Gaussian object is returned.
	 * @param measurement The feature modeled as a Gaussian. This paramenter only contains the
	 * mean matrix because we suppose fixed noise (and not taken into the account in this method).
	 * @param object Which one of the objects tracked is gonna be evaluated
	 * @return The value of the evaluation. Higher value means good compatibility between observation
	 * and object.
	 */
	float getProbFeatureGivenObject(const CvMat* measurement, unsigned object);

	/*
	 * updateAssociation. A 2D vector called 'association' is updated in this method. This container
	 * maintains the compatibility among features and objects. Note that the content of each cell
	 * expresses the matching between an observation and an object without taking into account the rest
	 * of features and objects. A special feature is created (z=0) for modeling that an object does
	 * not generate an observation.
	 * @param features The list of features received in this frame
	 * @return The method returns 'false' in case the number of features exceed the maximum number
	 * of features allowed by 'maxNFeatures' attribute, or true in other case.
	 */
	bool updateAssociation(list<AbstractSample> features);

	/*
	 * updateBeta. a 2D vector called 'beta' is updated in this method. This container maintains
	 * the whole compatibility among features and objects. Note that the content of each cell
	 * expresses the matching between an observation and an object taking into account the rest
	 * of features and objects. This is the real output of the association.
	 * @param features The list of features received in this frame
	 * @return The method returns 'false' in case the number of features exceed the maximum number
	 * of features allowed by 'maxNFeatures' attribute, or true in other case.
	 */
	bool updateBeta(list<AbstractSample> features);

	/*
	 * getBeta1. This method calculates the beta value for an specific pair of object/feature,
	 * given the rest of objects and features. This method is used when there is only one object
	 * to track.
	 * @param feature The index of the feature.
	 * @param object The index of the object
	 * @return The value of the beta for the feature / object selected.
	 */
	float getBeta1(unsigned feature, unsigned object,
			list<AbstractSample> features);

	/*
	 * getBeta2. This method calculates the beta value for an specific pair of object/feature,
	 * given the rest of objects and features. This method is used when there are two objects
	 * to track.
	 * @param feature The index of the feature.
	 * @param object The index of the object
	 * @return The value of the beta for the feature / object selected.
	 */
	float getBeta2(unsigned feature, unsigned object,
			list<AbstractSample> features);

	/*
	 * getBeta4. This method calculates the beta value for an specific pair of object/feature,
	 * given the rest of objects and features. This method is used when there are four objects
	 * to track.
	 * @param feature The index of the feature.
	 * @param object The index of the object
	 * @return The value of the beta for the feature / object selected.
	 */
	float getBeta4(unsigned feature, unsigned object,
			list<AbstractSample> features);

	void updateProcessNoiseMatrix (tOperation op);

	/*
	 * reset. Method that sets all the cells of a container to value 0.0 .
	 * @param v Container to reset.
	 */
	void reset(vector<vector<double> > &v);

	/*
	 * normalize. Method that normalizes a container for summing to one.
	 * @param v Container to normalize.
	 */
	void normalize(vector<vector<double> > &v, bool first_row = true);

	/*
	 * print. Method that prints a container.
	 * @param label Label to be displayed next to the value of each cell.
	 * @param v Container to print.
	 */
	void print(const string label, const vector<vector<double> > v);


	tDynamism objType;
	unsigned _numObjects;
	unsigned maxNFeatures;
	unsigned currentNFeatures;
	list<AbstractSample> currentFeatures;
	float lastBodyX, lastBodyY, lastBodyTh;
	vector<vector<double> > association;
	vector<vector<double> > beta;
	CvRandState rng;
	Body *body;
	Kinematics *kinematics;
};

#endif
