/*
 * Name: RobotRegion.cpp
 * @Author: Gonzalo Abella (abellagonzalo@gmail.com)
 *
 * Description: Class to check if a robot region is valid.
 *
 * Copyright (C) 2011-2012 Universidad Rey Juan Carlos
 * All Rights Reserved.
 */

#ifndef _ROBOTREGION_H_
#define _ROBOTREGION_H_

#include "AbstractRegion.h"
#include <list>

class RobotRegion: public AbstractRegion {
public:
	RobotRegion();
	~RobotRegion();

	// Check if the line is valid
	bool validate(Region * region);

	// Draw the line in the image
	void drawRegion(IplImage* src);

	// Get orientation of the region
	bool isHorizontal();

	// Get mass centered
	HPoint2D getMassCenter() { return massCenter; };

	// Get ground hypothesis pixel
	HPoint2D getGroundHypothesisPixel() { return groundHypothesisPixel; };

	// Get corners of the area
	HPoint2D getTopLeftCorner() { return topLeftCorner; };
	HPoint2D getBottomLeftCorner() { return bottomLeftCorner; };
	HPoint2D getTopRightCorner() { return topRightCorner; };
	HPoint2D getBottomRightCorner() { return bottomRightCorner; };

private:

	HPoint2D massCenter;
	HPoint2D groundHypothesisPixel;
	HPoint2D topLeftCorner;
	HPoint2D bottomLeftCorner;
	HPoint2D topRightCorner;
	HPoint2D bottomRightCorner;
	
	float orientation;
	Region *_region;

	static const int REGION_MIN_SIZE = 24;
	static const int REGION_MAX_SIZE = 10000;
	static const float WHITE_PIXEL_THRESHOLD = 0.7;
	static const int GROUND_HYPOTHESIS_MIN_AREA_PIXELS = 3;

	bool calculateGroundHypothesisPixel(HPoint2D pixel);
	bool calculateMarkerArea(HPoint2D center);
	float calculateWhitePixelsUpwards(HPoint2D startPixel, int height, bool debug);
	float calculateWhitePixelsDownwards(HPoint2D startPixel, int height, bool debug);

	ImageHandler *_ImageHandler;
	
	// Points for debug
	list<HPoint2D> whiteEnvironmentPoints;
	list<HPoint2D> nonWhiteEnvironmentPoints;
};

#endif //_ROBOTREGION_H_
