/*
 * Name: SingleObjectModel.cpp
 * @Author: Carlos Agüero (caguero@gsyc.es)
 *
 * Description:
 *
 * Created on: 23/02/2011
 *
 * Copyright (C) Universidad Rey Juan Carlos
 * All Rights Reserved.
 *
 */


#include "SingleObjectModel.h"

/** ====================================================
 * @class SingleObjectModel
 *
 * Contains all current knowledge about the object.
 * =====================================================*/
SingleObjectModel::SingleObjectModel( int numObjects = 1, int maxNFeatures = 3 )
{
	this->jpdaf = new JPDAF (JPDAF::DYNAMIC_OBJ, numObjects, maxNFeatures);

	elapsedTimeSinceLastObs = ObjectState::NEVER;

	updateEstimate();

	// ToDo: multiple objects (nets)
	// Velocity estimation
	lastPos.x = 0.0f;
	lastPos.y = 0.0f;
	lastMeasurement = getCurrentTime();
	dx = 0.0f;
	dy = 0.0f;
}

// Calculate the area with 95% of probability of surrounding the object.
// Area of ellipse = Pi * a * b, where a and b are the semi-axis.
float
SingleObjectModel::calculateSingleObjectQuality( int object )
{
	float a = 4 * sqrt(cvmGet(this->jpdaf->objects[object]->error_cov_post, 0, 0));
	float b = 4 * sqrt(cvmGet(this->jpdaf->objects[object]->error_cov_post, 1, 1));
	float area = pi * a * b;
	return area > ObjectState::MAX_UNCERTAINTY_AREA ? 0.0f : 1.0f - (area / ObjectState::MAX_UNCERTAINTY_AREA);

	if (area > ObjectState::MAX_UNCERTAINTY_AREA) {
		return 0.0f;
	} else {
		return 1.0f - (area / ObjectState::MAX_UNCERTAINTY_AREA);
	}
}

void
SingleObjectModel::updateEstimate()
{
	// Last estimation (ball seen or not seen)
	estimate.setPositionAndVelocityInRelativeCoordinates(
			Vector2<double>(cvmGet(this->jpdaf->objects[0]->state_post, 0, 0),
					cvmGet(this->jpdaf->objects[0]->state_post, 1, 0)));

	// Update the quality estimations
	estimate.setQuality( calculateSingleObjectQuality(0) );

	// Update reliability
	updateReliability();
}

void
SingleObjectModel::predict()
{
	// Update the elapsed time since the object was seen (ms.)
	elapsedTimeSinceLastObs = (getCurrentTime() - lastMeasurement) / 1000.0f;

	jpdaf->predict();

	updateEstimate();
}

void
SingleObjectModel::updateFromObservation(list<AbstractSample> features)
{
	long now = getCurrentTime();
	elapsed = now - lastMeasurement;
	lastMeasurement = now;

	// The object has been detected
	elapsedTimeSinceLastObs = 0.0f;

	this->jpdaf->correct( features );

	updateEstimate();
}

void
SingleObjectModel::updateFromOdometry()
{
	jpdaf->correctFromOdometry();

	// Update the elapsed time since the object was seen (ms.)
	elapsedTimeSinceLastObs = (getCurrentTime() - lastMeasurement) / 1000.0f;

	updateEstimate();
}

bica::ShapeList
SingleObjectModel::getGrDebugRel()
{
	pthread_mutex_lock(&(jpdaf->mutex));

	shapeListRel.clear();

	// Ball
	bica::Point3DPtr p(new bica::Point3D);
	p->x = estimate.getPositionInRelativeCoordinates().x;
	p->y = estimate.getPositionInRelativeCoordinates().y;;
	p->z = 0.0f;

	bica::EllipsePtr estEllipse(new bica::Ellipse);
	estEllipse->center = p;
	estEllipse->width = 4 * sqrt(cvmGet(jpdaf->objects[0]->error_cov_post, 0, 0));
	estEllipse->length = 4 * sqrt(cvmGet(jpdaf->objects[0]->error_cov_post, 1, 1));
	estEllipse->angle =  toDegrees(atan2(cvmGet(jpdaf->objects[0]->state_post, 1, 0),
			cvmGet(jpdaf->objects[0]->state_post, 0, 0)));
	estEllipse->z = 0.0f;
	estEllipse->color = bica::ORANGE;
	estEllipse->filled = true;
	estEllipse->opacity = 125;
	estEllipse->accKey = "o";
	estEllipse->label = "Ball";
	shapeListRel.push_back(estEllipse);

	pthread_mutex_unlock(&(jpdaf->mutex));

	return shapeListRel;
}

