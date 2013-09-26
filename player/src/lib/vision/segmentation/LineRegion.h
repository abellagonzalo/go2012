/*
 * Name: LineRegion.cpp
 * @Author: Eduardo Perdices (eperdices@gsyc.es)
 *
 * Description: Class to check if a line region is valid.
 *
 * Copyright (C) 2009-2010 Universidad Rey Juan Carlos
 * All Rights Reserved.
 */

#ifndef _LINEREGION_H_
#define _LINEREGION_H_

#include <list>
#include "AbstractRegion.h"

class LineRegion: public AbstractRegion {
public:
	LineRegion();
	~LineRegion();

	/*Check if the line is valid*/
	bool validate(Region * region);

	/*Check if a line is an obstacle*/
	bool validate_obstacle(Region * region);

	/*Draw the line in the image*/
	void drawRegion(IplImage* src);

	int cent_x;
	int cent_y;
	float alpha;
	bool vertical;
	float dist_height;
	float dist_width;
	HPoint2D ps2D, pe2D;
	HPoint3D ps3D, pe3D;
	float real_alpha;
	float real_dist;
	HPoint3D real_cent;
	HPoint3D normal;
	bool is_obstacle;

private:

	/*Check if exists green above the region*/
	int checkGreenUp(Region * region);
	/*Check if exists green below the region*/
	int checkGreenDown(Region * region);
	/*Check if exists green left the region*/
	int checkGreenLeft(Region * region);
	/*Check if exists green right the region*/
	int checkGreenRight(Region * region);
	/*Check minimum distance to horizon above the region*/
	bool checkMinimumDistanceUp(Region * region);
	/*Get the green in a column between two points*/
	int getGreenColumn(int i, int yini, int yend);
	/*Obtain distances to axis and start-end points*/
	void getLineAxis(Region * region);

	static const int MIN_REGION_SIZE_LINE = 2;	
	static const int MIN_TOTAL_SIZE_LINE = 9;
	static const int MIN_TOTAL_SIZE_SINGLE_LINE = 30;
	static const int MAX_UNKNOWN_SIZE = 525;
	static const float MAX_UNKNOWN_RATIO;
	static const float UNKNOWN_ALPHA;

	static const int MIN_GREEN_HORIZONTAL = 28;
	static const int MIN_GREEN_VERTICAL = 20;
	static const int MAX_GREEN_SKIP = 4;
	static const int MIN_DISTANCE_UP = 4;

	static const int MIN_TOTAL_SIZE_OBSTACLE = 50;	
	static const float MIN_OBSTACLE_RATIO;

	Horizon		* _Horizon;
};

#endif //_LINEREGION_H_
