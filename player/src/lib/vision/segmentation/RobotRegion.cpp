/*
 * Name: RobotRegion.cpp
 * @Author: Gonzalo Abella (abellagonzalo@gmail.com)
 *
 * Description: Class to check if a robot region is valid.
 *
 * Copyright (C) 2011-2012 Universidad Rey Juan Carlos
 * All Rights Reserved.
 */

#include "RobotRegion.h"

RobotRegion::RobotRegion()
{
	_ImageHandler = ImageHandler::getInstance();
}

RobotRegion::~RobotRegion()
{
}

bool
RobotRegion::validate(Region *region)
{
	_region = region;

	// Get mass center
	int regionTotalMass, massCenterX, massCenterY;
	_RegionGeometry->getMassCenter(region, regionTotalMass, massCenterX, massCenterY);
	massCenter.x = massCenterX;
	massCenter.y = massCenterY;
	massCenter.h = 1.0;

	// Get orientation
	_RegionGeometry->getOrientation(region, regionTotalMass, massCenter.x, massCenter.y, orientation);

	// Calculate area
	if (!calculateMarkerArea(massCenter)) {
		return false;
	}

	// Calculate white pixels environment
	HPoint2D centerPoint;
	centerPoint.x = massCenter.x;
	centerPoint.y = topLeftCorner.y;
	centerPoint.h = 1.0;
	int height = 2*(bottomLeftCorner.y - topLeftCorner.y);
	float whitePixelPercentage = (calculateWhitePixelsUpwards(topLeftCorner, height, false) +
								  calculateWhitePixelsUpwards(centerPoint, height, false) +
								  calculateWhitePixelsUpwards(topRightCorner, height, false) +
								  calculateWhitePixelsDownwards(bottomLeftCorner, height, false) +
								  calculateWhitePixelsDownwards(bottomRightCorner, height, false) ) / 5;
	if (whitePixelPercentage < WHITE_PIXEL_THRESHOLD) {
		//cerr << "Me tira el porcentaje de blancos con un valor de " << whitePixelPercentage << endl;
		return false;
	}

		// Calculate ground hypothesis point
	if (!calculateGroundHypothesisPixel(centerPoint)) {
		//cerr << "Me tira el calculo de la hipotesis suelo" << endl;
		return false;
	}

	return true;
}

void
RobotRegion::drawRegion(IplImage* src) {
	// Draw marker's area
	CvPoint topLeft, bottomRight;
	topLeft.x = topLeftCorner.x;
	topLeft.y = topLeftCorner.y;
	bottomRight.x = bottomRightCorner.x;
	bottomRight.y = bottomRightCorner.y;
	cvRectangle(src, topLeft, bottomRight, CV_RGB(0,255,255), CV_FILLED);

	// Draw ground hypothesis pixel
	CvPoint ghPoint;
	ghPoint.x = groundHypothesisPixel.x;
	ghPoint.y = groundHypothesisPixel.y;
	cvCircle(src, ghPoint, 3.0, CV_RGB(0,255,0), CV_FILLED);

	// Draw environment points
	whiteEnvironmentPoints.clear();
	nonWhiteEnvironmentPoints.clear();

	HPoint2D centerPoint;
	centerPoint.x = massCenter.x;
	centerPoint.y = topLeftCorner.y;
	centerPoint.h = 1.0;
	int height = 2*(bottomLeftCorner.y - topLeftCorner.y);
	calculateWhitePixelsUpwards(topLeftCorner, height, true);
	calculateWhitePixelsUpwards(centerPoint, height, true);
	calculateWhitePixelsUpwards(topRightCorner, height, true);
	calculateWhitePixelsDownwards(bottomLeftCorner, height, true);
	calculateWhitePixelsDownwards(bottomRightCorner, height, true);

	list<HPoint2D>::iterator iter = whiteEnvironmentPoints.begin();
	while (iter != whiteEnvironmentPoints.end()) {
		CvPoint point;
		point.x = iter->x;
		point.y = iter->y;
		cvLine(src, point, point, CV_RGB(0, 255, 0), 1, 8);
		iter++;
	}

	iter = nonWhiteEnvironmentPoints.begin();
	while (iter != nonWhiteEnvironmentPoints.end()) {
		CvPoint point;
		point.x = iter->x;
		point.y = iter->y;
		cvLine(src, point, point, CV_RGB(255, 0, 0), 1, 8);
		iter++;
	}
}

bool
RobotRegion::isHorizontal() {
	return true;
}

bool
RobotRegion::calculateGroundHypothesisPixel(HPoint2D pixel) {

	// Buscamos el primer pixel verde en linea recta hacia abajo
	while (pixel.y < ImageInput::IMG_HEIGHT-2) {

		if (_ImageHandler->getColor(pixel.x, pixel.y) == ImageInput::CGREEN) {

			bool miniAreaIsGreen = true;
			for (int x=pixel.x-2; x<=pixel.x+2; x++) {
				for (int y=pixel.y; y<=pixel.y+2; y++) {
					if (_ImageHandler->getColor(x,y) != ImageInput::CGREEN) {
						miniAreaIsGreen = false;
					}
				}
			}
			if (miniAreaIsGreen) break;
		}
		pixel.y++;
	}

	// Comprobamos que se haya encontrado un verde
	if (pixel.y >= ImageInput::IMG_HEIGHT) {
		return false;
	}

	
	
	// Guardamos el pixel
	groundHypothesisPixel = pixel;

	return true;
}


bool
RobotRegion::calculateMarkerArea(HPoint2D center) {

	int markerColor = _ImageHandler->getColor(center.x, center.y);

	// Buscamos el primer pixel distinto del color de la muñequera por la izquierda
	HPoint2D leftPixel = center;
	while ( (leftPixel.x > 0) && 
			(_ImageHandler->getColor(leftPixel.x, leftPixel.y) == markerColor) )
 	{
		leftPixel.x--;
	}
	if (leftPixel.x > 0) {
		leftPixel.x++;
	}

	HPoint2D rightPixel = center;
	while ( (rightPixel.x < ImageInput::IMG_WIDTH-1) && 
			(_ImageHandler->getColor(rightPixel.x, rightPixel.y) == markerColor) )
 	{
		rightPixel.x++;
	}
	if (rightPixel.x < ImageInput::IMG_WIDTH-1) {
		rightPixel.x--;
	}

	HPoint2D topPixel = center;
	while ( (topPixel.y > 0) && 
			(_ImageHandler->getColor(topPixel.x, topPixel.y) == markerColor) )
 	{
		topPixel.y--;
	}
	if (topPixel.y > 0) {
		topPixel.y++;
	}

	HPoint2D bottomPixel = center;
	while ( (bottomPixel.y < ImageInput::IMG_HEIGHT-1) && 
			(_ImageHandler->getColor(bottomPixel.x, bottomPixel.y) == markerColor) )
 	{
		bottomPixel.y++;
	}
	if (bottomPixel.y < ImageInput::IMG_HEIGHT-1) {
		bottomPixel.y--;
	}

	// Top left corner pixel
	topLeftCorner.x = leftPixel.x;
	topLeftCorner.y = topPixel.y;
	topLeftCorner.h = 1.0;
	//cerr << "Top left corner pixel " << topLeftCorner.x << ", " << topLeftCorner.y << endl;

	// Bottom left corner pixel
	bottomLeftCorner.x = leftPixel.x;
	bottomLeftCorner.y = bottomPixel.y;
	bottomLeftCorner.h = 1.0;
	//cerr << "Bottom left corner pixel " << bottomLeftCorner.x << ", " << bottomLeftCorner.y << endl;
	
	// Top right corner pixel
	topRightCorner.x = rightPixel.x;
	topRightCorner.y = topPixel.y;
	topRightCorner.h = 1.0;
	//cerr << "Top right corner pixel " << topRightCorner.x << ", " << topRightCorner.y << endl;

	// Bottom right corner pixel
	bottomRightCorner.x = rightPixel.x;
	bottomRightCorner.y = bottomPixel.y;
	bottomRightCorner.h = 1.0;
	//cerr << "Bottom right corner pixel " << bottomRightCorner.x << ", " << bottomRightCorner.y << endl;

	// Check area size
	int area = (bottomPixel.y - topPixel.y) * (rightPixel.x - leftPixel.x);
	if (area < REGION_MIN_SIZE || area > REGION_MAX_SIZE) {
		return false;
	}

	return true;
}

float
RobotRegion::calculateWhitePixelsUpwards(HPoint2D startPixel, int height, bool debug) {

	HPoint2D endPixel;
	endPixel.x = startPixel.x;
	endPixel.y = startPixel.y - 2*height;
	if (endPixel.y<0) endPixel.y=0;

	int whitePixelcounter = 0;
	int totalPixelCounter = 2*height;

	HPoint2D currentPixel = startPixel;
	while (currentPixel.y > endPixel.y) {

		if (_ImageHandler->getColor(currentPixel.x, currentPixel.y) == ImageInput::CWHITE) {
			whitePixelcounter++;
			if (debug) {
				whiteEnvironmentPoints.push_back(currentPixel);
			}
		} else {
			if (debug) {
				nonWhiteEnvironmentPoints.push_back(currentPixel);
			}
		}
		currentPixel.y--;
	}

	//cerr << "El resultado parcial es " << whitePixelcounter << " / " << totalPixelCounter << endl;
	return (float)whitePixelcounter/(float)totalPixelCounter;
}

float
RobotRegion::calculateWhitePixelsDownwards(HPoint2D startPixel, int height, bool debug) {

	HPoint2D endPixel;
	endPixel.x = startPixel.x;
	endPixel.y = startPixel.y + 2*height;
	if (endPixel.y>ImageInput::IMG_HEIGHT-1) endPixel.y=ImageInput::IMG_HEIGHT-1;

	int whitePixelcounter = 0;
	int totalPixelCounter = 2*height;

	HPoint2D currentPixel = startPixel;
	while (currentPixel.y < endPixel.y) {

		if (_ImageHandler->getColor(currentPixel.x, currentPixel.y) == ImageInput::CWHITE) {
			whitePixelcounter++;
			if (debug) {
				whiteEnvironmentPoints.push_back(currentPixel);
			}
		} else {
			if (debug) {
				nonWhiteEnvironmentPoints.push_back(currentPixel);
			}
		}
		currentPixel.y++;
	}

	//cerr << "El resultado parcial es " << whitePixelcounter << " / " << totalPixelCounter << endl;
	return (float)whitePixelcounter/(float)totalPixelCounter;
}
