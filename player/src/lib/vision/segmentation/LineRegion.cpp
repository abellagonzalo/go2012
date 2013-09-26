/*
 * Name: LineRegion.cpp
 * @Author: Eduardo Perdices (eperdices@gsyc.es)
 *
 * Description: Class to check if a line region is valid.
 *
 * Copyright (C) 2009-2010 Universidad Rey Juan Carlos
 * All Rights Reserved.
 */

#include "LineRegion.h"

const float LineRegion::MAX_UNKNOWN_RATIO = 0.5;
const float LineRegion::UNKNOWN_ALPHA = 1000000.0;
const float LineRegion::MIN_OBSTACLE_RATIO = 2.0;

LineRegion::LineRegion()
{
	this->vertical = false;
	this->is_obstacle = false;
	this->alpha = UNKNOWN_ALPHA;
	_Horizon = Horizon::getInstance();
}

LineRegion::~LineRegion()
{
}

bool
LineRegion::validate(Region * region)
{
	/*To be a line, it has to fulfill some filters:
	- Orientation must be determinable
	- Certain number of segments with big size, of 1 segment very long (vertical lines)
	- It can't have a big number of "unknown color" segments as neighbors (it usually means that it's a robot)
	- It must be between green segments (right-left in vertical lines and up-down in horizontal lines)
	*/

	int unknownsize = 0;
	int total_mass;

	/*Get mass center and orientation*/
	_RegionGeometry->getMassCenter(region, total_mass, this->cent_x, this->cent_y);
	if(!_RegionGeometry->getOrientation(region, total_mass, this->cent_x, this->cent_y, this->alpha)) {
		this->alpha = UNKNOWN_ALPHA;
		return false;
	}

	// Region over the horizon
	Point2D massCenter;
	massCenter.x = cent_x;
	massCenter.y = cent_y;
	massCenter.h = 1.0;
	if (_Horizon->isOverHorizon(massCenter))
		return false;

	/*Check if it's "vertical", angle between 45º and 135º*/
	this->vertical = fabs(this->alpha) > M_PI/4.0 && fabs(this->alpha) < 3.0*M_PI/4.0;
	
	/*Minimal size*/
	if(region->total_size < MIN_TOTAL_SIZE_LINE)
		return false;

	/*If it hasn't enough segments, it must be very long*/
	if(region->nsegments < MIN_REGION_SIZE_LINE && region->total_size < MIN_TOTAL_SIZE_SINGLE_LINE)
		return false;

	/*Check the neighbors of the region, if it has unknown regions it can be an obstacle. Note: Neighbors repeted*/
	for(list<Region*>::iterator reg = region->neighbors.begin(); reg != region->neighbors.end(); reg++) {
		if((*reg)->color == ImageInput::CUNKNOWN)
			unknownsize += (*reg)->total_size;
	}
	if(unknownsize > MAX_UNKNOWN_SIZE || ((float)unknownsize/(float)region->total_size) > MAX_UNKNOWN_RATIO)
		return false;

	/*Green around the line*/
	if(this->vertical) {
		if(this->checkGreenRight(region) < MIN_GREEN_VERTICAL || this->checkGreenLeft(region) < MIN_GREEN_VERTICAL)
			return false;
		/*Must be some distance to the horizon (if not, it can be a leg of a robot)*/
		if(!this->checkMinimumDistanceUp(region))
			return false;
	} else {
		if(this->checkGreenUp(region) < MIN_GREEN_HORIZONTAL || this->checkGreenDown(region) < MIN_GREEN_HORIZONTAL)
			return false;
	}

	/*Calculate 2D line*/
	this->getLineAxis(region);

	this->is_obstacle = false;
	return true;
}

bool
LineRegion::validate_obstacle(Region * region)
{
	/*To be an obstacle line, it has to fulfill some filters:
	- Orientation must be determinable and vertical
	- Its size must be big
	- Its ratio must be big
	*/

	float ratio;

	/*Check vertical orientation*/
	if(this->alpha == UNKNOWN_ALPHA || !this->vertical)
		return false;

	/*Minimal size*/
	if(region->total_size < MIN_TOTAL_SIZE_OBSTACLE)
		return false;

	/*Calculate 2D line*/
	this->getLineAxis(region);

	/*Check ratio*/
	if(this->dist_height > 0.0 && this->dist_width > 0.0) {
		ratio = this->dist_width/this->dist_height;

		if(ratio < MIN_OBSTACLE_RATIO)
			return false;
	}

	this->is_obstacle = true;
	return true;
}

int
LineRegion::checkGreenUp(Region * region) {

	Segment * seg;
	list<Segment> * lsegments;
	int green = 0;
	int col;
	bool found;

	seg = region->init;
	while(seg!=NULL) {
		col = seg->indexcol;
		found = false;

		/*Search the previous green segment in the same column*/
		lsegments = _Segmentation->getSegments(col);
		for(list<Segment>::reverse_iterator it = lsegments->rbegin(); it != lsegments->rend() && !found; it++) {
			/*Go to the current segment*/
			if((*it).y >= seg->y)
				continue;

			if((*it).color == ImageInput::CGREEN) {
				/*If the green is close to the line, save length of the green segment*/
				if(seg->y - (*it).yend <= MAX_GREEN_SKIP) {
					green += (*it).length;
					/*Optimize search*/
					if(green >= MIN_GREEN_HORIZONTAL)
						return green;
				}
				found = true;
			}
		}	
		seg = seg->next;
	}
	return green;
}

int
LineRegion::checkGreenDown(Region * region) {

	Segment * seg;
	list<Segment> * lsegments;
	int green = 0;
	int col;
	bool found;

	seg = region->init;
	while(seg!=NULL) {
		col = seg->indexcol;
		found = false;

		/*Search the next green segment in the same column*/
		lsegments = _Segmentation->getSegments(col);
		for(list<Segment>::iterator it = lsegments->begin(); it != lsegments->end() && !found; it++) {
			/*Go to the current segment*/
			if((*it).y <= seg->y)
				continue;

			if((*it).color == ImageInput::CGREEN) {
				/*If the green is close to the line, save length of the green segment*/
				if((*it).y - seg->yend <= MAX_GREEN_SKIP) {
					green += (*it).length;
					/*Optimize search*/
					if(green >= MIN_GREEN_HORIZONTAL)
						return green;
				}
				found = true;
			}
		}	
		seg = seg->next;
	}
	return green;
}

int
LineRegion::checkGreenLeft(Region * region) {

	Segment * seg;
	int green = 0;
	int col;

	seg = region->init;
	while(seg!=NULL) {
		col = seg->indexcol-1;
		/*If it's the first colums, green is zero*/
		if(col>=0) {
			/*Cover the next column to find green at the segment side*/
			green += this->getGreenColumn(col, seg->y, seg->yend);

			/*Optimize search*/
			if(green >= MIN_GREEN_VERTICAL)
				return green;
		}
		seg = seg->next;
	}
	return green;
}

int
LineRegion::checkGreenRight(Region * region) {

	Segment * seg;
	int green = 0;
	int col;

	seg = region->init;
	while(seg!=NULL) {
		col = seg->indexcol+1;
		/*If it's the last colums, green is zero*/
		if(col<ImageInput::IMG_STEPS_V) {
			/*Cover the next column to find green at the segment side*/
			green += this->getGreenColumn(col, seg->y, seg->yend);

			/*Optimize search*/
			if(green >= MIN_GREEN_VERTICAL)
				return green;
		}
		seg = seg->next;
	}
	return green;
}

bool
LineRegion::checkMinimumDistanceUp(Region * region) {

	Segment * seg;
	list<Segment> * lsegments;
	int col;
	bool found;

	seg = region->init;
	while(seg!=NULL) {
		col = seg->indexcol;
		found = false;

		/*Check the previous segments in the same column*/
		lsegments = _Segmentation->getSegments(col);
		for(list<Segment>::iterator it = lsegments->begin(); it != lsegments->end() && !found; it++) {
			/*If it's the first segment with known color, distance is not valid*/
			if((*it).y == seg->y)
				return false;

			/*When color is known, check distance*/
			if((*it).color != ImageInput::CUNKNOWN && (*it).color != ImageInput::CWHITE) {
				if(seg->y - (*it).y < MIN_DISTANCE_UP)
					return false;
				else {
					found = true;
					continue;
				}
			}
		}	
		seg = seg->next;
	}
	return true;
}

int
LineRegion::getGreenColumn(int col, int yini, int yend) {

	list<Segment> * lsegments;
	int green = 0;
	int touchi, touche;

	lsegments = _Segmentation->getSegments(col);
	for(list<Segment>::iterator it = lsegments->begin(); it != lsegments->end(); it++) {
		/*If starts after the end of the segment, end search*/
		if((*it).y > yend)
			return green;

		/*Find the green segments touching the current segment*/
		if((*it).yend > yini && (*it).color == ImageInput::CGREEN) {
			/*Calc the number of pixels touching*/
			touchi = ((*it).y > yini) ? (*it).y : yini;
			touche = ((*it).yend < yend) ? (*it).yend : yend;
			if(touche > touchi)
				green += touche - touchi;
		}
	}

	return green;
}

void
LineRegion::getLineAxis(Region * region)
{
	Segment * seg;
	float alphac, alphas, alphac2, alphas2;
	float distx, disty, distyend, dist;
	float height_max, height_min, width_max, width_min;
	
	height_max = height_min = width_max = width_min = 0.0;

	alphac = cos(this->alpha);
	alphas = sin(this->alpha);
	alphac2 = cos(this->alpha+pi_2);
	alphas2 = sin(this->alpha+pi_2);
	
	seg = region->init;
	while(seg!=NULL) {
		distx = seg->x - this->cent_x;
		disty = seg->y - this->cent_y;
		distyend = seg->yend - this->cent_y;

		/*Distance height and width using the orientation*/
		dist = distx*alphac + disty*alphas;
		if(dist > width_max)
			width_max = dist;
		else if(dist < width_min)
			width_min = dist;	
		
		dist = distx*alphac + distyend*alphas;
		if(dist > width_max)
			width_max = dist;
		else if(dist < width_min)
			width_min = dist;	

		dist = distx*alphac2 + disty*alphas2;
		if(dist > height_max)
			height_max = dist;
		else if(dist < height_min)
			height_min = dist;		
		
		dist = distx*alphac2 + distyend*alphas2;
		if(dist > height_max)
			height_max = dist;
		else if(dist < height_min)
			height_min = dist;	

		seg = seg->next;
	}

	/*Save the minimal value*/
	if(fabs(width_min) < width_max)
		this->dist_width = fabs(width_min);
	else
		this->dist_width = width_max;

	if(fabs(height_min) < height_max)
		this->dist_height = fabs(height_min);
	else
		this->dist_height = height_max;

	/*Calc start and end points*/
	this->ps2D.x = (int)(this->cent_x + cos(this->alpha) * width_max);
	this->ps2D.y = (int)(this->cent_y + sin(this->alpha) * width_max);
	this->ps2D.h = 1.0;
	this->pe2D.x = (int)(this->cent_x + cos(this->alpha) * width_min);
	this->pe2D.y = (int)(this->cent_y + sin(this->alpha) * width_min);
	this->pe2D.h = 1.0;
}

void
LineRegion::drawRegion(IplImage* src)
{
	CvPoint p, q;

	/*Draw alpha*/
	p.x  = this->ps2D.x;
	p.y  = this->ps2D.y;
	q.x  = this->pe2D.x;
	q.y  = this->pe2D.y;
	if(this->is_obstacle)
		cvLine(src, p, q, CV_RGB(0,0,255), 1, CV_AA, 0);
	else
		cvLine(src, p, q, CV_RGB(0,255,0), 1, CV_AA, 0);		

	/*Draw mass center*/
	p.x = this->cent_x;
	p.y = this->cent_y;
	cvCircle(src, p, 1, CV_RGB(0,95,255), 1);
}

