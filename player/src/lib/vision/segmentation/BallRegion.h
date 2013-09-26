/*
 * Name: LineRegion.cpp
 * @Author: Eduardo Perdices (eperdices@gsyc.es)
 *
 * Description: Class to check if a ball region is valid.
 *
 * Copyright (C) 2009-2010 Universidad Rey Juan Carlos
 * All Rights Reserved.
 */

#ifndef _BALLREGION_H_
#define _BALLREGION_H_

#include "AbstractRegion.h"

class BallRegion: public AbstractRegion {
public:
	BallRegion();
	~BallRegion();

	/*Check if the ball is valid*/
	bool validate(Region * region);

	/*Draw the ball in the image*/
	void drawRegion(IplImage* src);

	Region * region;
	int cent_x;
	int cent_y;

private:

	static const int MIN_REGION_SIZE_BALL = 1;
	Horizon		* _Horizon;
};

#endif //_BALLREGION_H_
