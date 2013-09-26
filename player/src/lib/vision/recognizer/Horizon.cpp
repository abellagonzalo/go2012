/*
 * Name: Horizon.cpp
 * @Author: Carlos Agüero (caguero@gsyc.es)
 *
 * Description: Class that processes an image in order to obtain the visual horizon. The horizon is
 * used for adding restrictions while objects are recognized.
 *
 * Copyright (C) 2008-2009 Universidad Rey Juan Carlos
 * All Rights Reserved.
 */

#include "Horizon.h"
#include <stdio.h>
#include <iostream>

Horizon::Horizon()
{
	_Kinematics = Kinematics::getInstance();
	_ImageHandler = ImageHandler::getInstance();

	cvImg = cvCreateImage( cvSize( ImageInput::IMG_WIDTH, ImageInput::IMG_HEIGHT ), 8, 1 );
}


Horizon::~Horizon()
{
	cvReleaseImage(&cvImg);
}

void
Horizon::process()
{
	float A, B, C;
	int col, row, rowend;
	int marginx = ImageInput::IMG_MARGIN_STEP;
	int marginy = 1;
	int ccolor, size, counter;
	int step = ImageInput::IMG_MINISTEP;

	/*Calculate horizon line using kinematics*/
	this->getHorizonLine(&A, &B, &C);

	counter = 0;
	for(col=marginx;col<ImageInput::IMG_WIDTH;col=col+ImageInput::IMG_STEP) {

		/*Starts from the horizon line*/
		row = this->getHorizonPos(A, B, C, col);
		while(row <= ImageInput::IMG_HEIGHT-1-marginy) {

			rowend = ImageInput::IMG_HEIGHT-1-marginy;

			ccolor = _ImageHandler->getColor(col, row);
			if(ccolor != ImageInput::CGREEN) {
				row+=step;
				continue;
			}

			/*Get the size of the green color*/
			size = _ImageHandler->getSegmentSizeDown(col, row, rowend, ccolor);
			if(size >= NUM_GREEN_PIXELS)
				break;

			row+=size;
		}

		borders[counter].x = (float) col;
		borders[counter].y = (float) row;
		borders[counter].h = 1.0;
		
		counter++;
	}

	/*Calculate convex hull*/
	this->calcConvexHull();
}

void
Horizon::calcConvexHull()
{
	int i, j;
	int numelems = 0;

	/*Monotone Chain Convex Hull, select the indexes which create a upper convex hull*/
	for(i=0;i<ImageInput::IMG_STEPS_V;i++) {
		/*While upperHull contains at least two points and the cross product does not make a clockwise turn:
		crossproduct between the last two points saved in the upperHull and the new point*/
		while (numelems >= 2 && crossproduct(borders[upperHull[numelems-2]], borders[upperHull[numelems-1]], borders[i]) <= 0)
			numelems--;

		/*Add the point to the upperHull*/
		upperHull[numelems] = i;
		numelems++;
	}

	/*Calculate new field border with the selected indexes, Note: first column and last column are always selected*/
	int before, next, found;
  for(i = 1; i < ImageInput::IMG_STEPS_V-1; i++ ) {
		before = 0;
		next = ImageInput::IMG_STEPS_V-1;	
		found = 0;
		for(j = numelems-1; j>=0 && !found; j--) {
			//Point selected in upperHull
			if(upperHull[j] == i)
				found = 1;
			else if(upperHull[j] < i && upperHull[j] > before)
				before = upperHull[j];
			else if(upperHull[j] > i && upperHull[j] < next)
					next = upperHull[j];
		}
		/*Calculate average*/
		if(!found)
			borders[i].y = (int) borders[before].y + (borders[next].y-borders[before].y)*(borders[i].x-borders[before].x)/(borders[next].x-borders[before].x);
	}	
}

int
Horizon::crossproduct(HPoint2D O, HPoint2D A, HPoint2D B) {
	return (A.x - O.x) * (B.y - O.y) - (A.y - O.y) * (B.x - O.x);
}

void
Horizon::getHorizonLine(float * A, float * B, float *C)
{
	HPoint3D p3d1, p3d2;
	HPoint2D p2d1, p2d2;	

	//Obtain kinematics horizon
	p3d1.X = 6050.0;
	p3d1.Y = 2000.0;
	p3d1.Z = 0.0;
	p3d1.H = 1.0;
	p3d2.X = 6050.0;
	p3d2.Y = -2000.0;
	p3d2.Z = 0.0;
	p3d2.H = 1.0;

	_Kinematics->get2DPosition(p3d1, p2d1);
	_Kinematics->get2DPosition(p3d2, p2d2);

	//Calculate line equation Ax+By+X=0, cross product
	*A = p2d1.y - p2d2.y; //y1*z2 - z1*y2
	*B = p2d2.x - p2d1.x; //z1*x2 - x1*z2
	*C = p2d1.x*p2d2.y - p2d1.y*p2d2.x; //x1*y2 - y1*x2
}

int
Horizon::getHorizonPos(float A, float B, float C, int x)
{
	float res;
	int row;

	if(B==0)
		return 0;

	//Solve Ax+By+C=0
	res = -C-A*(float)x;

	if (B<1.0) B = 1.0;
	row = (int)res/(int)B;
	if(row < 0)
		row = 0;

	return row;
}

bool
Horizon::isUnderHorizon(Point2D point)
{
	return !this->isOverHorizon(point);
}

bool
Horizon::isOverHorizon(Point2D point)
{
	int step, idxM, idxm;
	int margin = 1;

	if(point.x >= ImageInput::IMG_WIDTH || point.x < 0 || point.y >= ImageInput::IMG_HEIGHT || point.y < 0)
		return false;

	step = ImageInput::IMG_STEP;

	idxm = ((int)(point.x / (float)step));
	idxM = ((int)(point.x / (float)step)) + 1;

	if(idxm<0) idxm = 0;
	if(idxM>(ImageInput::IMG_STEPS_V - 1)) idxM = ImageInput::IMG_STEPS_V - 1;

	if((point.y < borders[idxm].y+margin) || (point.y < borders[idxM].y+margin))
		return true;
	else
		return false;
}

int
Horizon::distance2horizon(Point2D point)   //Check please !!!
{
	float d1, d2;
	int step;
	int idxM, idxm;

	if(point.x >= ImageInput::IMG_WIDTH || point.y >= ImageInput::IMG_HEIGHT)
		return false;

	step = ImageInput::IMG_STEP;

	idxm = ((int)(point.x / (float)step));
	idxM = ((int)(point.x / (float)step)) + 1;

	if(idxm<0) idxm = 0;
	if(idxM>(ImageInput::IMG_STEPS_V - 1)) idxM = ImageInput::IMG_STEPS_V - 1;

	d1 = (borders[idxm].y - point.y);
	d2 = (borders[idxM].y - point.y);

	if(d1 < d2)
		return (int)d1;
	else
		return (int)d2;

}

int
Horizon::getFieldBorderCol(int col)
{
	if(col >= ImageInput::IMG_STEPS_V || col < 0)
		return 0;

	return (int) borders[col].y;
}

int
Horizon::getHorizonRow()
{
	HPoint3D p3d;
	HPoint2D p2d;

	//Obtain kinematics horizon in front of the robot
	p3d.X = 6050.0;
	p3d.Y = 0.0;
	p3d.Z = 0.0;
	p3d.H = 1.0;

	_Kinematics->get2DPosition(p3d, p2d);

	return p2d.y;
}

char *
Horizon::getImageProcessed()
{
	return this->getCvHorizon()->imageData;
}

IplImage*
Horizon::getCvHorizon()
{
	float x1, y1, x2, y2;

	cvZero(cvImg);

	for(int i = 0; i < (ImageInput::IMG_STEPS_V - 1); i++)
	{
		//creeamos la recta con el punto "i" e "i+1"
		x1 = borders[i].x;
		y1 = borders[i].y;
		x2 = borders[i+1].x;
		y2 = borders[i+1].y;

		cvLine(cvImg, cvPoint((int)x1,(int)y1), cvPoint((int)x2,(int)y2), CV_RGB(0.,0.,255.));

	}	
	
	return cvImg;
}

void
Horizon::drawFieldBorder(IplImage* src)
{
	float x1, y1, x2, y2;

	for(int i = 0; i < (ImageInput::IMG_STEPS_V - 1); i++)
	{
		/*Create a line with the points i and i+1*/
		x1 = borders[i].x;
		y1 = borders[i].y;
		x2 = borders[i+1].x;
		y2 = borders[i+1].y;

		cvLine(src, cvPoint((int)x1,(int)y1), cvPoint((int)x2,(int)y2), CV_RGB(0.0,0.0,0.0), 2, CV_AA, 0);
	}
}
