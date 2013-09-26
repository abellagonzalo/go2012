/*
 * Name: RegionGeometry.cpp
 * @Author: Eduardo Perdices (eperdices@gsyc.es)
 *
 * Description: Class to calculate the Geometry of the Region.
 *
 * Copyright (C) 2009-2010 Universidad Rey Juan Carlos
 * All Rights Reserved.
 */

#include "RegionGeometry.h"

RegionGeometry::RegionGeometry()
{
}

RegionGeometry::~RegionGeometry()
{
}

void
RegionGeometry::getMassCenter(Region * region, int &total, int &x, int &y)
{
	int m10, m01;

	/*Get total mass and moments of 1st order*/
	total = region->calcMoment00();
	m10 = region->calcMoment10();
	m01 = region->calcMoment01();

	/*Get mass center from moments*/
	x = m10/total;
	y = m01/total;
}

bool
RegionGeometry::getOrientation(Region * region, int total_mass, int mass_x, int mass_y, float &alpha)
{

	float m20, m02, m11;
	float cm20, cm02;

	/*Get moments with mass*/
  m20 = (float) region->calcMoment20(mass_x),
  m02 = (float) region->calcMoment02(mass_y),
  m11 = (float) region->calcMoment11(mass_x, mass_y);

	cm20 = m20/total_mass;
  cm02 = m02/total_mass;

	if(cm20 != cm02) {
		/*Calculate alpha*/
		alpha = .5f * atan2f(-2 * ((float)m11),(float)(m02-m20))+(float)pi_2;
		return true;
	} else {
		/*Can't calculate orientation*/
		alpha = 0.0;
		return false;
	}
}
