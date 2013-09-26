/*
 * Name: Region.h
 * @Author: Eduardo Perdices (eperdices@gsyc.es)
 *
 * Description: Class to collect up several Segments with the same color.
 *
 * Copyright (C) 2009-2010 Universidad Rey Juan Carlos
 * All Rights Reserved.
 */

#ifndef _REGION_H_
#define _REGION_H_

#include <iostream>
#include <list>
#include "Segment.h"
#include "Geometry.h"

using namespace std;

class Segment;

class Region
{
public:

	Region();
	~Region();

	/*Geometric moments*/
	int calcMoment00();
	int calcMoment10();
	int calcMoment01();
	float calcMoment11(int val_x, int val_y);
	int calcMoment20(int val_x);
	float calcMoment02(int val_y);

	Segment * init;				/*First Segment of the Region*/
	int nsegments;				/*Total number of Segments in the Region*/
	int total_size;				/*Size of the Region (sum of pixels of the Segments)*/
	int color;					/*Color of the Region*/
	list<Region*> neighbors;	/*Regions touching the current Region*/		
};

#endif //_REGION_H_
