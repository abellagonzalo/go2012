/*
 * Name: RegionBuilder.cpp
 * @Author: Eduardo Perdices (eperdices@gsyc.es)
 *
 * Description: Class to create regions with the Segments.
 *
 * Copyright (C) 2009-2010 Universidad Rey Juan Carlos
 * All Rights Reserved.
 */

#include "RegionBuilder.h"

const float RegionBuilder::LINE_ANGLE_DIFF = 0.3;

RegionBuilder::RegionBuilder()
{
	_Segmentation = Segmentation::getInstance();
}

RegionBuilder::~RegionBuilder()
{
}

list<BallRegion*> *
RegionBuilder::getBallRegions()
{
	return &(this->balls);
}

list<LineRegion*> *
RegionBuilder::getLineRegions()
{
	return &(this->lines);
}

list<LineRegion*> *
RegionBuilder::getObstaclesRegions()
{
	return &(this->obstacle_lines);
}

list<RobotRegion*> *
RegionBuilder::getTeammateRegions()
{
	return &(this->teammates);
}

list<RobotRegion*> *
RegionBuilder::getOpponentRegions()
{
	return &(this->opponents);
}

list<Segment> *
RegionBuilder::getGoalVSegments(int col)
{
	return _Segmentation->getGoalVSegments(col);
}

list<Segment> *
RegionBuilder::getGoalHSegments(int row)
{
	return _Segmentation->getGoalHSegments(row);
}

void
RegionBuilder::createRegions()
{
	int i;
	list<Segment> * segments;

	/*Reset regions*/
	this->clearRegions();

	/*Get the segments of every column*/
	for(i=0;i<ImageInput::IMG_STEPS_V;i++) {
		segments = _Segmentation->getSegments(i);

		/*Create regions with every segment*/
		for(list<Segment>::iterator seg = segments->begin(); seg != segments->end(); seg++) {
			/*Skip green segments*/
			if((*seg).color == ImageInput::CGREEN)
				continue;

			/*Connect the current segment with segments in the previous column*/
			this->updateRegions(&(*seg), i);
		}
	}
}

void
RegionBuilder::validateRegions(int color)
{
	/*Reset valid regions of this color*/
	this->clearValidRegions(color);

	for(list<Region*>::iterator reg = regions.begin(); reg != regions.end(); reg++) {
		if(color == ImageInput::CWHITE && (*reg)->color == ImageInput::CWHITE) {
			LineRegion * line = new LineRegion();

			if(line->validate(*reg))
				this->lines.push_back(line);
			else {
				/*Check if it can be an obstacle*/
				if(line->validate_obstacle(*reg))
					this->obstacle_lines.push_back(line);
				else
					delete line;
			}
		}
		if(color == ImageInput::CORANGE && (*reg)->color == ImageInput::CORANGE) {
			BallRegion * ball = new BallRegion();

			if(ball->validate(*reg))
				this->balls.push_back(ball);
			else
				delete ball;
		}

		int teammateColor, opponentColor;
		//teammateColor = ImageInput::CCYAN;
		//opponentColor = ImageInput::CMAGENTA;
		if (GameController::getInstance()->getMyColor() == TEAM_BLUE) {
			teammateColor = ImageInput::CCYAN;
			opponentColor = ImageInput::CMAGENTA;
		} else {
			teammateColor = ImageInput::CMAGENTA;
			opponentColor = ImageInput::CCYAN;
		}


		if ((color==teammateColor) && (*reg)->color==teammateColor) {
			RobotRegion *robot = new RobotRegion();
			if (robot->validate(*reg)) {
				this->teammates.push_back(robot);
			} else {
				delete robot;
			}
		}

		if ((color==opponentColor) && (*reg)->color == opponentColor) {
			RobotRegion *robot = new RobotRegion();
			if (robot->validate(*reg)) {
				this->opponents.push_back(robot);
			} else {
				delete robot;
			}
		}
	}
}

void
RegionBuilder::updateRegions(Segment * segment, int index)
{
	list<Region*> neighbors;
	list<Segment> * segments;
	int lastcol = index-1;

	/*If it's the first column, create a new region*/
	if(index==0) {
		this->createNewRegion(segment);
		return;
	}

	/*Search segments in the previous column in the same position with the same color*/
	segments = _Segmentation->getSegments(lastcol);
	for(list<Segment>::iterator seg = segments->begin(); seg != segments->end(); seg++) {	
		/*If the segments are together*/
		if(	(*seg).yend >= segment->miny && (*seg).y <= segment->maxy ) {
			/*If have the same color try to fusion*/
			if((*seg).color == segment->color) {
				/*Fusion segments in a region if it's possible, else add neighbor*/
				if(!this->fusionSegments(segment, &(*seg)))
					neighbors.push_back((*seg).region);
			} else {
				/*Add neighbor, if it already has a region (it's not green)*/
				if((*seg).region != NULL)
					neighbors.push_back((*seg).region);		
			}
		}
	}

	/*If the segment couldn't be connected to a region, create a new region*/
	if(segment->region == NULL)
		this->createNewRegion(segment);

	/*Save neighbors*/
	for(list<Region*>::iterator reg = neighbors.begin(); reg != neighbors.end(); reg++) {
		(*reg)->neighbors.push_back(segment->region);
		segment->region->neighbors.push_back(*reg);
	}
}

void
RegionBuilder::createNewRegion(Segment * segment)
{
	Region * region = new Region();

	segment->region = region;
	region->init = segment;
	region->nsegments = 1;
	region->total_size = segment->length;
	region->color = segment->color;
	region->neighbors.clear();

	/*Save the region*/
	regions.push_back(region);
}

bool
RegionBuilder::fusionSegments(Segment * newsegment, Segment * lastsegment)
{
	/*If it's a line, validate fusion*/
	if(newsegment->color == ImageInput::CWHITE) {

		/*Check if newsegment has already a region*/
		if(newsegment->region != NULL)
			return false;

		/*Check if lastsegment has already a segment in the current column*/
		if(lastsegment->next != NULL) {
			return false;
		}

		/*If the size of the region in too long, create a new region, to avoid curves*/
		if(lastsegment->region->nsegments >= MAX_LINE_REGION_SIZE)
			return false;	
	
		/*Check the length change*/
		if(newsegment->length < lastsegment->length && newsegment->length*LINE_LENGTH_FACTOR < lastsegment->length)
			return false;
		if(lastsegment->length < newsegment->length && lastsegment->length*LINE_LENGTH_FACTOR < newsegment->length)
			return false;

		/*Check the line direction if the lastsegment is not the first one of the region*/
		newsegment->anglePrevious = calcAngleSegments(lastsegment, newsegment);
		if(lastsegment->previous != NULL) {
			/*The angle has to be similar to the previous one*/
			if(fabs(lastsegment->anglePrevious - newsegment->anglePrevious) > LINE_ANGLE_DIFF)
				return false;
		}
	}

	/*If it's a goal, validate fusion*/
	if(newsegment->color == ImageInput::CBLUE || newsegment->color == ImageInput::CYELLOW) {
	
		/*Check the length change*/
		if(newsegment->length < lastsegment->length && newsegment->length*GOAL_LENGTH_FACTOR < lastsegment->length)
			return false;
		if(lastsegment->length < newsegment->length && lastsegment->length*GOAL_LENGTH_FACTOR < newsegment->length)
			return false;
	}

	/*Merge regions if segments are divided*/
	if(newsegment->region != NULL || lastsegment->next != NULL) {
		/*TODO: En el caso de otros elementos que no son líneas, hacer un merge entre regiones cuando los dos tengan ya una, puesto que no es tan importante la forma que tengan*/
		return false;
	}

	/*Fusion segments*/
	lastsegment->next = newsegment;
	newsegment->previous = lastsegment;
	newsegment->region = lastsegment->region;
	newsegment->region->nsegments++;
	newsegment->region->total_size += newsegment->length;
	
	return true;
}

float
RegionBuilder::calcAngleSegments(Segment * segment1, Segment * segment2)
{
	float x,y;

	x = (float)(segment2->x - segment1->x);
	y = (float)((segment2->y+segment2->length/2) - (segment1->y+segment1->length/2));


	if(x==0)
		return pi_2;

	return atan(y/x);
}

void
RegionBuilder::clearRegions()
{
	if(!this->regions.empty())
	{
		/*Delete all the regions*/
		for(list<Region*>::iterator reg = this->regions.begin(); reg != this->regions.end(); reg++)
			delete (*reg);

		this->regions.clear();
	}
}

void
RegionBuilder::clearValidRegions(int color)
{
	if(color == ImageInput::CWHITE) {
		if(!this->lines.empty())
		{
			/*Delete all line regions*/
			for(list<LineRegion*>::iterator line = this->lines.begin(); line != this->lines.end(); line++)
				delete (*line);

			this->lines.clear();
		}
		if(!this->obstacle_lines.empty())
		{
			/*Delete all nonline regions*/
			for(list<LineRegion*>::iterator line = this->obstacle_lines.begin(); line != this->obstacle_lines.end(); line++)
				delete (*line);

			this->obstacle_lines.clear();
		}
	}

	if(color == ImageInput::CORANGE) {
		if(!this->balls.empty())
		{
			/*Delete all ball regions*/
			for(list<BallRegion*>::iterator ball = this->balls.begin(); ball != this->balls.end(); ball++)
				delete (*ball);

			this->balls.clear();
		}
	}

	if (color == ImageInput::CCYAN) {
		// Delete all robot regions
		list<RobotRegion*>::iterator robot=teammates.begin();
		while (robot != teammates.end()) {
			delete(*robot);
			robot++;
		}
		teammates.clear();
	}

	if (color == ImageInput::CMAGENTA) {
		// Delete all robot regions
		list<RobotRegion*>::iterator robot=opponents.begin();
		while (robot != opponents.end()) {
			delete(*robot);
			robot++;
		}
		opponents.clear();
	}	
}

void
RegionBuilder::drawRegions(IplImage* src)
{
	CvPoint p, q;
	Segment * seg;

	/*General Regions*/
	for(list<Region*>::iterator reg = regions.begin(); reg != regions.end(); reg++) {

		if((*reg)->color == ImageInput::CBLUE || (*reg)->color == ImageInput::CYELLOW)
			continue;

		seg = (*reg)->init;

		/*Initial line*/
		p.x = seg->x;
		p.y = seg->y;
		q.x = seg->x;
		q.y = seg->yend;
		this->drawLine(src, p, q, (*reg)->color);	

		while(seg->next!=NULL) {
			p.x = seg->x;
			p.y = seg->y;
			q.x = seg->next->x;
			q.y = seg->next->y;
			this->drawLine(src, p, q, (*reg)->color);	

			p.x = seg->x;
			p.y = seg->yend;
			q.x = seg->next->x;
			q.y = seg->next->yend;
			this->drawLine(src, p, q, (*reg)->color);			
			seg = seg->next;		
		}

		/*Last line*/
		p.x = seg->x;
		p.y = seg->y;
		q.x = seg->x;
		q.y = seg->yend;
		this->drawLine(src, p, q, (*reg)->color);	
	}

	/*Ball Regions*/
	for(list<BallRegion*>::iterator ball = balls.begin(); ball != balls.end(); ball++) {
		(*ball)->drawRegion(src);
	}

	/*Line Regions*/
	for(list<LineRegion*>::iterator line = lines.begin(); line != lines.end(); line++) {
		(*line)->drawRegion(src);
	}

	/*Obstacle Regions*/
	for(list<LineRegion*>::iterator line = obstacle_lines.begin(); line != obstacle_lines.end(); line++) {
		(*line)->drawRegion(src);
	}

	// Teammate regions
	list<RobotRegion*>::iterator teammate = teammates.begin();
	while (teammate != teammates.end()) {
		(*teammate)->drawRegion(src);
		teammate++;
	}

	// Opponent regions
	list<RobotRegion*>::iterator opponent = opponents.begin();
	while (opponent != opponents.end()) {
		(*opponent)->drawRegion(src);
		opponent++;
	}
	
}

void
RegionBuilder::drawLine(IplImage* src, CvPoint p, CvPoint q, int color) {

	if(color == ImageInput::CUNKNOWN) {
		cvLine(src, p, q, CV_RGB(255,0,0), 1, CV_AA, 0);
	}
	if(color == ImageInput::CORANGE) {
		cvLine(src, p, q, CV_RGB(0,95,255), 1, CV_AA, 0);
	}
	if(color == ImageInput::CGREEN) {
		cvLine(src, p, q, CV_RGB(0,255,0), 1, CV_AA, 0);
	}
	if(color == ImageInput::CBLUE) {
		cvLine(src, p, q, CV_RGB(0,255,255), 1, CV_AA, 0);
	}
	if(color ==  ImageInput::CYELLOW) {
		cvLine(src, p, q, CV_RGB(255,255,0), 1, CV_AA, 0);
	}
	if(color == ImageInput::CWHITE) {
		cvLine(src, p, q, CV_RGB(0,0,0), 1, CV_AA, 0);
	}
	if(color == ImageInput::CCYAN) {
		cvLine(src, p, q, CV_RGB(0,255,255), 1, CV_AA, 0);
	}
	if(color == ImageInput::CMAGENTA) {
		cvLine(src, p, q, CV_RGB(255,0,255), 1, CV_AA, 0);
	}
}

