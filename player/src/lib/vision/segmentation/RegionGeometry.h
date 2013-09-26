/*
 * Name: RegionGeometry.cpp
 * @Author: Eduardo Perdices (eperdices@gsyc.es)
 *
 * Description: Class to calculate the Geometry of the Region.
 *
 * Copyright (C) 2009-2010 Universidad Rey Juan Carlos
 * All Rights Reserved.
 */

#ifndef _REGIONGEOMETRY_H_
#define _REGIONGEOMETRY_H_

#include "almath/tools/almath.h"
#include "Singleton.h"
#include "Segment.h"
#include "Region.h"
#include "Common.h"
#include "Geometry.h"

using namespace std;

class RegionGeometry: public Singleton<RegionGeometry>
{
public:
	RegionGeometry();
	~RegionGeometry();

	/*Calc the mass center of the region*/
	void getMassCenter(Region * region, int &total, int &x, int &y);

	/**
 	* Get the orientation with the mass of a region.
 	*
 	* @param region The region to calculate orientation.
 	* @param total_mass Mass of the region.
 	* @param mass_x X coordinate of the mass center's region.
	* @param mass_y Y coordinate of the mass center's region.
	* @param alpha Return value with the orientation in radians. 
	* @return True if the calculate has been done, False if not and alpha=0.0.
 	*/
	bool getOrientation(Region * region, int total_mass, int mass_x, int mass_y, float &alpha);
};

#endif //_REGIONGEOMETRY_H_
