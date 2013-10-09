/*
 * Name: GoalsModel.cpp
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

#include "GoalsModel.h"

GoalsModel::GoalsModel()
{
	gameController = GameController::getInstance();
	kinematics = Kinematics::getInstance();

	p0l = p0r = p1l = p1r = 0;

	this->yellowJPDAF = new JPDAF (JPDAF::STATIC_OBJ, 4, 4);

	updateEstimate();

	// Time elapsed since last net observations
	p0ElapsedTimeSinceLastObs = ObjectState::NEVER;
	p1ElapsedTimeSinceLastObs = ObjectState::NEVER;

	p0LastMeasurement = getCurrentTime();
	p1LastMeasurement = getCurrentTime();
}

void
GoalsModel::updateEstimate()
{
	// Label posts
	labelPosts(this->p0l, this->p0r, this->p1l, this->p1r);

	fitPosts(P0);
	fitPosts(P1);

	// Posts estimations
	p0LeftEstimate.setPositionAndVelocityInRelativeCoordinates(
			Vector2<double>(cvmGet(this->yellowJPDAF->objects[this->p0l]->state_post, 0, 0),
					cvmGet(this->yellowJPDAF->objects[this->p0l]->state_post, 1, 0)));
	p0LeftEstimate.setQuality( calculateSingleObjectQuality(this->p0l) );
	updatePostReliability(&p0LeftEstimate);

	p0RightEstimate.setPositionAndVelocityInRelativeCoordinates(
			Vector2<double>(cvmGet(this->yellowJPDAF->objects[this->p0r]->state_post, 0, 0),
					cvmGet(this->yellowJPDAF->objects[this->p0r]->state_post, 1, 0)));
	p0RightEstimate.setQuality( calculateSingleObjectQuality(this->p0r) );
	updatePostReliability(&p0RightEstimate);

	p1LeftEstimate.setPositionAndVelocityInRelativeCoordinates(
			Vector2<double>(cvmGet(this->yellowJPDAF->objects[this->p1l]->state_post, 0, 0),
					cvmGet(this->yellowJPDAF->objects[this->p1l]->state_post, 1, 0)));
	p1LeftEstimate.setQuality( calculateSingleObjectQuality(this->p1l) );
	updatePostReliability(&p1LeftEstimate);

	p1RightEstimate.setPositionAndVelocityInRelativeCoordinates(
			Vector2<double>(cvmGet(this->yellowJPDAF->objects[this->p1r]->state_post, 0, 0),
					cvmGet(this->yellowJPDAF->objects[this->p1r]->state_post, 1, 0)));
	p1RightEstimate.setQuality( calculateSingleObjectQuality(this->p1r) );
	updatePostReliability(&p1RightEstimate);

	// Net center estimations (seen or not seen)
	int p0CenterX, p0CenterY;
	getGoalCenter(P0, &p0CenterX, &p0CenterY);
	p0NetCenterEstimate.setPositionAndVelocityInRelativeCoordinates(
			Vector2<double>(p0CenterX,p0CenterY));
	p0NetCenterEstimate.setQuality( calculateNetQuality(P0) );
	p0NetCenterEstimate.setReliability( calculateNetReliability(P0) );

	int p1CenterX, p1CenterY;
	getGoalCenter(P1, &p1CenterX, &p1CenterY);
	p1NetCenterEstimate.setPositionAndVelocityInRelativeCoordinates(
			Vector2<double>(p1CenterX,p1CenterY));
	p1NetCenterEstimate.setQuality( calculateNetQuality(P1) );
	p1NetCenterEstimate.setReliability( calculateNetReliability(P1) );
}

// Calculate the area with 95% of probability of surrounding the object.
// Area of ellipse = Pi * a * b, where a and b are the semi-axis.
float
GoalsModel::calculateSingleObjectQuality( int object )
{
	float a = 4 * sqrt(cvmGet(this->yellowJPDAF->objects[object]->error_cov_post, 0, 0));
	float b = 4 * sqrt(cvmGet(this->yellowJPDAF->objects[object]->error_cov_post, 1, 1));
	float area = pi * a * b;

	if (area > ObjectState::MAX_UNCERTAINTY_AREA) {
		return 0.0f;
	} else {
		return 1.0f - (area / ObjectState::MAX_UNCERTAINTY_AREA);
	}
}

// Calculate the area with 95% of probability of surrounding each post.
// Area of ellipse = Pi * a * b, where a and b are the semi-axis.
float
GoalsModel::calculateNetQuality(GoalType net)
{
	if (net == P0) {
		return (p0LeftEstimate.getQuality() + p0RightEstimate.getQuality()) / 2.0f;
	} else {
		return (p1LeftEstimate.getQuality() + p1RightEstimate.getQuality()) / 2.0f;
	}
}

ObjectState::ObjectReliability
GoalsModel::calculateNetReliability(GoalType net)
{
	ObjectState *netCenter;

	if (net == P0) {
		netCenter = &p0NetCenterEstimate;
	} else {
		netCenter = &p1NetCenterEstimate;
	}

	if (netCenter->getQuality() > ObjectState::HIGH_RELIABILITY_QUALITY)
		return ObjectState::MOST_RELIABLE;
	else if (netCenter->getQuality() > ObjectState::MEDIUM_RELIABILITY_QUALITY)
		return ObjectState::HIGH_RELIABLE;
	else if (netCenter->getQuality() > ObjectState::LOW_RELIABILITY_QUALITY)
		return ObjectState::MEDIUM_RELIABLE;
	else if (netCenter->getQuality() > ObjectState::VERY_LOW_RELIABILITY_QUALITY)
		return ObjectState::LOW_RELIABLE;
	else
		return ObjectState::UNRELIABLE;
}


void
GoalsModel::getGoalCenter(GoalType net, int *x, int *y)
{
	//ToDo: use worldmodel
	static const float GOAL_POSTS_DISTANCE = 1400.0f;

	// 1. Supposing that the left post estimation is perfect, calculate the virtual
	// right post estimation (using the GOAL_POST_DISTANCE restriction)
	//	1.1 Obtain the vector between left and right posts.
	//  1.2 Calculate the intersection between the vector and a circumference
	// with center the right post and radius GOAL_POST_DISTANCE
	// 2. Supposing that the right post estimation is perfect, calculate the virtual
	// left post estimation (using the GOAL_POST_DISTANCE restriction).
	//	2.1 Same as 1.1
	//  2.2 Calculate the intersection between the vector and a circumference
	// with center the left post and radius GOAL_POST_DISTANCE

	int p0, p1;

	//JPDAF *jpdaf;
	HPoint3D v;
	HPoint3D int1, int2;
	HPoint3D virtualP0, virtualP1;

	if (net == P0) {
		p0 = this->p0l;
		p1 = this->p0r;
	} else {
		p0 = this->p1l;
		p1 = this->p1r;
	}

	HPoint2D p0_2D = {cvmGet(this->yellowJPDAF->objects[p0]->state_post, 0, 0),
			cvmGet(this->yellowJPDAF->objects[p0]->state_post, 1, 0),
			1.0f};

	HPoint2D p1_2D = {cvmGet(this->yellowJPDAF->objects[p1]->state_post, 0, 0),
			cvmGet(this->yellowJPDAF->objects[p1]->state_post, 1, 0),
			1.0f};

	HPoint3D p0_3D = {cvmGet(this->yellowJPDAF->objects[p0]->state_post, 0, 0),
			cvmGet(this->yellowJPDAF->objects[p0]->state_post, 1, 0),
			0.0f,
			1.0f};

	HPoint3D p1_3D = {cvmGet(this->yellowJPDAF->objects[p1]->state_post, 0, 0),
			cvmGet(this->yellowJPDAF->objects[p1]->state_post, 1, 0),
			0.0f,
			1.0f};

	// calculate the vector between p0 and p1
	calcVector2D( p0_2D, p1_2D, v );

	//Calculate the intersection between the vector and a circumference
	// with center p0 and radius GOAL_POST_DISTANCE
	HPoint3D circunf_center0 = {p0_2D.x, p0_2D.y, 0.0f, 1.0f};
	calIntersectionCircleVector( v, circunf_center0, GOAL_POSTS_DISTANCE, int1, int2 );
	// Reject one intersection (the one that is at a longer distance from the other post.
	if (calcDistance3D(int1, p1_3D) < calcDistance3D(int2, p1_3D))
		virtualP1 = int1;
	else
		virtualP1 = int2;

	//Calculate the intersection between the vector and a circumference
	// with center p1 and radius GOAL_POST_DISTANCE
	HPoint3D circunf_center1 = {p1_2D.x, p1_2D.y, 0.0f, 1.0f};
	calIntersectionCircleVector( v, circunf_center1, GOAL_POSTS_DISTANCE, int1, int2 );
	// Reject one intersection (the one that is at a longer distance from the other post.
	if (calcDistance3D(int1, p0_3D) < calcDistance3D(int2, p0_3D))
		virtualP0 = int1;
	else
		virtualP0 = int2;

	// Middle point between p0 and vP1
	float x0 = (p0_2D.x + virtualP1.X) / 2.0f;
	float y0 = (p0_2D.y + virtualP1.Y) / 2.0f;

	// Middle point between p1 and vP0
	float x1 = (p1_2D.x + virtualP0.X) / 2.0f;
	float y1 = (p1_2D.y + virtualP0.Y) / 2.0f;

	// Calculate an intermediate point between (x0,y0) and (x1,y1) according
	// the uncertainty of each post.
	MatrixCM *x1m = new MatrixCM(2,2);
	MatrixCM *x2m = new MatrixCM(2,2);
	MatrixCM *centerm = new MatrixCM(2,2);
	MatrixCM *c1m = new MatrixCM(2,2);
	MatrixCM *c2m = new MatrixCM(2,2);

	x1m->e(0,0) = x0;
	x1m->e(1,0) = y0;

	x2m->e(0,0) = x1;
	x2m->e(1,0) = y1;

	c1m->e(0,0) = cvmGet(this->yellowJPDAF->objects[p0]->error_cov_post, 0, 0);
	c1m->e(0,1) = 0;
	c1m->e(1,0) = 0;
	c1m->e(1,1) = cvmGet(this->yellowJPDAF->objects[p0]->error_cov_post, 1, 1);

	c2m->e(0,0) = cvmGet(this->yellowJPDAF->objects[p1]->error_cov_post, 0, 0);
	c2m->e(0,1) = 0;
	c2m->e(1,0) = 0;
	c2m->e(1,1) = cvmGet(this->yellowJPDAF->objects[p1]->error_cov_post, 1, 1);

	//Set new middle net point
	*centerm = (*x1m) + (((*c1m) * (inverse((*c1m) + (*c2m)))) * ((*x2m) - (*x1m)));

	*x = centerm->e(0,0);
	*y = centerm->e(1,0);
}

void
GoalsModel::fitPosts(GoalType net)
{
	static const float GOAL_POSTS_DISTANCE = 1400.0f;

	int p0, p1;
	//JPDAF *jpdaf;
	HPoint3D v;
	HPoint3D int1, int2;
	HPoint3D virtualP0, virtualP1;

	if (net == P0) {
		p0 = this->p0l;
		p1 = this->p0r;
	} else {
		p0 = this->p1l;
		p1 = this->p1r;
	}

	HPoint2D p0_2D = {cvmGet(this->yellowJPDAF->objects[p0]->state_post, 0, 0),
			cvmGet(this->yellowJPDAF->objects[p0]->state_post, 1, 0),
			1.0f};

	HPoint2D p1_2D = {cvmGet(this->yellowJPDAF->objects[p1]->state_post, 0, 0),
			cvmGet(this->yellowJPDAF->objects[p1]->state_post, 1, 0),
			1.0f};

	HPoint3D p0_3D = {cvmGet(this->yellowJPDAF->objects[p0]->state_post, 0, 0),
			cvmGet(this->yellowJPDAF->objects[p0]->state_post, 1, 0),
			0.0f,
			1.0f};

	HPoint3D p1_3D = {cvmGet(this->yellowJPDAF->objects[p1]->state_post, 0, 0),
			cvmGet(this->yellowJPDAF->objects[p1]->state_post, 1, 0),
			0.0f,
			1.0f};

	// calculate the vector between p0 and p1
	calcVector2D( p0_2D, p1_2D, v );

	//Calculate the intersection between the vector and a circumference
	// with center p0 and radius GOAL_POST_DISTANCE
	HPoint3D circunf_center0 = {p0_2D.x, p0_2D.y, 0.0f, 1.0f};
	calIntersectionCircleVector( v, circunf_center0, GOAL_POSTS_DISTANCE, int1, int2 );
	// Reject one intersection (the one that is at a longer distance from the other post.
	if (calcDistance3D(int1, p1_3D) < calcDistance3D(int2, p1_3D))
		virtualP1 = int1;
	else
		virtualP1 = int2;

	//Calculate the intersection between the vector and a circumference
	// with center p1 and radius GOAL_POST_DISTANCE
	HPoint3D circunf_center1 = {p1_2D.x, p1_2D.y, 0.0f, 1.0f};
	calIntersectionCircleVector( v, circunf_center1, GOAL_POSTS_DISTANCE, int1, int2 );
	// Reject one intersection (the one that is at a longer distance from the other post.
	if (calcDistance3D(int1, p0_3D) < calcDistance3D(int2, p0_3D))
		virtualP0 = int1;
	else
		virtualP0 = int2;

	// Middle point between p0 and vP1
	float x0 = (p0_2D.x + virtualP1.X) / 2.0f;
	float y0 = (p0_2D.y + virtualP1.Y) / 2.0f;

	// Middle point between p1 and vP0
	float x1 = (p1_2D.x + virtualP0.X) / 2.0f;
	float y1 = (p1_2D.y + virtualP0.Y) / 2.0f;

	// Calculate an intermediate point between (x0,y0) and (x1,y1) according
	// the uncertainty of each post.
	MatrixCM *x1m = new MatrixCM(2,2);
	MatrixCM *x2m = new MatrixCM(2,2);
	MatrixCM *centerm = new MatrixCM(2,2);
	MatrixCM *c1m = new MatrixCM(2,2);
	MatrixCM *c2m = new MatrixCM(2,2);

	x1m->e(0,0) = x0;
	x1m->e(1,0) = y0;

	x2m->e(0,0) = x1;
	x2m->e(1,0) = y1;

	c1m->e(0,0) = cvmGet(this->yellowJPDAF->objects[p0]->error_cov_post, 0, 0);
	c1m->e(0,1) = 0;
	c1m->e(1,0) = 0;
	c1m->e(1,1) = cvmGet(this->yellowJPDAF->objects[p0]->error_cov_post, 1, 1);

	c2m->e(0,0) = cvmGet(this->yellowJPDAF->objects[p1]->error_cov_post, 0, 0);
	c2m->e(0,1) = 0;
	c2m->e(1,0) = 0;
	c2m->e(1,1) = cvmGet(this->yellowJPDAF->objects[p1]->error_cov_post, 1, 1);

	//Set new middle net point
	*centerm = (*x1m) + (((*c1m) * (inverse((*c1m) + (*c2m)))) * ((*x2m) - (*x1m)));

	// Move posts
	HPoint3D circunf_center2 = {centerm->e(0,0), centerm->e(1,0), 0.0f, 1.0f};
	calIntersectionCircleVector( v, circunf_center2, GOAL_POSTS_DISTANCE / 2.0f, int1, int2 );

	if (calcDistance3D(int1, p0_3D) < calcDistance3D(int2, p0_3D)){
		// Set the mean of estimation 0
		cvmSet(this->yellowJPDAF->objects[p0]->state_post, 0, 0, int1.X);
		cvmSet(this->yellowJPDAF->objects[p0]->state_post, 1, 0, int1.Y);
		// Set the mean of estimation 1
		cvmSet(this->yellowJPDAF->objects[p1]->state_post, 0, 0, int2.X);
		cvmSet(this->yellowJPDAF->objects[p1]->state_post, 1, 0, int2.Y);
	} else {
		// Set the mean of estimation 0
		cvmSet(this->yellowJPDAF->objects[p0]->state_post, 0, 0, int2.X);
		cvmSet(this->yellowJPDAF->objects[p0]->state_post, 1, 0, int2.Y);
		// Set the mean of estimation 1
		cvmSet(this->yellowJPDAF->objects[p1]->state_post, 0, 0, int1.X);
		cvmSet(this->yellowJPDAF->objects[p1]->state_post, 1, 0, int1.Y);
	}
}

void
GoalsModel::fitNets()
{
	static const float GOAL_POSTS_DISTANCE = 1400.0f;
	static const float GOALS_DISTANCE = 6000.0f;

	HPoint3D v, v2;
	HPoint3D int1, int2, int3, int4;
	HPoint3D virtualP0, virtualP1;

	HPoint2D opponent_2D = {p0NetCenterEstimate.getPositionInRelativeCoordinates().x,
			p0NetCenterEstimate.getPositionInRelativeCoordinates().y,
			1.0f};

	HPoint2D own_2D = {p1NetCenterEstimate.getPositionInRelativeCoordinates().x,
			p1NetCenterEstimate.getPositionInRelativeCoordinates().y,
			1.0f};

	HPoint3D opponent_3D = {p0NetCenterEstimate.getPositionInRelativeCoordinates().x,
			p0NetCenterEstimate.getPositionInRelativeCoordinates().y,
			0.0f,
			1.0f};

	HPoint3D own_3D = {p1NetCenterEstimate.getPositionInRelativeCoordinates().x,
			p1NetCenterEstimate.getPositionInRelativeCoordinates().y,
			0.0f,
			1.0f};

	// calculate the vector between p0 and p1
	calcVector2D( opponent_2D, own_2D, v );

	//Calculate the intersection between the vector and a circumference
	// with center p0 and radius GOAL_POST_DISTANCE
	HPoint3D circunf_center0 = {opponent_2D.x, opponent_2D.y, 0.0f, 1.0f};
	calIntersectionCircleVector( v, circunf_center0, GOALS_DISTANCE, int1, int2 );
	// Reject one intersection (the one that is at a longer distance from the other post.
	if (calcDistance3D(int1, own_3D) < calcDistance3D(int2, own_3D))
		virtualP1 = int1;
	else
		virtualP1 = int2;

	//Calculate the intersection between the vector and a circumference
	// with center p1 and radius GOAL_POST_DISTANCE
	HPoint3D circunf_center1 = {own_2D.x, own_2D.y, 0.0f, 1.0f};
	calIntersectionCircleVector( v, circunf_center1, GOALS_DISTANCE, int1, int2 );
	// Reject one intersection (the one that is at a longer distance from the other post.
	if (calcDistance3D(int1, opponent_3D) < calcDistance3D(int2, opponent_3D))
		virtualP0 = int1;
	else
		virtualP0 = int2;

	// Middle point between p0 and vP1
	float x0 = (opponent_2D.x + virtualP1.X) / 2.0f;
	float y0 = (opponent_2D.y + virtualP1.Y) / 2.0f;

	// Middle point between p1 and vP0
	float x1 = (own_2D.x + virtualP0.X) / 2.0f;
	float y1 = (own_2D.y + virtualP0.Y) / 2.0f;

	// Calculate an intermediate point between (x0,y0) and (x1,y1) according
	// the uncertainty of each net.
	MatrixCM *x1m = new MatrixCM(2,2);
	MatrixCM *x2m = new MatrixCM(2,2);
	MatrixCM *centerm = new MatrixCM(2,2);
	MatrixCM *c1m = new MatrixCM(2,2);
	MatrixCM *c2m = new MatrixCM(2,2);

	x1m->e(0,0) = x0;
	x1m->e(1,0) = y0;

	x2m->e(0,0) = x1;
	x2m->e(1,0) = y1;

	// ToDo: change this
	c1m->e(0,0) = 500;
	c1m->e(0,1) = 0;
	c1m->e(1,0) = 0;
	c1m->e(1,1) = 500;

	c2m->e(0,0) = 500;
	c2m->e(0,1) = 0;
	c2m->e(1,0) = 0;
	c2m->e(1,1) = 500;

	//Set new middle net point
	*centerm = (*x1m) + (((*c1m) * (inverse((*c1m) + (*c2m)))) * ((*x2m) - (*x1m)));

	// Move center of the nets
	HPoint3D circunf_center3 = {centerm->e(0,0), centerm->e(1,0), 0.0f, 1.0f};
	calIntersectionCircleVector( v, circunf_center3, GOALS_DISTANCE / 2.0f, int1, int2 );

	if (calcDistance3D(int1, opponent_3D) < calcDistance3D(int2, opponent_3D)){
		p0NetCenterEstimate.setPositionAndVelocityInRelativeCoordinates(
				Vector2<double>(int1.X,int1.Y));
		p1NetCenterEstimate.setPositionAndVelocityInRelativeCoordinates(
				Vector2<double>(int2.X,int2.Y));

	} else {
		p0NetCenterEstimate.setPositionAndVelocityInRelativeCoordinates(
				Vector2<double>(int2.X,int2.Y));
		p1NetCenterEstimate.setPositionAndVelocityInRelativeCoordinates(
				Vector2<double>(int1.X,int1.Y));
	}
}

void
GoalsModel::generateVirtualNet(HPoint3D p0, HPoint3D p1, HPoint3D &vp0, HPoint3D &vp1 )
{
	static const float GOAL_POSTS_DISTANCE = 1400.0f;
	static const float GOALS_DISTANCE = 6000.0f;
	HPoint3D v, v2, vc, int1, int2, vp03D, vp13D;
	HPoint3D ORIGIN = {0.0f, 0.0f, 0.0f, 1.0f};

	// Middle point c between p0 and p1
	HPoint3D c = {(p0.X + p1.X) / 2.0f, (p0.Y + p1.Y) / 2.0f, (p0.Z + p1.Z) / 2.0f, 1.0f};

	// Middle point vc of the opposite virtual net
	calcNormalVector2D(p0, p1, c, v);
	calIntersectionCircleVector(v, c, GOALS_DISTANCE, int1, int2);
	if (calcDistance3D(int1, ORIGIN) < calcDistance3D(int2, ORIGIN)){
		vc.X = int1.X;
		vc.Y = int1.Y;
	}else{
		vc.X = int2.X;
		vc.Y = int2.Y;
	}

	vc.Z = c.Z;
	vc.H = 1.0f;

	// Get the normal vector 'v2' to 'v' in point 'vc'
	calcNormalVector2D(c, vc, vc, v2);

	// Get the virtual opposite posts
	calIntersectionCircleVector(v2, vc, GOAL_POSTS_DISTANCE / 2.0f, vp0, vp1);
}


void
GoalsModel::predict()
{
	this->yellowJPDAF->predict();
	updateEstimate();

	// Time elapsed since last net observations
	p0ElapsedTimeSinceLastObs = (getCurrentTime() - p0LastMeasurement) / 1000.0f;
	p1ElapsedTimeSinceLastObs = (getCurrentTime() - p1LastMeasurement) / 1000.0f;
}

void
GoalsModel::updateFromObservation(int color, list<AbstractSample> features)
{


	this->yellowJPDAF->correct( features );

	updateEstimate();

	//Veo una portería
	if((p0LeftEstimate.getReliability() == ObjectState::MOST_RELIABLE) &&
	   (p0RightEstimate.getReliability() == ObjectState::MOST_RELIABLE) &&
	   (p1LeftEstimate.getReliability() == ObjectState::MOST_RELIABLE) &&
	   (p1RightEstimate.getReliability() == ObjectState::MOST_RELIABLE))
	{
		p0LastMeasurement = getCurrentTime();
		p1LastMeasurement = getCurrentTime();
	}else
		if((p0LeftEstimate.getReliability() == ObjectState::MOST_RELIABLE) ||
		   (p0RightEstimate.getReliability() == ObjectState::MOST_RELIABLE))
		{
			p0LastMeasurement = getCurrentTime();
		}
		else
			p1LastMeasurement = getCurrentTime();




}

void
GoalsModel::updateFromOdometry()
{
	this->yellowJPDAF->correctFromOdometry();
	//ownJPDAF->correctFromOdometry();

	// Update the elapsed time since the object was seen (ms.)
	p0ElapsedTimeSinceLastObs = (getCurrentTime() - p0LastMeasurement) / 1000.0f;
	p1ElapsedTimeSinceLastObs = (getCurrentTime() - p1LastMeasurement) / 1000.0f;

	updateEstimate();
	//fitNets();
	//updateEstimate();
}

bica::ShapeList
GoalsModel::getGrDebugRel()
{
	pthread_mutex_lock(&(yellowJPDAF->mutex));

	JPDAF *jpdaf = yellowJPDAF;
	bica::ColorType color = bica::YELLOW;
	string key = "y", label;

	shapeListRel.clear();

	// JPDAF
	ShapeList auxList = yellowJPDAF->getGrDebugRel();
	//Insert the JPDAF's shape list into my shape list
	shapeListRel.insert(shapeListRel.end(), auxList.begin(), auxList.end());

	// Frustum
	vector<HPoint3D> frustumCorners(4);
	kinematics->getFrustum (frustumCorners[0], frustumCorners[1], frustumCorners[3], frustumCorners[2]);
	float vertx[4];
	float verty[4];

	for (unsigned i = 0; i < 3; i++) {
		vertx[i] = frustumCorners[i].X;
		verty[i] = frustumCorners[i].Y;


		bica::Point3DPtr p(new bica::Point3D);
		bica::Point3DPtr p1(new bica::Point3D);

		p->x = (float)(frustumCorners[i].X);
		p->y = (float)(frustumCorners[i].Y);
		p->z = 0.0f;

		p1->x = (float)(frustumCorners[i + 1].X);
		p1->y = (float)(frustumCorners[i + 1].Y);
		p1->z = 0.0f;

		bica::LinePtr line(new bica::Line);
		line->p1 = p1;
		line->p2 = p;
		line->width = 1;
		line->color = bica::BLACK;
		line->filled = false;
		line->opacity = 125;
		line->accKey = "f";
		line->label = "";
		shapeListRel.push_back(line);
	}

	vertx[3] = frustumCorners[3].X;
	verty[3] = frustumCorners[3].Y;

	bica::Point3DPtr p(new bica::Point3D);
	bica::Point3DPtr p1(new bica::Point3D);

	p->x = (float)(frustumCorners[3].X);
	p->y = (float)(frustumCorners[3].Y);
	p->z = 0.0f;

	p1->x = (float)(frustumCorners[0].X);
	p1->y = (float)(frustumCorners[0].Y);
	p1->z = 0.0f;

	bica::LinePtr line(new bica::Line);
	line->p1 = p1;
	line->p2 = p;
	line->width = 1;
	line->color = bica::BLACK;
	line->filled = false;
	line->opacity = 125;
	line->accKey = "f";
	line->label = "";
	shapeListRel.push_back(line);

	// Center of the frustum
	float frustumCenterX = (vertx[0] + vertx[1] + vertx[2] + vertx[3]) / 4;
	float frustumCenterY = (verty[0] + verty[1] + verty[2] + verty[3]) / 4;
	bica::Point3DPtr pF(new bica::Point3D);
	bica::CirclePtr cF(new bica::Circle);

	pF->x = frustumCenterX;
	pF->y = frustumCenterY;
	pF->z = 0.0f;

	cF->center = pF;
	cF->radius = 50.0f;
	cF->z = 0.0f;
	cF->color = color;
	cF->filled = true;
	cF->opacity = 255;
	cF->accKey = key;
	stringstream ss;//create a stringstream
	ss << label << "fc";
	cF->label = ss.str();
	shapeListRel.push_back(cF);

	// Label posts
	int p0l, p0r, p1l, p1r;
	labelPosts(p0l, p0r, p1l, p1r);

	for (unsigned i = 0; i < 4; i++ ) { // Num posts
		// Post
		bica::Point3DPtr p(new bica::Point3D);
		p->x = cvmGet(jpdaf->objects[i]->state_post, 0, 0);
		p->y = cvmGet(jpdaf->objects[i]->state_post, 1, 0);
		p->z = 0.0f;

		bica::EllipsePtr estEllipse(new bica::Ellipse);
		estEllipse->center = p;
		estEllipse->width = 4 * sqrt(cvmGet(jpdaf->objects[i]->error_cov_post, 0, 0));
		estEllipse->length = 4 * sqrt(cvmGet(jpdaf->objects[i]->error_cov_post, 1, 1));
		estEllipse->angle =  toDegrees(atan2(cvmGet(jpdaf->objects[i]->state_post, 1, 0),
				cvmGet(jpdaf->objects[i]->state_post, 0, 0)));
		estEllipse->z = 0.0f;
		estEllipse->color = color;

		// Check if its inside the field of view
		if (!kinematics->pointInPolygon (4, vertx, verty, p->x, p->y))
			estEllipse->color = bica::RED;

		estEllipse->filled = true;
		estEllipse->opacity = 125;
		estEllipse->accKey = key;
		stringstream ss;//create a stringstream
		//if (i == leftPost)
		if (p0l == i) {
			label = "P0L(";
		} else if (p0r == i) {
			label = "P0R(";
		} else if (p1l == i) {
			label = "P1L(";
		} else if (p1r == i) {
			label = "P1R(";
		} else
			label = "Wrong label!";
		ss << label << i << ")";
		estEllipse->label = ss.str();
		shapeListRel.push_back(estEllipse);
	}

	// Center of the net
	for (int i = 0; i < 2; i++) {
		ObjectState *center;
		if (i == 0) {
			center = &p0NetCenterEstimate;
			label = "P0center";
		} else {
			center = &p1NetCenterEstimate;
			label = "P1center";
		}
		bica::Point3DPtr p1(new bica::Point3D);
		bica::CirclePtr c1(new bica::Circle);

		p1->x = center->getPositionInRelativeCoordinates().x;
		p1->y = center->getPositionInRelativeCoordinates().y;;
		p1->z = 0.0f;

		c1->center = p1;
		c1->radius = 20.0f;
		c1->z = 0.0f;
		c1->color = color;
		c1->filled = true;
		c1->opacity = 125;
		c1->accKey = key;
		stringstream ss;//create a stringstream
		ss << label;
		c1->label = ss.str();
		shapeListRel.push_back(c1);
	}

	pthread_mutex_unlock(&(yellowJPDAF->mutex));
	//pthread_mutex_unlock(&(blueJPDAF->mutex));

	return shapeListRel;
}

float
GoalsModel::evaluatePosts (int *posts)
{
	static const float GOAL_POSTS_DISTANCE = 1400.0f;
	static const float GOALS_DISTANCE = 6000.0f;

	float result = 0.0f;

	float p0l_x = cvmGet(this->yellowJPDAF->objects[posts[0]]->state_post, 0, 0);
	float p0l_y = cvmGet(this->yellowJPDAF->objects[posts[0]]->state_post, 1, 0);
	float p0r_x = cvmGet(this->yellowJPDAF->objects[posts[1]]->state_post, 0, 0);
	float p0r_y = cvmGet(this->yellowJPDAF->objects[posts[1]]->state_post, 1, 0);
	float p1l_x = cvmGet(this->yellowJPDAF->objects[posts[2]]->state_post, 0, 0);
	float p1l_y = cvmGet(this->yellowJPDAF->objects[posts[2]]->state_post, 1, 0);
	float p1r_x = cvmGet(this->yellowJPDAF->objects[posts[3]]->state_post, 0, 0);
	float p1r_y = cvmGet(this->yellowJPDAF->objects[posts[3]]->state_post, 1, 0);

	// Check distance between P0L and P0R
	float c1 = calcDistance2D(p0l_x, p0l_y, p0r_x, p0r_y);

	// Check distance beween P1L and P1R
	float c2 = calcDistance2D(p1l_x, p1l_y, p1r_x, p1r_y);

	// Check distance between P0L and P1L
	float c3 = calcDistance2D(p0l_x, p0l_y, p1l_x, p1l_y);

	// Check distance between P0R and P1R
	float c4 = calcDistance2D(p0r_x, p0r_y, p1r_x, p1r_y);

	result = fabs(c1 - GOAL_POSTS_DISTANCE) + fabs(c2 - GOAL_POSTS_DISTANCE) +
			fabs(c3 - GOALS_DISTANCE) + fabs(c4 - GOALS_DISTANCE);

	// Extra penalty if left and right P0 does not fit
	float p0l_angle = atan2(p0l_y, p0l_x);
	float p0r_angle = atan2(p0r_y, p0r_x);

	if ((((p0l_angle < 0) && (p0r_angle >= 0)) ||
			((p0l_angle >= 0) && (p0r_angle < 0))) &&
			(fabs(p0l_angle) > toRadians(90.0f))){
		if (p0l_angle < p0r_angle) {	// It fits!
			result += 0;
		} else {								// It doesn't fit -> Penalty!
			result += 500.0f;
		}
	}else{

		if (p0l_angle < p0r_angle) {			// It doesn't fit -> Penalty!
			result += 500.0f;
		} else {								// It fits!
			result += 0;
		}
	}

	// Extra penalty if left and right P1 does not fit
	float p1l_angle = atan2(p1l_y, p1l_x);
	float p1r_angle = atan2(p1r_y, p1r_x);

	if ((((p1l_angle < 0) && (p1r_angle >= 0)) ||
			((p1l_angle >= 0) && (p1r_angle < 0))) &&
			(fabs(p1l_angle) > toRadians(90.0f))){
		if (p1l_angle < p1r_angle) {	// It fits!
			result += 0;
		} else {								// It doesn't fit -> Penalty!
			result += 500.0f;
		}
	}else{

		if (p1l_angle < p1r_angle) {			// It doesn't fit -> Penalty!
			result += 500.0f;
		} else {								// It fits!
			result += 0;
		}
	}

	return result;
}

void
GoalsModel::labelPosts(int &p0l, int &p0r, int &p1l, int &p1r)
{
	// [p0l, p0r, p1l, p1r]
	int posts[4];
	Masks m;
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

	float minValue = 999999999.0f;	// Max points obtained
	int best = 0;			// Mask index

	for (int k = 0; k < m.size(); k++) {
		Mask mask = m[k];

		for (int i = 0; i < mask.size(); i++) {
			posts[i] = mask[i] - 1;	// Each cell of posts is an object
		}

		float points = evaluatePosts(posts);	// Evaluate this alternative

		if (points < minValue) {	// Better option found!
			minValue = points;
			best = k;
		}
	}

	// Get the best combination and return results
	Mask mask = m[best];
	p0l = mask[0] - 1;
	p0r = mask[1] - 1;
	p1l = mask[2] - 1;
	p1r = mask[3] - 1;

}
