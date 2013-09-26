/*
 * Name: RegionBuilder.cpp
 * @Author: Eduardo Perdices (eperdices@gsyc.es)
 *
 * Description: Class to create regions with the Segments.
 *
 * Copyright (C) 2009-2010 Universidad Rey Juan Carlos
 * All Rights Reserved.
 */

#ifndef _REGIONBUILDER_H_
#define _REGIONBUILDER_H_

#include <list>
#include "Singleton.h"
#include "ImageInput.h"
#include "Segmentation.h"
#include "Segment.h"
#include "Region.h"
#include "BallRegion.h"
#include "LineRegion.h"
#include "RobotRegion.h"
#include "GameController.h"

using namespace std;

class RegionBuilder: public Singleton<RegionBuilder>
{
public:
	RegionBuilder();
	~RegionBuilder();

	/*Create Regions with the Segments*/
	void createRegions();

	/*Check the regions created and classificates it*/
	void checkRegions();

	/*Validate all the regions of the type selected*/
	void validateRegions(int color);

	/*Get all ball regions*/
	list<BallRegion*> * getBallRegions();

	/*Get all line regions*/
	list<LineRegion*> * getLineRegions();

	/*Get all obstacle regions*/
	list<LineRegion*> * getObstaclesRegions();

	/*Get all robots regions*/
	list<RobotRegion*> *getTeammateRegions();
	list<RobotRegion*> *getOpponentRegions();

	/*Get the vertical segments of Goals*/
	list<Segment> * getGoalVSegments(int col);

	/*Get the horizontal segments of Goals*/
	list<Segment> * getGoalHSegments(int row);

	/*Draw the Regions in the image*/
	void drawRegions(IplImage* src);

private:

	/*Connect the segment with the regions created or create new regions*/
	void updateRegions(Segment * segment, int index);
	/*Creates a new region from a segment*/
	void createNewRegion(Segment * segment);
	/*Fusion two regions*/
	bool fusionSegments(Segment * newsegment, Segment * lastsegment);
	/*Calc angle between two segments*/
	float calcAngleSegments(Segment * segment1, Segment * segment2);
	/*Clear the regions already created*/
	void clearRegions();
	/*Clear the valid regions already created*/
	void clearValidRegions(int color);

	/*Draw a line in the image*/
	void drawLine(IplImage* src, CvPoint p, CvPoint q, int color);

	static const int MAX_LINE_REGION_SIZE = 7;
	static const int LINE_LENGTH_FACTOR = 4;
	static const float LINE_ANGLE_DIFF;
	static const int GOAL_LENGTH_FACTOR = 6;

	list<Region*> regions;
	list<BallRegion*> balls;
	list<LineRegion*> lines;
	list<LineRegion*> obstacle_lines;
	list<RobotRegion*> teammates;
	list<RobotRegion*> opponents;

	Segmentation * _Segmentation;
};

#endif //_REGIONBUILDER_H_
