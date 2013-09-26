/*
 * Name: LineRegion.cpp
 * @Author: Eduardo Perdices (eperdices@gsyc.es)
 *
 * Description: Class to check if a ball region is valid.
 *
 * Copyright (C) 2009-2010 Universidad Rey Juan Carlos
 * All Rights Reserved.
 */
#include "BallRegion.h"


BallRegion::BallRegion()
{
	this->region = NULL;

	_Horizon = Horizon::getInstance();
}

BallRegion::~BallRegion()
{
}

bool
BallRegion::validate(Region * region)
{
	int mass_x, mass_y, total_mass;

	//std::cout << "validamos en ball region" << std::endl;

	/*Minimal num segments*/
	if(region->nsegments < MIN_REGION_SIZE_BALL)
		return false;

	/*Obtain the mass center of the region*/
	_RegionGeometry->getMassCenter(region, total_mass, mass_x, mass_y);

	// Region over the horizon
	Point2D massCenter;
	massCenter.x = mass_x;
	massCenter.y = mass_y;
	massCenter.h = 1.0;
	if (_Horizon->isOverHorizon(massCenter))
		return false;
	
	/*Save line values*/
	this->region = region;
	this->cent_x = mass_x;
	this->cent_y = mass_y;

	return true;
}

void
BallRegion::drawRegion(IplImage* src)
{
	CvPoint p;

	p.x = this->cent_x;
	p.y = this->cent_y;

	cvCircle(src, p, 2, CV_RGB(95,0,255), 1);	
}

