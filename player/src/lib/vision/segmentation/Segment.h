/*
 * Name: Segment.h
 * @Author: Eduardo Perdices (eperdices@gsyc.es)
 *
 * Description: Class to collect up several pixels aligned with the same color.
 *
 * Copyright (C) 2009-2010 Universidad Rey Juan Carlos
 * All Rights Reserved.
 */

#ifndef _SEGMENT_H_
#define _SEGMENT_H_

#include <iostream>
#include "Region.h"

using namespace std;

class Region;

class Segment
{
public:

	Segment();
	~Segment();

	int x;				/*Start column of the Segment*/
	int xend;			/*End column of the Segment*/
	int y;				/*Start row of the Segment*/
	int yend;			/*End row of the Segment*/
	int miny;			/*Min row in the previous columns (to touch the previous Segments)*/
	int maxy;			/*Max row in the previous columns (to touch the previous Segments)*/
	int length;			/*Length of the Segment*/
	int color;			/*Color of the Segment*/
	int indexcol;		/*Index to know in what column was the Segment*/
	Segment * next;		/*Next Segment in the Region*/
	Segment * previous;	/*Previous Segment in the Region*/
	Region * region;	/*Region of the Segment*/
	float anglePrevious;/*Angle between the Segment and the previous one*/
};

#endif //_SEGMENT_H_
