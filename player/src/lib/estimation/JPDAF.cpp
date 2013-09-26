/*
 * Name: JPDAF.cpp
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

#include "JPDAF.h"

/*
 * JPDAF. Class constructor.
 * @param numObjects Specify the fixed number of objects to track
 * @param maxNFeatures Specify the maximum number of features by frame. If more than
 *  maxNFeatures are received in the correction phase, they will be ignored.
 */
JPDAF::JPDAF(tDynamism objType, unsigned numObjects, unsigned maxNFeatures)
{
	body = Body::getInstance();
	kinematics = Kinematics::getInstance();

	this->objType = objType;

	pthread_mutex_init(&mutex, NULL);

	// Initialize random number generator
	srand ( time(NULL) );
	//cvRandInit( &rng, 0, 20000, -1, CV_RAND_UNI );

	this->_numObjects = numObjects;

	// Add an extra feature for including false negatives
	this->maxNFeatures = maxNFeatures + 1;

	// Resize 'association and 'beta' to accommodate the [maxFeatures + 1][numObjects] array
	this->association.resize(this->maxNFeatures); // maxFeatures rows
	this->beta.resize(this->maxNFeatures);
	for (unsigned i = 0; i < this->maxNFeatures; i++){
		this->association[i].resize(this->_numObjects); // maxObjects columns
		this->beta[i].resize(this->_numObjects);
	}

	// Initialize a Kalman filter for each object to estimate
	this->objects.resize(this->_numObjects);
	for (unsigned i = 0; i < this->_numObjects; i++){
		this->objects[i] = bicaCreateKalman( 4, 4, 0 );

		// Transition matrix 'F' or 'A' describes relationship between model parameters at step k
		// and at step k+1 (this is the "dynamics" in our model).
		//
		//	|1	0	dt	0 |	  |x	|	|x+velX*dt	|
		//	|0	1	0	dt|   |y	|	|y+velY*dt	|
		//	|0	0	1	0 |   |velX	|	|velX	  	|
		//	|0	0	0	1 | * |velY	|= 	|velY		|
		//
		float dt = 0.0f;
		/*const float F[] = { 1, 0, dt, 0,
				0, 1, 0, dt,
				0, 0, 1, 0,
				0, 0, 0, 1 };*/
		const float F[] = { 1, 0, 0, 0,
				0, 1, 0, 0,
				0, 0, 0, 0,
				0, 0, 0, 0 };

		memcpy( objects[i]->transition_matrix->data.fl, F, sizeof(F));

		// Initialize other Kalman filter parameters.
		cvSetIdentity( objects[i]->measurement_matrix, cvRealScalar(1) );
		cvSetIdentity( objects[i]->process_noise_cov, cvRealScalar(MED_UNCERTAINTY_COV * MED_UNCERTAINTY_COV) );
		cvmSet(objects[i]->process_noise_cov, 2, 2, PROCESS_NOISE_VEL_COV * PROCESS_NOISE_VEL_COV);
		cvmSet(objects[i]->process_noise_cov, 3, 3, PROCESS_NOISE_VEL_COV * PROCESS_NOISE_VEL_COV); // 2 mm/seg
		cvSetIdentity( objects[i]->measurement_noise_cov, cvRealScalar(MEASUREMENT_NOISE_COV * MEASUREMENT_NOISE_COV) );
		cvmSet(objects[i]->measurement_noise_cov, 2, 2, MEASUREMENT_NOISE_VEL_COV * MEASUREMENT_NOISE_VEL_COV); // 5 mm/seg
		cvmSet(objects[i]->measurement_noise_cov, 3, 3, MEASUREMENT_NOISE_VEL_COV * MEASUREMENT_NOISE_VEL_COV); // 5 mm/seg
		cvSetIdentity( objects[i]->error_cov_post, cvRealScalar(INITIAL_ERROR_COV_POST * INITIAL_ERROR_COV_POST));
		cvmSet(objects[i]->error_cov_post, 2, 2, 0.0f);
		cvmSet(objects[i]->error_cov_post, 3, 3, 0.0f);

		// Initialize initial state
		cvZero( objects[i]->state_post );
		cvmSet(objects[i]->state_post, 0, 0, (rand() % INITIAL_MAX_POS_X * 2) - INITIAL_MAX_POS_X);
		//cvmSet(objects[i]->state_post, 0, 0, -1 * (rand() % INITIAL_MAX_POS_X));
		cvmSet(objects[i]->state_post, 1, 0, (rand() % INITIAL_MAX_POS_Y * 2) - INITIAL_MAX_POS_Y);
	}
}

/*
 * reset. Method that sets all the cells of a container to value 0.0 .
 * @param v Container to reset.
 */
void
JPDAF::reset( vector<vector<double> > &v )
{
	for (unsigned i = 0; i < v.size(); i++)
		fill ( v[i].begin(), v[i].end(), 0.0 );
}

/*
 * normalize. Method that normalizes a container for summing to one.
 * @param v Container to normalize.
 */
void
JPDAF::normalize( vector<vector<double> > &v, bool first_row )
{
	double total = 0.0;
	unsigned starting;

	if (first_row)
		starting = 0;
	else
		starting = 1;

	for (unsigned i = starting; i < v.size(); i++) {
		total += std::accumulate ( v[i].begin(), v[i].end(), 0.0 );
	}

	if (total != 0.0) {
		for (unsigned i = starting; i < v.size(); i++)
			for (unsigned j = 0; j < v[i].size(); j++) {
				v[i][j] /= total;
			}
	}
}

/*
 * print. Method that prints a container.
 * @param label Label to be displayed next to the value of each cell.
 * @param v Container to print.
 */
void
JPDAF::print(const string label, const vector<vector<double> > v )
{
	cout.precision(5);
	for (unsigned j = 0; j < this->maxNFeatures; j++ ) {   // Features
		for (unsigned i = 0; i < this->_numObjects; i++ ) { // Objects
			if (j < this->currentNFeatures)
				cerr << begin_green << label << " (" << j << "," << i << ") = " << v[j][i] << " " << end_color;
			else
				cerr << begin_red << label << " (" << j << "," << i << ") = " << v[j][i] << " " << end_color;

		}
		cerr << endl;
	}
	cerr << "----\n";
}


/*
 * getProbFeatureGivenObject. Method that returns the probability of an object generates
 * an observation. Basically, the evaluation of the observation in the Gaussian object is returned.
 * @param measurement The feature modeled as a Gaussian. This paramenter only contains the
 * mean matrix because we suppose fixed noise (and not taken into the account in this method).
 * @param object Which one of the objects tracked is gonna be evaluated
 * @return The value of the evaluation. Higher value means good compatibility between observation
 * and object.
 */
float
JPDAF::getProbFeatureGivenObject( const CvMat* measurement, unsigned i )
{
	GaussianDistribution2D gauss;

	// Set the parameters of the Gaussian (same as object i)
	gauss.mean.x = cvmGet(objects[i]->state_post, 0, 0);
	gauss.mean.y = cvmGet(objects[i]->state_post, 1, 0);
	gauss.covariance[0][0] = cvmGet(objects[i]->error_cov_post, 0, 0);
	gauss.covariance[0][1] = cvmGet(objects[i]->error_cov_post, 0, 1);
	gauss.covariance[1][0] = cvmGet(objects[i]->error_cov_post, 1, 0);
	gauss.covariance[1][1] = cvmGet(objects[i]->error_cov_post, 1, 1);

	// Evaluate the measurement at the Gaussian (only position)
	Vector2D_BH<double> feature2evaluate(cvmGet(measurement, 0, 0), cvmGet(measurement, 1, 0));
	return std::max(gauss.probabilityAt(feature2evaluate), pow(10, -40));
}


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
bool
JPDAF::updateAssociation(list<AbstractSample> features)
{
	// Crop uncertainties
	/*for (unsigned i = i; i < this->_numObjects; i++) {
		float uncert = cvmGet(objects[i]->error_cov_post, 0, 0);
		uncert = std::max (uncert, 250.0f);
		cvmSet(objects[i]->error_cov_post, 0, 0, uncert);

		uncert = cvmGet(objects[i]->error_cov_post, 1, 1);
		uncert = std::max (uncert, 250.0f);
		cvmSet(objects[i]->error_cov_post, 1, 1, uncert);
	}*/

	// If number of features is greater than maxNfeatures just ignore them
	if (features.size() > this->maxNFeatures - 1) return false;

	CvMat *z_k = cvCreateMat( 4, 1, CV_32FC1 );
	cvZero( z_k );
	list<AbstractSample>::iterator it = features.begin();

	reset(association);	// Set to 0.0 all values

	for (unsigned j = 0; j < features.size() + 1; j++ ) { // Features
		for (unsigned i = 0; i < this->_numObjects; i++ ) { // Objects
			if (j == 0) { // Evaluate extra measurement for false negative

				// Frustum
				vector<HPoint3D> frustumCorners(4);
				kinematics->getFrustum (frustumCorners[0], frustumCorners[1], frustumCorners[3], frustumCorners[2]);
				float vertx[4];
				float verty[4];
				for (unsigned k = 0; k < 3; k++) {
					vertx[k] = frustumCorners[k].X;
					verty[k] = frustumCorners[k].Y;
				}
				vertx[3] = frustumCorners[3].X;
				verty[3] = frustumCorners[3].Y;

				// Center of the frustum
				float frustumCenterX = (vertx[0] + vertx[1] + vertx[2] + vertx[3]) / 4;
				float frustumCenterY = (verty[0] + verty[1] + verty[2] + verty[3]) / 4;

				// If the object is outside the FOV the probability for not generating observations should be high.
				// In the same way, if the object is inside the FOV, the prob. for not generating observations should be low.
				// In both cases, the prob should be proportional to the distance between the object and the center of the frustum.

				// Check if its inside the field of view
				if (this->_numObjects == 1) {
					cvmSet(z_k, 0, 0, cvmGet(objects[i]->state_post, 0, 0) + VIRTUAL_OBS_DIST2MEAN);
					cvmSet(z_k, 1, 0, cvmGet(objects[i]->state_post, 1, 0) + VIRTUAL_OBS_DIST2MEAN);
					this->association[j][i] = getProbFeatureGivenObject( z_k, i);
				} else {
					if (features.size() == this->_numObjects) {
						this->association[j][i] = 0.000000001;
						//cout << "Observación completa\n";
					}else if (!kinematics->pointInPolygon (4, vertx, verty, cvmGet(objects[i]->state_post, 0, 0), cvmGet(objects[i]->state_post, 1, 0)))
						this->association[j][i] = 0.1;
					else
						this->association[j][i] = 0.02;
				}
				/*GaussianDistribution2D gauss;

				gauss.mean.x = cvmGet(objects[i]->state_post, 0, 0);
				gauss.mean.y = cvmGet(objects[i]->state_post, 1, 0);
				gauss.covariance[0][0] = cvmGet(objects[i]->error_cov_post, 0, 0);
				gauss.covariance[0][1] = 0;
				gauss.covariance[1][0] = 0;
				gauss.covariance[1][1] = cvmGet(objects[i]->error_cov_post, 1, 1);

				// Evaluate the measurement at the Gaussian (only position)
				Vector2D_BH<double> feature2evaluate(frustumCenterX, frustumCenterY);
				this->association[j][i] = gauss.probabilityAt(feature2evaluate);
				cout << "Resultado evaluación para (" << j << "," << i << ") = " << gauss.probabilityAt(feature2evaluate) << endl;*/

				//cvmSet(z_k, 0, 0, cvmGet(objects[i]->state_post, 0, 0) + VIRTUAL_OBS_DIST2MEAN);
				//cvmSet(z_k, 1, 0, cvmGet(objects[i]->state_post, 1, 0) + VIRTUAL_OBS_DIST2MEAN);
				//this->association[j][i] = 0.000001f;
			}else{
				cvmSet(z_k, 0, 0, (*it).p3D.X);
				cvmSet(z_k, 1, 0, (*it).p3D.Y);

				// Checking distance
				float x1 = cvmGet(objects[i]->state_post, 0, 0);
				float y1 = cvmGet(objects[i]->state_post, 1, 0);
				float x2 = cvmGet(z_k, 0, 0);
				float y2 = cvmGet(z_k, 1, 0);
				//ToDo: Check this to avoid division by zero
				this->association[j][i] = 1.0f / std::min(calcDistance2D(x1, y1, x2, y2), 6000.0f);
				//this->association[j][i] = getProbFeatureGivenObject( z_k, i);
			}
			//this->association[j][i] = getProbFeatureGivenObject( z_k, i);
		}
		if (j != 0) it++;
	}

	//print( "association antes", association );

	// Normalize
	normalize( association, false );
	//print( "association paso 1", association );
	normalize( association);
	//print( "association paso 2", association );



	cvReleaseMat( &z_k );

	return true;
}


/*
 * getBeta1. This method calculates the beta value for an specific pair of object/feature,
 * given the rest of objects and features. This method is used when there is only one object
 * to track.
 * @param feature The index of the feature.
 * @param object The index of the object
 * @return The value of the beta for the feature / object selected.
 */
float
JPDAF::getBeta1(unsigned feature, unsigned object, list<AbstractSample> features )
{
	//if (this->_numObjects != 1) cerr << "JPDAF::getBeta1(): The number of current objects does not match: " <<
	//		this->_numObjects << endl;
	return this->association[feature][object];
}

/*
 * getBeta2. This method calculates the beta value for an specific pair of object/feature,
 * given the rest of objects and features. This method is used when there are two objects
 * to track.
 * @param feature The index of the feature.
 * @param object The index of the object
 * @return The value of the beta for the feature / object selected.
 */
float
JPDAF::getBeta2(unsigned feature, unsigned object, list<AbstractSample> features)
{
	//if (this->_numObjects != 2) cerr << "JPDAF::getBeta2(): The number of current objects does not match: " <<
	//		this->_numObjects << endl;

	// ToDo: Allow several objects to associate observation 0 (no observation)
	float result = 0.0f;
	for (unsigned j = 0; j < features.size() + 1; j++) {
		for (unsigned i = 0; i < this->_numObjects; i++) {
			if ((j != feature) && (i != object)) {
				result += this->association[feature][object] * this->association[j][i];
			}
		}
	}
	return result;
}

/*
 * getBeta4. This method calculates the beta value for an specific pair of object/feature,
 * given the rest of objects and features. This method is used when there are four objects
 * to track.
 * @param feature The index of the feature.
 * @param object The index of the object
 * @return The value of the beta for the feature / object selected.
 */
float
JPDAF::getBeta4(unsigned feature, unsigned object, list<AbstractSample> features )
{
	if (this->_numObjects != 4) {
		//cerr << "JPDAF::getBeta4(): The number of current objects does not match: " << this->_numObjects << endl;
		return 0.0f;
	}

	double result = 0.0;

	Masks m;
	if (features.size() == 1) {
		Mask m1; m1.push_back(0); m1.push_back(0);m1.push_back(0); m1.push_back(1);  		//[0,0,0,1]
		Mask m2; m2.push_back(0); m2.push_back(0);m2.push_back(1); m2.push_back(0);  		//[0,0,1,0]
		Mask m3; m3.push_back(0); m3.push_back(1);m3.push_back(0); m3.push_back(0);  		//[0,1,0,0]
		Mask m4; m4.push_back(1); m4.push_back(0);m4.push_back(0); m4.push_back(0);  		//[1,0,0,0]
		m.push_back(m1); m.push_back(m2); m.push_back(m3);  m.push_back(m4);
	} else if (features.size() == 2) {
		Mask m1; m1.push_back(0); m1.push_back(0);m1.push_back(1); m1.push_back(2);  		//[0,0,1,2]
		Mask m2; m2.push_back(0); m2.push_back(0);m2.push_back(2); m2.push_back(1);  		//[0,0,2,1]
		Mask m3; m3.push_back(0); m3.push_back(1);m3.push_back(0); m3.push_back(2);  		//[0,1,0,2]
		Mask m4; m4.push_back(0); m4.push_back(1);m4.push_back(2); m4.push_back(0);  		//[0,1,2,0]
		Mask m5; m5.push_back(0); m5.push_back(2);m5.push_back(0); m5.push_back(1);  		//[0,2,0,1]
		Mask m6; m6.push_back(0); m6.push_back(2);m6.push_back(1); m6.push_back(0);  		//[0,2,1,0]
		Mask m7; m7.push_back(1); m7.push_back(0);m7.push_back(0); m7.push_back(2);  		//[1,0,0,2]
		Mask m8; m8.push_back(1); m8.push_back(0);m8.push_back(2); m8.push_back(0);  		//[1,0,2,0]
		Mask m9; m9.push_back(1); m9.push_back(2);m9.push_back(0); m9.push_back(0);  		//[1,2,0,0]
		Mask m10; m10.push_back(2); m10.push_back(0);m10.push_back(0); m10.push_back(1);  	//[2,0,0,1]
		Mask m11; m11.push_back(2); m11.push_back(0);m11.push_back(1); m11.push_back(0);  	//[2,0,1,0]
		Mask m12; m12.push_back(2); m12.push_back(1);m12.push_back(0); m12.push_back(0);  	//[2,1,0,0]

		m.push_back(m1); m.push_back(m2); m.push_back(m3);  m.push_back(m4); m.push_back(m5); m.push_back(m6);
		m.push_back(m7); m.push_back(m8); m.push_back(m9);  m.push_back(m10); m.push_back(m11); m.push_back(m12);
	} else if (features.size() == 4) {
		Mask m1; m1.push_back(1); m1.push_back(2);m1.push_back(3); m1.push_back(4);  		//[1,2,3,4]
		Mask m2; m2.push_back(1); m2.push_back(2);m2.push_back(4); m2.push_back(3);  		//[1,2,4,3]
		Mask m3; m3.push_back(1); m3.push_back(3);m3.push_back(2); m3.push_back(4);  		//[1,3,2,4]
		Mask m4; m4.push_back(1); m4.push_back(3);m4.push_back(4); m4.push_back(2);  		//[1,3,4,2]
		Mask m5; m5.push_back(1); m5.push_back(4);m5.push_back(2); m5.push_back(3);  		//[1,4,2,3]
		Mask m6; m6.push_back(1); m6.push_back(4);m6.push_back(3); m6.push_back(2);  		//[1,4,3,2]
		Mask m7; m7.push_back(2); m7.push_back(1);m7.push_back(3); m7.push_back(4);  		//[2,1,3,4]
		Mask m8; m8.push_back(2); m8.push_back(1);m8.push_back(4); m8.push_back(3);  		//[2,1,4,3]
		Mask m9; m9.push_back(2); m9.push_back(3);m9.push_back(1); m9.push_back(4);  		//[2,3,1,4]
		Mask m10; m10.push_back(2); m10.push_back(3);m10.push_back(4); m10.push_back(1);  	//[2,3,4,1]
		Mask m11; m11.push_back(2); m11.push_back(4);m11.push_back(1); m11.push_back(3);  	//[2,4,1,3]
		Mask m12; m12.push_back(2); m12.push_back(4);m12.push_back(3); m12.push_back(1);  	//[2,4,3,1]
		Mask m13; m13.push_back(3); m13.push_back(1);m13.push_back(2); m13.push_back(4);  	//[3,1,2,4]
		Mask m14; m14.push_back(3); m14.push_back(1);m14.push_back(4); m14.push_back(2);  	//[3,1,4,2]
		Mask m15; m15.push_back(3); m15.push_back(2);m15.push_back(1); m15.push_back(4);  	//[3,2,1,4]
		Mask m16; m16.push_back(3); m16.push_back(2);m16.push_back(4); m16.push_back(1);  	//[3,2,4,1]
		Mask m17; m17.push_back(3); m17.push_back(4);m17.push_back(1); m17.push_back(2);  	//[3,4,1,2]
		Mask m18; m18.push_back(3); m18.push_back(4);m18.push_back(2); m18.push_back(1);  	//[3,4,2,1]
		Mask m19; m19.push_back(4); m19.push_back(1);m19.push_back(2); m19.push_back(3);  	//[4,1,2,3]
		Mask m20; m20.push_back(4); m20.push_back(1);m20.push_back(3); m20.push_back(2);  	//[4,1,3,2]
		Mask m21; m21.push_back(4); m21.push_back(2);m21.push_back(1); m21.push_back(3);  	//[4,2,1,3]
		Mask m22; m22.push_back(4); m22.push_back(2);m22.push_back(3); m22.push_back(1);  	//[4,2,3,1]
		Mask m23; m23.push_back(4); m23.push_back(3);m23.push_back(1); m23.push_back(2);  	//[4,3,1,2]
		Mask m24; m24.push_back(4); m24.push_back(3);m24.push_back(2); m24.push_back(1);  	//[4,3,2,1]

		m.push_back(m1); m.push_back(m2); m.push_back(m3);  m.push_back(m4); m.push_back(m5); m.push_back(m6);
		m.push_back(m7); m.push_back(m8); m.push_back(m9);  m.push_back(m10); m.push_back(m11); m.push_back(m12);
		m.push_back(m13); m.push_back(m14); m.push_back(m15);  m.push_back(m16); m.push_back(m17); m.push_back(m18);
		m.push_back(m19); m.push_back(m20); m.push_back(m21);  m.push_back(m22); m.push_back(m23); m.push_back(m24);
	} else {
		//cerr << "JPDAF::getBeta4() Number of features (" << features.size() << ") not implemented\n";
		return 0.0f;
	}

	//cout << "JPDAF::getBeta4()" << "(" << feature << "," << object << ") = \n";
	for (unsigned k = 0; k < m.size(); k++) {
		Mask mask = m[k];
		if (mask[object] == feature) {
			float assoc = 1.0f;
			//cout << "\t";
			for (unsigned i = 0; i < mask.size(); i++) {
				assoc *= this->association[mask[i]][i];
				//cout << mask[i] << "(" << this->association[mask[i]][i] << ") * ";
			}
			//cout << endl;
			result += assoc;
		}
	}
	//cout << "Result: " << result << endl;

	return result;
}


/*
 * updateBeta. a 2D vector called 'beta' is updated in this method. This container maintains
 * the whole compatibility among features and objects. Note that the content of each cell
 * expresses the matching between an observation and an object taking into account the rest
 * of features and objects. This is the real output of the association.
 * @param features The list of features received in this frame
 * @return The method returns 'false' in case the number of features exceed the maximum number
 * of features allowed by 'maxNFeatures' attribute, or true in other case.
 */
bool
JPDAF::updateBeta( list<AbstractSample> features )
{
	bool ret = updateAssociation( features );
	//print( "association", association );

	// If number of features is greater than maxNfeatures just ignore them
	if (! ret) return false;

	for (unsigned j = 0; j < maxNFeatures; j++ ) { // Features
		for (unsigned i = 0; i < this->_numObjects; i++ ) { // Objects
			if (this->_numObjects == 1)
				this->beta[j][i] = getBeta1( j, i, features );
			else if (this->_numObjects == 2)
				this->beta[j][i] = getBeta2( j, i, features);
			/*else if (this->_numObjects == 3)
				this->beta[j][i] = getBeta3( j, i, features );*/
			else if (this->_numObjects == 4)
				this->beta[j][i] = getBeta4( j, i, features );
			//else
			//cerr << "JPDAF::updateBeta(): Number of objects not supported yet\n";
		}
	}

	// Normalize
	normalize( beta );
	//

	//print( "beta", beta );

	return true;
}

void
JPDAF::predict()
{
	// Update the process noise matrix for a prediction update.
	updateProcessNoiseMatrix(PREDICTION);

	for (unsigned i = 0; i < this->_numObjects; i++ ) {
		bicaKalmanPredict( this->objects[i], 0 );

		// Update the state of the object
		cvmSet(objects[i]->state_post, 0, 0, cvmGet(objects[i]->state_pre, 0, 0));
		cvmSet(objects[i]->state_post, 1, 0, cvmGet(objects[i]->state_pre, 1, 0));

		cvmSet(objects[i]->error_cov_post, 0, 0, cvmGet(objects[i]->error_cov_pre, 0, 0));
		cvmSet(objects[i]->error_cov_post, 0, 1, cvmGet(objects[i]->error_cov_pre, 0, 1));
		cvmSet(objects[i]->error_cov_post, 1, 0, cvmGet(objects[i]->error_cov_pre, 1, 0));
		cvmSet(objects[i]->error_cov_post, 1, 1, cvmGet(objects[i]->error_cov_pre, 1, 1));
	}
}

/*
 * correct. The JPDAF is based in the classical prediction / correction updates. This
 * method implements the correction step given a list of features.
 * @param features List of observations for a given frame.
 */
void
JPDAF::correct( list<AbstractSample> features )
{
	// If number of features is greater than maxNfeatures just ignore them
	if (features.size() > this->maxNFeatures - 1) return;

	pthread_mutex_lock(&mutex);

	this->currentFeatures = features;

	//cout << "Num observaciones recibidas: " << features.size() << endl;

	bool starting;
	list<AbstractSample>::iterator it;
	GaussianDistribution2D gauss;
	GaussianDistribution2D gaussAux;
	CvMat	*x_k = cvCreateMat( 4, 1, CV_32FC1 );
	CvMat   *xpred_k = cvCreateMat( 4, 1, CV_32FC1 );
	CvMat   *C_k = cvCreateMat( 4, 4, CV_32FC1 );
	CvMat	*Cpred_k = cvCreateMat( 4, 4, CV_32FC1 );
	CvMat   *z_k = cvCreateMat( 4, 1, CV_32FC1 );
	cvZero( z_k );

	this->currentNFeatures = features.size() + 1;
	updateBeta( features );

	for (unsigned i = 0; i < this->_numObjects; i++ ) {

		float max = -1.0f;
		unsigned pos = 0;
		for (unsigned k = 0; k < features.size() + 1; k++) {
			if (this->beta[k][i] > max) {
				max = this->beta[k][i];
				pos = k;
			}
		}

		// ToDo: Tener en cuenta que puede haber más observaciones que objetos
		if (((this->_numObjects == 1) && (max > BETA_THRESHOLD_1_OBJECT) && (pos == 0)) ||
				((this->_numObjects == 2) && (max > BETA_THRESHOLD_2_OBJECT) ) ||
				((this->_numObjects == 4) && (max > BETA_THRESHOLD_4_OBJECT) )) {

			//cout << "Asociación muy clara para el objeto " << i << " con observación " << pos << endl;

			if (pos == 0) {
				updateProcessNoiseMatrix(PREDICTION);
				bicaKalmanPredict( this->objects[i], 0 );
			} else {
				// Update the process noise matrix for a correction update.
				updateProcessNoiseMatrix(MEASUREMENT);

				// Prediction
				bicaKalmanPredict( this->objects[i], 0 );

				it = features.begin();
				for (unsigned j = 1; j < pos; j++)
					it++;

				// Incorporate observation
				cvmSet(z_k, 0, 0, (*it).p3D.X);
				cvmSet(z_k, 1, 0, (*it).p3D.Y);
				//cout << "Observación en (" << (*it).p3D.X << "," << (*it).p3D.Y << ")\n";

				bicaKalmanCorrect( this->objects[i], z_k );
			}

			continue;
		}

		if (this->_numObjects == 4)
			continue;  // Only full associations

		it = features.begin();
		starting = true;

		// Save the state of object i before any measurement update
		x_k = cvCloneMat(this->objects[i]->state_post);
		C_k = cvCloneMat(this->objects[i]->error_cov_post);
		xpred_k = cvCloneMat(this->objects[i]->state_pre);
		Cpred_k = cvCloneMat(this->objects[i]->error_cov_pre);

		// Update the process noise matrix for a correction update.
		updateProcessNoiseMatrix(MEASUREMENT);

		for (unsigned j = 0; j < features.size() + 1; j++) {

			// Prediction
			bicaKalmanPredict( this->objects[i], 0 );

			if (j != 0) {
				// Incorporate observation
				cvmSet(z_k, 0, 0, (*it).p3D.X);
				cvmSet(z_k, 1, 0, (*it).p3D.Y);
				it++;

				bicaKalmanCorrect( this->objects[i], z_k );
			}

			// Crop between [MIN_BETA,MAX_BETA] and calculate factor for multiplying Gaussians
			//float b = 1.0f / std::max ( MIN_BETA, std::min ( MAX_BETA, (float)beta[j][i] ) );
			double b = 1.0f / std::max ( 0.0001, std::min ( 0.9999, beta[j][i] ) );

			// Set the parameters of the Gaussian (same as object i)
			gaussAux.mean.x = cvmGet(objects[i]->state_post, 0, 0);
			gaussAux.mean.y = cvmGet(objects[i]->state_post, 1, 0);
			gaussAux.covariance[0][0] = b * cvmGet(objects[i]->error_cov_post, 0, 0);
			gaussAux.covariance[0][1] = b * cvmGet(objects[i]->error_cov_post, 0, 1);
			gaussAux.covariance[1][0] = b * cvmGet(objects[i]->error_cov_post, 1, 0);
			gaussAux.covariance[1][1] = b * cvmGet(objects[i]->error_cov_post, 1, 1);

			// Undo the prediction & correction
			objects[i]->state_pre = cvCloneMat(xpred_k);
			objects[i]->error_cov_pre = cvCloneMat(Cpred_k);
			objects[i]->state_post = cvCloneMat(x_k);
			objects[i]->error_cov_post = cvCloneMat(C_k);

			if (starting)		// Initializing gauss
			{
				starting = false;
				gauss = gaussAux;
			}else{
				gauss += gaussAux;
			}
		}

		// Update the state of the object
		cvmSet(objects[i]->state_post, 0, 0, gauss.mean.x);
		cvmSet(objects[i]->state_post, 1, 0, gauss.mean.y);

		cvmSet(objects[i]->error_cov_post, 0, 0, std::min( gauss.covariance[0][0], 500.0 * 500.0));
		cvmSet(objects[i]->error_cov_post, 0, 1, std::min( gauss.covariance[0][1], 500.0 * 500.0));
		cvmSet(objects[i]->error_cov_post, 1, 0, std::min( gauss.covariance[1][0], 500.0 * 500.0));
		cvmSet(objects[i]->error_cov_post, 1, 1, std::min( gauss.covariance[1][1], 500.0 * 500.0));
	}

	cvReleaseMat( &x_k );
	cvReleaseMat( &xpred_k );
	cvReleaseMat( &C_k );
	cvReleaseMat( &Cpred_k );
	cvReleaseMat( &z_k );

	pthread_mutex_unlock(&mutex);
}

void
JPDAF::correct(AbstractSample feature, int object)
{

	pthread_mutex_lock(&mutex);

	this->currentFeatures.clear();
	this->currentFeatures.push_back(feature);

	CvMat   *z_k = cvCreateMat( 4, 1, CV_32FC1 );
	cvZero( z_k );

	// Update the process noise matrix for a correction update.
	updateProcessNoiseMatrix(MEASUREMENT);

	// Prediction
	bicaKalmanPredict( this->objects[object], 0 );

	// Incorporate observation
	cvmSet(z_k, 0, 0, feature.p3D.X);
	cvmSet(z_k, 1, 0, feature.p3D.Y);

	bicaKalmanCorrect( this->objects[object], z_k );

	// Testing frustim


	cvReleaseMat( &z_k );
	pthread_mutex_unlock(&mutex);
}


void
JPDAF::correctFromOdometry()
{
	// Update the process noise matrix for an odometry update.
	updateProcessNoiseMatrix(ODOMETRY);

	// Calculate movement from last step
	float movX, movY, movA, currentBodyX, currentBodyY, currentBodyTh, dx, dy;

	body->getGlobalMovement(currentBodyX, currentBodyY, currentBodyTh);
	dx = currentBodyX - lastBodyX;
	dy = currentBodyY - lastBodyY;

	body->getRelativeMovement(lastBodyX, lastBodyY, lastBodyTh, movX, movY,  movA);

	// Update current values of movement
	lastBodyX = currentBodyX;
	lastBodyY = currentBodyY;
	lastBodyTh = currentBodyTh;

	//cout << "Odometría relativa: (" << movX << "," << movY << "," << movA << ")\n";

	if (movX != 0 || movY != 0 || movA != 0) {	// There has been some movement

		for (unsigned i = 0; i < this->_numObjects; i++ ) {
			Vector2<double> pos (cvmGet(objects[i]->state_post, 0, 0),
					cvmGet(objects[i]->state_post, 1, 0));

			// Translation
			Vector2<double> translation(movX, movY);
			pos -= translation;

			// Rotation
			pos.rotate(-movA);

			// Update the state of the object
			cvmSet(objects[i]->state_post, 0, 0, pos.x);
			cvmSet(objects[i]->state_post, 1, 0, pos.y);

			bicaKalmanPredict( this->objects[i], 0 );

			// Update the state of the object
			cvmSet(objects[i]->state_post, 0, 0, cvmGet(objects[i]->state_pre, 0, 0));
			cvmSet(objects[i]->state_post, 1, 0, cvmGet(objects[i]->state_pre, 1, 0));

			cvmSet(objects[i]->error_cov_post, 0, 0, std::min( cvmGet(objects[i]->error_cov_pre, 0, 0), 500.0 * 500.0));
			cvmSet(objects[i]->error_cov_post, 0, 1, std::min( cvmGet(objects[i]->error_cov_pre, 0, 1), 500.0 * 500.0));
			cvmSet(objects[i]->error_cov_post, 1, 0, std::min( cvmGet(objects[i]->error_cov_pre, 1, 0), 500.0 * 500.0));
			cvmSet(objects[i]->error_cov_post, 1, 1, std::min( cvmGet(objects[i]->error_cov_pre, 1, 1), 500.0 * 500.0));

			//cvmSet(objects[i]->error_cov_post, 0, 0, cvmGet(objects[i]->error_cov_post, 0, 0) + 1000.0);
			//cvmSet(objects[i]->error_cov_post, 1, 1, cvmGet(objects[i]->error_cov_post, 1, 1) + 1000.0);
		}
	}
}

/*
 * updateProcessNoiseMatrix. Changes the process noise matrix according to the
 * type of object (static or dynamic) and the operation to be performed. See
 * the table bellow.
 *
 * 			  	||	staticObj	|	dynamicObj	|
 	 	 	 	=================================
 * prediction	||	  LOW		|	  MEDIUM	|
 * ----------------------------------------------
 * odometry		||	  HIGH		|		HIGH	|
 * ----------------------------------------------
 * observation	||	 MEDIUM		|	  MEDIUM	|
 * ----------------------------------------------
 *
 * @param op Type of operation to be done after readjusting the matrix
 */
void
JPDAF::updateProcessNoiseMatrix (tOperation op)
{
	float uncertValue = MED_UNCERTAINTY_COV;

	if (op == PREDICTION) {
		if (objType == STATIC_OBJ)
			uncertValue = LOW_UNCERTAINTY_COV;
		//uncertValue = HIGH_UNCERTAINTY_COV;
		else
			//uncertValue = HIGH_UNCERTAINTY_COV;
			uncertValue = MED_UNCERTAINTY_COV;
	} else if (op == ODOMETRY) {
		//uncertValue = HIGH_UNCERTAINTY_COV;
		uncertValue = MED_UNCERTAINTY_COV;
	} else if (op == MEASUREMENT) {
		uncertValue = MED_UNCERTAINTY_COV;
		//uncertValue = HIGH_UNCERTAINTY_COV;
	} //else {
	//	cerr << "JPDAF::updateProcessNoiseMatrix() Ilegal operation (" << op << ")\n";
	//}

	for (unsigned i = 0; i < this->_numObjects; i++){
		cvSetIdentity( objects[i]->process_noise_cov, cvRealScalar(uncertValue * uncertValue) );
		cvmSet(objects[i]->process_noise_cov, 2, 2, PROCESS_NOISE_VEL_COV * PROCESS_NOISE_VEL_COV);
		cvmSet(objects[i]->process_noise_cov, 3, 3, PROCESS_NOISE_VEL_COV * PROCESS_NOISE_VEL_COV); // 2 mm/seg
	}
}

bica::ShapeList
JPDAF::getGrDebugRel()
{
	//pthread_mutex_lock(&mutex);

	shapeListRel.clear();

	//cout << "JPDAF::getGrDebugRel()\n";

	list<AbstractSample>::iterator it;
	int i = 1;
	for ( it = this->currentFeatures.begin(); it != this->currentFeatures.end(); it++) {
		double x = (*it).p3D.X;
		double y = (*it).p3D.Y;

		//cout << "JPDAF::getGrDebugRel() Observacion en punto (" << x << "," << y << ")\n";
		bica::Point3DPtr p(new bica::Point3D( (float)x, (float)y, 0.0f ));
		stringstream ss;
		ss << "Z" << i;
		bica::CirclePtr c(new bica::Circle( bica::BLACK, true, "z", 255, ss.str(), p, 100.0f, 0.0f ));
		shapeListRel.push_back(c);
		i++;
	}

	//pthread_mutex_unlock(&mutex);

	return shapeListRel;
}
