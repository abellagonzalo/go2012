/*
 * Name: Segmentation.cpp
 * @Author: Eduardo Perdices (eperdices@gsyc.es)
 *
 * Description: Class to segment the image into Segments.
 *
 * Copyright (C) 2009-2010 Universidad Rey Juan Carlos
 * All Rights Reserved.
 */

#ifndef _SEGMENTATION_H_
#define _SEGMENTATION_H_

#include <list>
#include "Singleton.h"
#include "ImageInput.h"
#include "ImageHandler.h"
#include "Horizon.h"
#include "Segment.h"
#include "BodyDetector.h"

using namespace std;

class Segmentation: public Singleton<Segmentation>
{
public:
	Segmentation();
	~Segmentation();

	/*Get the list with the Segments*/
	list<Segment> * getSegments(int col);

	/*Get the vertical segments of the Goal*/
	list<Segment> * getGoalVSegments(int col);

	/*Get the horizontal segments of the Goal*/
	list<Segment> * getGoalHSegments(int row);

	/*Analyze the image and save the Segments*/
	void analyzeImage(Horizon * horizon);

	/*Draw the Segments in the image*/
	void drawSegments(IplImage* src);

private:

	/*Calc horizontal segments*/
	void calcHorizontalSegments(Horizon * horizon);
	/*Calc vertical segments*/
	void calcVerticalSegments(Horizon * horizon);
	/*Create a new segment*/
	void createSegment(list<Segment> * list, int x, int y, int size, int color, int col, bool vertical);

	/*Draw a line in the image*/
	void drawLine(IplImage* src, CvPoint p, CvPoint q, int color);

	static const int MIN_SEGMENT_SIZE = 2;
	static const int MIN_SEGMENT_SIZE_GOAL = 2;	

	list<Segment> segments[ImageInput::IMG_STEPS_V];
	list<Segment> goalVSegments[ImageInput::IMG_STEPS_V];
	list<Segment> goalHSegments[ImageInput::IMG_STEPS_H];	

	ImageHandler * _ImageHandler;
	BodyDetector * _BodyDetector;
};

#endif //_SEGMENTATION_H_
