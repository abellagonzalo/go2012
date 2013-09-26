/*
 * Name: Horizon.h
 * @Author: Carlos Agüero (caguero@gsyc.es)
 *
 * Description: Class that processes an image in order to obtain the visual horizon. The horizon is
 * used for adding restrictions while objects are recognized.
 *
 * Copyright (C) 2008-2009 Universidad Rey Juan Carlos
 * All Rights Reserved.
 */

#ifndef _HORIZON_H_
#define _HORIZON_H_

#include "Singleton.h"
#include "../colorFilter/ColorFilter.h"
#include "Kinematics.h"
#include "ImageHandler.h"
#include "ImageInput.h"

using namespace std;

class Horizon: public Singleton<Horizon>
{
public:
	Horizon();
	~Horizon();

	char *getImageProcessed();

	IplImage* getCvHorizon();

	void process();

	bool isUnderHorizon(Point2D point);

	bool isOverHorizon(Point2D point);

	int distance2horizon(Point2D point);

	int getFieldBorderCol(int col);

	/*Get the row where the horizon is set*/
	int getHorizonRow();

	/*Draw the field border calculated in the image*/
	void drawFieldBorder(IplImage* src);

protected:

	void getHorizonLine(float * A, float * B, float *C);
	int getHorizonPos(float A, float B, float C, int x);

	/*Calculates the upper convex hull of the points*/
	void calcConvexHull();

	/*Return a positive value, if OAB makes a counter-clockwise turn, negative for clockwise turn, and zero if the points are collinear.*/
	int crossproduct(HPoint2D O, HPoint2D A, HPoint2D B);

	static const int NUM_GREEN_PIXELS = 4;

	HPoint2D borders[ImageInput::IMG_STEPS_V];
	int upperHull[ImageInput::IMG_STEPS_V];

	Kinematics *_Kinematics;
	ImageHandler * _ImageHandler;

	IplImage *cvImg;
};

#endif //_HORIZON_H_
