/*
 * Name: LineDetector.h
 * @Author: Eduardo Perdices (eperdices@gsyc.es)
 * 
 * Description: Class to detect lines from the Regions
 *
 * Created on: 31/05/2010
 *
 * Copyright (C) Universidad Rey Juan Carlos
 * All Rights Reserved.
 *
 */
#include "LineDetector.h"

const float LineDetector::FAREA_ANGLE_THRESHOLD = 0.05;
const float LineDetector::PARALLEL_RANGE_CLEAR = 0.2;
const float LineDetector::PARALLEL_RANGE_UNCLEAR = 0.4;
const float LineDetector::MIN_LINE_SEGMENT_SIZE_CLEAR = 50.0;
const float LineDetector::MIN_LINE_SEGMENT_SIZE_UNCLEAR = 50.0;
const float LineDetector::MAX_LINE_DISTANCE_X = 500.0;
const float LineDetector::MAX_LINE_DISTANCE_Y = 300.0;
const float LineDetector::MIN_LINE_RATIO = 1.2;
const float LineDetector::MIN_SIZE_SINGLE_LINESEGMENT = 300.0;
const float LineDetector::MAX_CIRCLE_DISTANCE_SEGMENTS = 1400.0;
const float LineDetector::CIRCLE_RADIUS = 600.0;
const float LineDetector::CIRCLE_RADIUS_MAX = 800.0;
const float LineDetector::CIRCLE_RADIUS_MIN = 400.0;
const float LineDetector::CIRCLE_RADIUS_DISTANCE = 250.0;
const float LineDetector::ANALYZE_MIN_SIZE = 180.0;
const float LineDetector::ANALYZE_PARALLEL = 0.5;
const float LineDetector::ANALYZE_PARALLEL_SEGMENTS = 0.4;
const float LineDetector::ANALYZE_DIST_X = 1500.0;
const float LineDetector::ANALYZE_DIST_SEGMENTS = 200.0;
const float LineDetector::MAX_MIDLINE_DISTANCE = 1000.0;
const float LineDetector::INTERSECTION_MARGIN = 200.0;
const float LineDetector::INTERSECTION_T_DISTANCE = 150.0;
const float LineDetector::INTERSECTION_PARALLEL_RANGE = 1.2;
const float LineDetector::INTERSECTION_DIST = 250.0;

LineDetector::LineDetector() : Detector()
{
	this->color = ImageInput::CWHITE;
}

LineDetector::~LineDetector()
{
}

void
LineDetector::init(const string newName, AL::ALPtr<AL::ALBroker> parentBroker) {
	Component::init(newName, parentBroker);

	this->setFreqTime(SHORT_RATE);

}

void
LineDetector::step()
{
	//perception->step();

	if (!isTime2Run())
		return;

	startDebugInfo();
	this->detect(true);
	endDebugInfo();
}

void
LineDetector::detect(bool debug)
{
	/*Clear saved elements*/
	this->clearDetected();

	/*Validate the regions*/
	_RegionBuilder->validateRegions(this->color);

	/*Create forbidden area*/
	this->createForbiddenArea();

	/*Validate lines*/
	this->validateLines();

	/*Merge the clear lines*/
	this->mergeLineSegments(PARALLEL_RANGE_CLEAR, MIN_NUM_CANDIDATES_CLEAR, MAX_LINE_DISTANCE_X, MIN_LINE_SEGMENT_SIZE_CLEAR, 1);

	/*Search the circle in the image*/
	this->searchCircle();

	/*Merge the unclear lines*/
	this->mergeLineSegments(PARALLEL_RANGE_UNCLEAR, MIN_NUM_CANDIDATES_UNCLEAR, MAX_LINE_DISTANCE_X, MIN_LINE_SEGMENT_SIZE_UNCLEAR, 1);

	/*Save big single segments*/
	this->searchSingleLineSegments();

	/*Analyze lines detected*/
	this->analyzeLines();

	/*Intersections*/
	this->calculateIntersections();
}

void
LineDetector::createForbiddenArea()
{
	float ang1, ang2, dist1, dist2, alpha;
	bool found;
	LineRegion lineRegion;
	ForbiddenArea farea;
	list<LineRegion*> * oregions = _RegionBuilder->getObstaclesRegions();

	for(list<LineRegion*>::iterator line = oregions->begin(); line != oregions->end(); line++) {

		lineRegion = *(*line);
		found = false;
		
		/*Obtain the points in 3D*/
		lineRegion.ps2D.h = 1.0; 
		lineRegion.pe2D.h = 1.0; 

		_Kinematics->get3DPosition(lineRegion.ps3D, lineRegion.ps2D);
		_Kinematics->get3DPosition(lineRegion.pe3D, lineRegion.pe2D);

		/*Get average angle, use angles close to 0 (+ or -) because is always vertical*/
		ang1 = calcVectorAngle(0.0, 0.0, lineRegion.ps3D.X, lineRegion.ps3D.Y);
		if(ang1 > pi_2) ang1 -= M_PI;
		ang2 = calcVectorAngle(0.0, 0.0, lineRegion.pe3D.X, lineRegion.pe3D.Y);
		if(ang2 > pi_2) ang2 -= M_PI;
		alpha = (ang1 + ang2)/2.0;

		/*Distance from robot*/
		dist1 = calcDistance2D(0.0, 0.0, lineRegion.ps3D.X, lineRegion.ps3D.Y);
		dist2 = calcDistance2D(0.0, 0.0, lineRegion.pe3D.X, lineRegion.pe3D.Y);

		/*Try to fit inside an existing area*/
		for(list<ForbiddenArea>::iterator area = fAreas.begin(); area != fAreas.end() && !found; area++) {
			/*If alpha is inside area*/
			if((*area).alphaL < alpha && (*area).alphaR > alpha) {
				if((dist1 > (*area).dist_start && dist1 < (*area).dist_end) || (dist2 > (*area).dist_start && dist2 < (*area).dist_end) ) {
					/*Save info*/
					if(dist1 < (*area).dist_start)
            (*area).dist_start = dist1;
					if(dist2 > (*area).dist_end)
            (*area).dist_end = dist2;

          if(alpha - FAREA_ANGLE_THRESHOLD < (*area).alphaL)
           (*area).alphaL = alpha - FAREA_ANGLE_THRESHOLD;
          if(alpha + FAREA_ANGLE_THRESHOLD > (*area).alphaR)
           (*area).alphaR = alpha + FAREA_ANGLE_THRESHOLD;
          (*area).size += 1;
					found = true;
				}
			}
		}

		/*Create a new area*/
		if(!found) {
			farea.alphaL = alpha - FAREA_ANGLE_THRESHOLD;
			farea.alphaR = alpha + FAREA_ANGLE_THRESHOLD;
			farea.dist_start = dist1;
			farea.dist_end = dist2;	
			farea.size = 1;
			this->fAreas.push_back(farea);		
		}
	}

	/*Merge areas*/
	for(list<ForbiddenArea>::iterator area1 = fAreas.begin(); area1 != fAreas.end(); area1++) {

		list<ForbiddenArea>::iterator area2 = area1;
		area2++;
		while(area2 != fAreas.end()) {

			/*Check alphas mixed*/
			if(	((*area1).alphaL > (*area2).alphaL && (*area1).alphaL < (*area2).alphaR) || ((*area1).alphaR > (*area2).alphaL && (*area1).alphaR < (*area2).alphaR) || 
					((*area2).alphaL > (*area1).alphaL && (*area2).alphaL < (*area1).alphaR) || ((*area2).alphaR > (*area1).alphaL && (*area2).alphaR < (*area1).alphaR) ) {
				/*Check distances*/
				if(((*area1).dist_start > (*area2).dist_start && (*area1).dist_start < (*area2).dist_end) || ((*area2).dist_start > (*area1).dist_start && (*area2).dist_start < (*area1).dist_end)) {
					if((*area2).alphaL < (*area1).alphaL) 
						(*area1).alphaL = (*area2).alphaL;
					if((*area2).alphaR > (*area1).alphaR) 
						(*area1).alphaR = (*area2).alphaR;
					if((*area2).dist_start < (*area1).dist_start)
						(*area1).dist_start = (*area2).dist_start;
					if((*area2).dist_end > (*area1).dist_end)
						(*area1).dist_end = (*area2).dist_end;
					(*area1).size += (*area2).size;
					/*Delete area*/
					area2 = fAreas.erase(area2);	
				}	
			}

			area2++;
		}
	}

	/*Delete areas too small*/
	list<ForbiddenArea>::iterator area = fAreas.begin();
	while(area != fAreas.end()) {
		if((*area).size < FAREA_MIN_SIZE)
			area = fAreas.erase(area);
		else {
			/*Calculate coordinates 2D and 3D*/
			(*area).ps3DL.Z = (*area).ps3DR.Z = (*area).pe3DL.Z = (*area).pe3DR.Z = 0.0;
			(*area).ps3DL.H = (*area).ps3DR.H = (*area).pe3DL.H = (*area).pe3DR.H = 1.0;

			(*area).ps3DL.X = cos((*area).alphaL) * (*area).dist_start;
			(*area).ps3DL.Y = sin((*area).alphaL) * (*area).dist_start;
			(*area).ps3DR.X = cos((*area).alphaR) * (*area).dist_start;
			(*area).ps3DR.Y = sin((*area).alphaR) * (*area).dist_start;
			(*area).pe3DL.X = cos((*area).alphaL) * (*area).dist_end;
			(*area).pe3DL.Y = sin((*area).alphaL) * (*area).dist_end;
			(*area).pe3DR.X = cos((*area).alphaR) * (*area).dist_end;
			(*area).pe3DR.Y = sin((*area).alphaR) * (*area).dist_end;

			_Kinematics->get2DPosition((*area).ps3DL, (*area).ps2DL);
			_Kinematics->get2DPosition((*area).ps3DR, (*area).ps2DR);
			_Kinematics->get2DPosition((*area).pe3DL, (*area).pe2DL);
			_Kinematics->get2DPosition((*area).pe3DR, (*area).pe2DR);

			area++;
		}
	}
}

void
LineDetector::validateLines()
{
	float ratio;
	float ang1, ang2, alpha, dist;
	bool found;
	LineRegion lineRegion;
	list<LineRegion*> * lregions = _RegionBuilder->getLineRegions();

	for(list<LineRegion*>::iterator line = lregions->begin(); line != lregions->end(); line++) {

		lineRegion = *(*line);

		/*Check the width/heigth ratio*/
		ratio = lineRegion.dist_width/lineRegion.dist_height;
		if(ratio < MIN_LINE_RATIO)
			continue;

		/*Obtain the points in 3D*/
		lineRegion.ps2D.h = 1.0; 
		lineRegion.pe2D.h = 1.0; 

		_Kinematics->get3DPosition(lineRegion.ps3D, lineRegion.ps2D);
		_Kinematics->get3DPosition(lineRegion.pe3D, lineRegion.pe2D);
		lineRegion.real_dist = calcDistance3D(lineRegion.ps3D, lineRegion.pe3D);
		lineRegion.real_cent.X = (lineRegion.ps3D.X + lineRegion.pe3D.X)/2.0;
		lineRegion.real_cent.Y = (lineRegion.ps3D.Y + lineRegion.pe3D.Y)/2.0;
		lineRegion.real_cent.Z = 0.0;

		/*Obtain the real alpha*/
		lineRegion.real_alpha = calcVectorAngle(lineRegion.ps3D.X, lineRegion.ps3D.Y, lineRegion.pe3D.X, lineRegion.pe3D.Y);

		/*Check forbidden area*/
		if(lineRegion.vertical) {
			/*Get average angle, use angles close to 0 (+ or -) because is vertical*/
			ang1 = calcVectorAngle(0.0, 0.0, lineRegion.ps3D.X, lineRegion.ps3D.Y);
			if(ang1 > pi_2) ang1 -= M_PI;
			ang2 = calcVectorAngle(0.0, 0.0, lineRegion.pe3D.X, lineRegion.pe3D.Y);
			if(ang2 > pi_2) ang2 -= M_PI;
			alpha = (ang1 + ang2)/2.0;

			/*Distance to the center of the line*/
			dist = calcDistance2D(0.0, 0.0, lineRegion.real_cent.X, lineRegion.real_cent.Y);

			found = false;
			for(list<ForbiddenArea>::iterator area = fAreas.begin(); area != fAreas.end() && !found; area++) {

				/*If alpha is inside area or very close*/
				if(	((*area).alphaL < alpha && (*area).alphaR > alpha) ||
						fabs(alpha - (*area).alphaL) < FAREA_ANGLE_THRESHOLD ||	fabs(alpha - (*area).alphaR) < FAREA_ANGLE_THRESHOLD) {

					/*Center of the line behind area start*/
					if(dist > (*area).dist_start)
						found = true;
				}					
			}
			if(found)
				continue;	
		}

		/*Save valid line*/
		this->validLines.push_back(lineRegion);		
	}
}

void
LineDetector::mergeLineSegments(float parallel_range, int min_candidates, float max_distance, float min_size, int type)
{
	list<list<LineRegion>::iterator> candidates;
	HPoint3D pstart, pend;
	float max_size;
	bool changed = true;
	float totalpha = 0.0;

	list<LineRegion>::iterator line1 = this->validLines.begin();
	while(line1 != this->validLines.end()) {

		/*Save the values of the current LineRegion*/
		pstart = (*line1).ps3D;
		pend = (*line1).pe3D;
		max_size = (*line1).real_dist;
		totalpha = (*line1).real_alpha;
		changed = false;
		candidates.push_back(line1);

		list<LineRegion>::iterator line2 = line1;
		for(line2++; line2 != this->validLines.end(); line2++) {

			/*They have to be almost parallel*/
			if(!areVectorsParallel((*line1).real_alpha, (*line2).real_alpha, parallel_range))
				continue;

			/*Calc the new angle of the line*/
			if(changed) {
				totalpha = calcVectorAngle(pstart.X, pstart.Y, pend.X, pend.Y);
				changed = false;
			}

			/*Merge lines if are together*/
			if(!this->mergeLines(pstart, pend, (*line2).ps3D, (*line2).pe3D, totalpha, max_distance))
				continue;

			/*Save candidate*/
			if((*line2).real_dist > max_size)
				max_size = (*line2).real_dist;

			changed = true;
			candidates.push_back(line2);
		}

		/*If the line has enough samples with enough size, save new line*/
		if((int)candidates.size() >= min_candidates && max_size > min_size) {

			LineSample lineSample;

			/*Calculate alpha*/
			lineSample.alpha = calcVectorAngle(pstart.X, pstart.Y, pend.X, pend.Y);

			/*Save points in 2D and 3D*/
			lineSample.ps3D = pstart;
			lineSample.pe3D = pend;
			_Kinematics->get2DPosition(pstart, lineSample.ps2D);
			_Kinematics->get2DPosition(pend, lineSample.pe2D);
			lineSample.type = type;
			lineSample.isMidline = false;

			this->lines.push_back(lineSample);

			/*Delete the candidates saved*/
			for(list<list<LineRegion>::iterator>::reverse_iterator lreg = candidates.rbegin(); lreg != candidates.rend(); lreg++) {
				/*Delete and get the next element of the iterator*/
				line1 = this->validLines.erase(*lreg);
			}	
		} else {
			line1++;
		}

		candidates.clear();
	}
}

void
LineDetector::searchCircle()
{
	int count;
	float dist1, dist2;
	CircleSample csample;

	/*Calculate normal vectors of the line regions*/
	for(list<LineRegion>::iterator line1 = this->validLines.begin(); line1 != this->validLines.end(); line1++)
		calcNormalVector2D((*line1).ps3D, (*line1).pe3D, (*line1).real_cent, (*line1).normal);

	/*Calc intersections between line regions*/
	for(list<LineRegion>::iterator line1 = this->validLines.begin(); line1 != this->validLines.end(); line1++) {

		list<LineRegion>::iterator line2 = line1;
		for(line2++; line2 != this->validLines.end(); line2++) {

			/*They can't be very far away*/
			if(calcDistance3D((*line1).real_cent, (*line2).real_cent) > MAX_CIRCLE_DISTANCE_SEGMENTS)
				continue;

			/*Calculate the intersection, if it's possible (they can be parallel)*/
			calcIntersection2D((*line1).normal, (*line2).normal, csample.p3D);
			if(csample.p3D.H == 0.0)
				continue;

			/*Distance between the intersection and the segments has to be similar to the radius of the circle*/
			dist1 = calcDistance3D(csample.p3D, (*line1).real_cent);
			if(dist1 > CIRCLE_RADIUS_MAX || dist1 < CIRCLE_RADIUS_MIN)
				continue;
			dist2 = calcDistance3D(csample.p3D, (*line2).real_cent);
			if(dist2 > CIRCLE_RADIUS_MAX || dist2 < CIRCLE_RADIUS_MIN)
				continue;

			/*Save the intersection as possible circle center*/
			_Kinematics->get2DPosition(csample.p3D, csample.p2D);
			this->circleCenters.push_back(csample);
		}
	}

	/*Count the number of points together in the circle center*/
	this->circle.p3D.X = 0.0;
	this->circle.p3D.Y = 0.0;
	this->circle.p3D.Z = 0.0;
	this->circle.p3D.H = 1.0;
	
	for(list<CircleSample>::iterator circle1 = this->circleCenters.begin(); circle1 != this->circleCenters.end(); circle1++) {

		this->circle.p3D.X = (*circle1).p3D.X;
		this->circle.p3D.Y = (*circle1).p3D.Y;

		count = 1;

		/*Find other points close to the point*/
		list<CircleSample>::iterator circle2 = circle1;
		for(circle2++; circle2 != this->circleCenters.end(); circle2++) {

			if(calcDistance3D((*circle1).p3D, (*circle2).p3D) < CIRCLE_RADIUS_DISTANCE) {
				this->circle.p3D.X += (*circle2).p3D.X;
				this->circle.p3D.Y += (*circle2).p3D.Y;
				count++;
			}
		}

		/*If has enough candidates, save the point and exit*/
		if(count > MIN_NUM_CANDIDATES_CIRCLE) {
			this->circle.p3D.X /= (float)count;
			this->circle.p3D.Y /= (float)count;
			this->circle.midLine = NULL;
			this->circle.found = true;

			break;
		}

	}	

	if(this->circle.found) {

		/*Delete lines of the circle*/
		list<LineRegion>::iterator line = this->validLines.begin();
		while(line != this->validLines.end()) {

			/*Distance between circle center and the segment similar to radius*/
			dist1 = calcDistance3D(this->circle.p3D, (*line).real_cent);
			if(dist1 > CIRCLE_RADIUS_MAX || dist1 < CIRCLE_RADIUS_MIN) {
				line++;
				continue;
			}

			/*Segment has to be perpendicular to the circle center*/
			if(!this->arePerpendicular((*line).ps3D, (*line).pe3D, (*line).real_cent, this->circle.p3D)) {
				line++;
				continue;
			}

			/*The segment is part of the circle, delete it*/
			line = this->validLines.erase(line);
		}

		/*Get 2D position*/
		_Kinematics->get2DPosition(this->circle.p3D, this->circle.p2D);
	}
}

void
LineDetector::searchSingleLineSegments()
{
	LineSample lineSample;

	list<LineRegion>::iterator line = this->validLines.begin();
	while(line != this->validLines.end()) {

		if((*line).real_dist > MIN_SIZE_SINGLE_LINESEGMENT) {
			
			lineSample.alpha = (*line).real_alpha;
			/*Normalize*/
			if(lineSample.alpha < 0)
				lineSample.alpha += M_PI;
			if(lineSample.alpha > M_PI)
				lineSample.alpha -= M_PI;

			/*Save points in 2D and 3D*/
			lineSample.ps3D = (*line).ps3D;
			lineSample.pe3D = (*line).pe3D;
			_Kinematics->get2DPosition((*line).ps3D, lineSample.ps2D);
			_Kinematics->get2DPosition((*line).pe3D, lineSample.pe2D);
			lineSample.type = 2;
			lineSample.isMidline = false;
		
			this->lines.push_back(lineSample);

			/*The segment is a line, delete it*/
			line = this->validLines.erase(line);
		} else
			line++;	
	}
}

void
LineDetector::analyzeLines()
{
	bool changed = true;
	bool updated = false;
	float totalpha = 0.0;
	float dist;

	/*Search parallel segments of the same line*/
	for(list<LineSample>::iterator line1 = this->lines.begin(); line1 != this->lines.end(); line1++) {

		updated = false;
		totalpha = (*line1).alpha;
		changed = false;

		list<LineSample>::iterator line2 = line1;
		line2++;
		while(line2 != this->lines.end()) {

			/*They have to be almost parallel*/
			if(!areVectorsParallel((*line1).alpha, (*line2).alpha, ANALYZE_PARALLEL)) {
				line2++;
				continue;
			}

			/*Calc the new angle of the line*/
			if(changed) {
				totalpha = calcVectorAngle((*line1).ps3D.X, (*line1).ps3D.Y, (*line1).pe3D.X, (*line1).pe3D.Y);
				changed = false;
			}

			/*Merge lines if are together*/
			if(!this->mergeLines((*line1).ps3D, (*line1).pe3D, (*line2).ps3D, (*line2).pe3D, totalpha, ANALYZE_DIST_X)) {
				line2++;
				continue;
			}

			updated = true;
			changed = true;
			line2 = this->lines.erase(line2);
		}

		/*Calc new values*/
		if(updated) {
			(*line1).alpha = calcVectorAngle((*line1).ps3D.X, (*line1).ps3D.Y, (*line1).pe3D.X, (*line1).pe3D.Y);
			_Kinematics->get2DPosition((*line1).ps3D, (*line1).ps2D);
			_Kinematics->get2DPosition((*line1).pe3D, (*line1).pe2D);
			(*line1).type = 3;			
		}
	}

	/*Check if the segments not saved yet are part of a line*/
	for(list<LineSample>::iterator line1 = this->lines.begin(); line1 != this->lines.end(); line1++) {

		updated = false;
		totalpha = (*line1).alpha;
		changed = false;

		list<LineRegion>::iterator line2 = this->validLines.begin();
		while(line2 != this->validLines.end()) {

			/*The size of the region has to be big*/
			if((*line2).real_dist < ANALYZE_MIN_SIZE) {
				line2++;
				continue;
			}

			/*They have to be almost parallel*/
			if(!areVectorsParallel((*line1).alpha, (*line2).real_alpha, ANALYZE_PARALLEL_SEGMENTS)) {
				line2++;
				continue;
			}

			/*Calc the new angle of the line*/
			if(changed) {
				totalpha = calcVectorAngle((*line1).ps3D.X, (*line1).ps3D.Y, (*line1).pe3D.X, (*line1).pe3D.Y);
				changed = false;
			}

			/*Merge lines if are together*/
			if(!this->mergeLines((*line1).ps3D, (*line1).pe3D, (*line2).ps3D, (*line2).pe3D, totalpha, ANALYZE_DIST_SEGMENTS)) {
				line2++;
				continue;
			}

			updated = true;
			changed = true;
			line2 = this->validLines.erase(line2);
		}

		/*Calc new values*/
		if(updated) {
			(*line1).alpha = calcVectorAngle((*line1).ps3D.X, (*line1).ps3D.Y, (*line1).pe3D.X, (*line1).pe3D.Y);
			_Kinematics->get2DPosition((*line1).ps3D, (*line1).ps2D);
			_Kinematics->get2DPosition((*line1).pe3D, (*line1).pe2D);
			(*line1).type = 4;			
		}
	}

	/*Search midline*/
	if(this->circle.found) {

		for(list<LineSample>::iterator line = this->lines.begin(); line != this->lines.end(); line++) {

			float centx = ((*line).ps3D.X + (*line).pe3D.X)/2.0;
			float centy = ((*line).ps3D.Y + (*line).pe3D.Y)/2.0;

			/*Check the center of the line*/
			dist = calcDistance2D(this->circle.p3D.X, this->circle.p3D.Y, centx, centy);
			if(dist <= MAX_MIDLINE_DISTANCE) {
				(*line).isMidline = true;
				this->circle.midLine = &(*line);
				break;
			}
			/*Check the extremes of the line*/
			dist = calcDistance2D(this->circle.p3D.X, this->circle.p3D.Y, (*line).ps3D.X, (*line).ps3D.Y);
			if(dist <= MAX_MIDLINE_DISTANCE) {
				(*line).isMidline = true;
				this->circle.midLine = &(*line);
				break;
			}

			dist = calcDistance2D(this->circle.p3D.X, this->circle.p3D.Y, (*line).pe3D.X, (*line).pe3D.Y);
			if(dist <= MAX_MIDLINE_DISTANCE) {
				(*line).isMidline = true;
				this->circle.midLine = &(*line);
				break;
			}
		}
	}
}

void
LineDetector::calculateIntersections()
{
	IntersectionSample intSample;
	HPoint3D ps1, pe1;
	HPoint3D ps2, pe2;
	HPoint3D int1, int2;
	int isT1, isT2;

	/*Calculate vectors*/
	for(list<LineSample>::iterator line1 = this->lines.begin(); line1 != this->lines.end(); line1++)
		calcVector2D((*line1).ps3D, (*line1).pe3D, (*line1).lineVector);

	/*Intersect not parallel lines*/
	for(list<LineSample>::iterator line1 = this->lines.begin(); line1 != this->lines.end(); line1++) {

		list<LineSample>::iterator line2 = line1;
		for(line2++; line2 != this->lines.end(); line2++) {

			/*If they are not parallel*/
			if(areVectorsParallel((*line1).alpha, (*line2).alpha, INTERSECTION_PARALLEL_RANGE))
				continue;

			/*Calculate intersection*/
			calcIntersection2D((*line1).lineVector, (*line2).lineVector, intSample.p3D);
			if(intSample.p3D.H == 0.0)
				continue;

			/*Check if the intersection is in the limits of the lines*/
			this->enlargeLine(*line1, INTERSECTION_MARGIN, ps1, pe1);
			if(!isPointInsideLine(intSample.p3D.X, intSample.p3D.Y, ps1.X, ps1.Y, pe1.X, pe1.Y))
				continue;
			this->enlargeLine(*line2, INTERSECTION_MARGIN, ps2, pe2);
			if(!isPointInsideLine(intSample.p3D.X, intSample.p3D.Y, ps2.X, ps2.Y, pe2.X, pe2.Y))
				continue;

			/*Check the type of intersection*/
			isT1 = this->isTIntersection(*line1, intSample.p3D);
			isT2 = this->isTIntersection(*line2, intSample.p3D);

			if(isT1 && isT2)	
				intSample.type = INTERSECTION_TYPE_X;
			else if(isT1 || isT2)
				intSample.type = INTERSECTION_TYPE_T;
			else
				intSample.type = INTERSECTION_TYPE_L;
			
			/*Save intersection*/
			intSample.line1 = &(*line1);		
			intSample.line2 = &(*line2);		

			this->intersections.push_back(intSample);
		}
	}

	/*Intersect the circle and the midLine*/
	if(this->circle.found) {	

		if(this->circle.midLine != NULL) {

			/*Calc intersection between the circle and the vector of the line*/
			calIntersectionCircleVector(this->circle.midLine->lineVector, this->circle.p3D, CIRCLE_RADIUS, int1, int2);

			/*If the intersection is correct, save it*/
			if(int1.H != 0) {
				intSample.p3D = int1;
				intSample.type = INTERSECTION_TYPE_X;
				intSample.line1 = this->circle.midLine;		
				intSample.line2 = this->circle.midLine;	
				this->intersections.push_back(intSample);			
			}
			if(int2.H != 0) {
				intSample.p3D = int2;
				intSample.type = INTERSECTION_TYPE_X;
				intSample.line1 = this->circle.midLine;		
				intSample.line2 = this->circle.midLine;	
				this->intersections.push_back(intSample);			
			}
		}		
	}

	/*Delete closed intersections*/
	for(list<IntersectionSample>::iterator inter1 = this->intersections.begin(); inter1 != this->intersections.end(); inter1++) {

		list<IntersectionSample>::iterator inter2 = inter1;
		inter2++;
		while(inter2 != this->intersections.end()) {
			if(calcDistance2D((*inter1).p3D.X, (*inter1).p3D.Y, (*inter2).p3D.X, (*inter2).p3D.Y) < INTERSECTION_DIST) {
				/*Delete the worst type of intersection*/
				if((*inter2).type > (*inter1).type) {
					(*inter1).line1 = (*inter2).line1;
					(*inter1).line2 = (*inter2).line2;
					(*inter1).p3D = (*inter2).p3D;
					(*inter1).type = (*inter2).type;
				}
				
				inter2 = this->intersections.erase(inter2);
				continue;
			}

			inter2++;
		}
	}
}

int
LineDetector::isTIntersection(LineSample line, HPoint3D p)
{
	float dline, d1, d2;

	dline = calcDistance3D(line.ps3D, line.pe3D);
	d1 = calcDistance3D(line.ps3D, p);
	d2 = calcDistance3D(line.pe3D, p);

	/*Intersection has to be between the line*/
	if(dline < d1 || dline < d2)
		return 0;

	/*Distance to the extremes more than a threshold if it's a T*/
	if(d1 >= INTERSECTION_T_DISTANCE && d2 >= INTERSECTION_T_DISTANCE) {
		return 1;
	}

	return 0;
}

bool
LineDetector::mergeLines(HPoint3D &ps1, HPoint3D &pe1, HPoint3D ps2, HPoint3D pe2, float alpha, float max_dist_x)
{
	float dist1, dist2, dist3, dist4, l1dist, l2dist;
	float cent_x1, cent_y1, cent_x2, cent_y2;
	float disty;
	float max_dist;
	float cosa, sina;

	/*Check parallel distance*/
	cent_x1 = (ps1.X + pe1.X)/2.0;
	cent_y1 = (ps1.Y + pe1.Y)/2.0;
	cent_x2 = (ps2.X + pe2.X)/2.0;
	cent_y2 = (ps2.Y + pe2.Y)/2.0;

	disty = calcDistanceAxis(cent_x1, cent_y1, cent_x2, cent_y2, alpha+(pi_2));
	if(disty > MAX_LINE_DISTANCE_Y)
		return false;

	cosa = cos(alpha);
	sina = sin(alpha);

	/*The extremes have to be together in the X axis*/
	dist1 = calcDistanceAxis(ps1, ps2, cosa, sina);
	max_dist = dist1;	
	dist2 = calcDistanceAxis(ps1, pe2, cosa, sina);
	if(dist2 > max_dist) max_dist = dist2;
	dist3 = calcDistanceAxis(pe1, ps2, cosa, sina);
	if(dist3 > max_dist) max_dist = dist3;
	dist4 = calcDistanceAxis(pe1, pe2, cosa, sina);
	if(dist4 > max_dist) max_dist = dist4;

	if(dist1 > max_dist_x && dist2 > max_dist_x && dist3 > max_dist_x && dist4 > max_dist_x)
		return false;

	l1dist = calcDistanceAxis(ps1, pe1, cosa, sina);
	l2dist = calcDistanceAxis(ps2, pe2, cosa, sina);

	/*Line 2 in the middle of line 1*/
	if(dist1 <= l1dist && dist2 <= l1dist && dist3 <= l1dist && dist4 <= l1dist)
		return true;

	/*Line 1 in the middle of line 2*/
	if(dist1 <= l2dist && dist2 <= l2dist && dist3 <= l2dist && dist4 <= l2dist) {	
		ps1 = ps2;
		pe1 = pe2;
		return true;
	}

	/*Change one extreme*/
	if(max_dist == dist1)
		pe1 = ps2;
	if(max_dist == dist2)
		pe1 = pe2;
	if(max_dist == dist3)
		ps1 = ps2;
	if(max_dist == dist4)
		ps1 = pe2;	

	return true;
}

int
LineDetector::arePerpendicular(HPoint3D p1, HPoint3D p2, HPoint3D q1, HPoint3D q2)
{
	float x1, y1, x2, y2;
	float ang1, ang2, diff;

	x1 = p2.X - p1.X;
	y1 = p2.Y - p1.Y;
	x2 = q2.X - q1.X;
	y2 = q2.Y - q1.Y;

	if(x1==0.0)
		ang1 = pi_2;
	else
		ang1 = atan(y1/x1);

	if(x2==0.0)
		ang2 = pi_2;
	else
		ang2 = atan(y2/x2);

	if(ang1 < 0.0) ang1 += M_PI;
	if(ang2 < 0.0) ang2 += M_PI;

	diff = ang1 - ang2;

	/*Check if it's between 45º and 135º*/
	if(fabs(diff) > M_PI/4.0 && fabs(diff) < 3.0*M_PI/4.0)
		return 1;

	return 0;
}

void
LineDetector::enlargeLine(LineSample line, float size, HPoint3D &ps, HPoint3D &pe)
{
	float dist1, dist2, dist3, dist4;
	float alpha, cosalpha, sinalpha;

	/*Dist before enlarge*/
	dist1 = G_SQUARE(line.pe3D.X - line.ps3D.X);
	dist2 = G_SQUARE(line.pe3D.Y - line.ps3D.Y);

	/*Calc alpha*/
	alpha = calcVectorAngle(line.ps3D.X, line.ps3D.Y, line.pe3D.X, line.pe3D.Y);

	/*Calc new line*/
	cosalpha = size*cos(alpha);
	sinalpha = size*sin(alpha);

	ps.X = line.ps3D.X - cosalpha;
	ps.Y = line.ps3D.Y - sinalpha;
	pe.X = line.pe3D.X + cosalpha;
	pe.Y = line.pe3D.Y + sinalpha;

	/*Check distance*/
	dist3 = G_SQUARE(pe.X - ps.X);
	dist4 = G_SQUARE(pe.Y - ps.Y);	

	/*If distances after < distances before, invert values*/
	if(dist3 < dist1) {
		cosalpha = -cosalpha;
		ps.X = line.ps3D.X - cosalpha;
		pe.X = line.pe3D.X + cosalpha;
	}

	if(dist4 < dist2) {
		sinalpha = -sinalpha;
		ps.Y = line.ps3D.Y - sinalpha;
		pe.Y = line.pe3D.Y + sinalpha;
	}

	ps.Z = 0.0;
	ps.H = 1.0;
	pe.Z = 0.0;
	pe.H = 1.0;
}

void
LineDetector::clearDetected()
{
	if(!this->fAreas.empty())
	{
		this->fAreas.clear();
	}

	if(!this->validLines.empty())
	{
		this->validLines.clear();
	}

	if(!this->lines.empty())
	{
		this->lines.clear();
	}

	if(!this->circleCenters.empty())
	{
		this->circleCenters.clear();
	}
	this->circle.found = false;

	if(!this->intersections.empty())
	{
		this->intersections.clear();
	}
}

void
LineDetector::drawValidRegions(IplImage* src)
{
}

void
LineDetector::drawDetected(IplImage* src)
{
	CvPoint p, q;

	/*Draw forbidden areas*/
	for(list<ForbiddenArea>::iterator area = fAreas.begin(); area != fAreas.end(); area++) {
		/*Left line*/
		p.x  = (*area).ps2DL.x;
		p.y  = (*area).ps2DL.y;
		q.x  = (*area).pe2DL.x;
		q.y  = (*area).pe2DL.y;
		cvLine(src, p, q, CV_RGB(197,75,140), 1, CV_AA, 0);

		/*Right line*/
		p.x  = (*area).ps2DR.x;
		p.y  = (*area).ps2DR.y;
		q.x  = (*area).pe2DR.x;
		q.y  = (*area).pe2DR.y;
		cvLine(src, p, q, CV_RGB(197,75,140), 1, CV_AA, 0);

		/*Bottom line*/
		p.x  = (*area).ps2DL.x;
		p.y  = (*area).ps2DL.y;
		q.x  = (*area).ps2DR.x;
		q.y  = (*area).ps2DR.y;
		cvLine(src, p, q, CV_RGB(197,75,140), 1, CV_AA, 0);

		/*Top line*/
		p.x  = (*area).pe2DL.x;
		p.y  = (*area).pe2DL.y;
		q.x  = (*area).pe2DR.x;
		q.y  = (*area).pe2DR.y;
		cvLine(src, p, q, CV_RGB(197,75,140), 1, CV_AA, 0);
	}

	/*Draw lines*/
	for(list<LineSample>::iterator line = this->lines.begin(); line != this->lines.end(); line++) {
		/*Draw alpha*/
		p.x  = (*line).ps2D.x;
		p.y  = (*line).ps2D.y;
		q.x  = (*line).pe2D.x;
		q.y  = (*line).pe2D.y;
		if((*line).type==1)
			cvLine(src, p, q, CV_RGB(0,0,255), 1, CV_AA, 0);
		else if((*line).type==2)
			cvLine(src, p, q, CV_RGB(0,0,128), 1, CV_AA, 0);
		else if((*line).type==3)
			cvLine(src, p, q, CV_RGB(255,0,0), 1, CV_AA, 0);
		else if((*line).type==4)
			cvLine(src, p, q, CV_RGB(128,0,0), 1, CV_AA, 0);
	}

	/*Draw circle intersections*/
	for(list<CircleSample>::iterator circle1 = this->circleCenters.begin(); circle1 != this->circleCenters.end(); circle1++) {
		/*Draw circle intersections*/
		p.x  = (*circle1).p2D.x;
		p.y  = (*circle1).p2D.y;
		cvCircle(src, p, (int) 2, CV_RGB(255, 0, 0), 1);	
	}
	/*Draw circle*/
	if(this->circle.found) {
		/*Center*/
		p.x  = this->circle.p2D.x;
		p.y  = this->circle.p2D.y;
		cvCircle(src, p, (int) 5, CV_RGB(0, 0, 0), 1);	
		/*External lines*/
		this->drawCircle(src, this->circle.p3D.X, this->circle.p3D.Y, CIRCLE_RADIUS+25.0, 15, 0);		
		this->drawCircle(src, this->circle.p3D.X, this->circle.p3D.Y, CIRCLE_RADIUS-25.0, 15, 0);	
	}

	/*Draw intersections*/
	for(list<IntersectionSample>::iterator inter = this->intersections.begin(); inter != this->intersections.end(); inter++) {
		this->drawCircle(src, (*inter).p3D.X, (*inter).p3D.Y, 100.0, 8, (*inter).type);
	}	
}

void
LineDetector::drawCircle(IplImage* src, float x, float y, float r, int num_segments, int type)
{
	CvPoint p, q;
	HPoint3D pini, pend;
	HPoint2D pini2D, pend2D;
	int col, iend;
	float theta, thetaend;

	for(col=0; col<num_segments; col++) {
		/*Get the current angle*/
		theta = 2.0 * M_PI * ((float)col) / ((float)num_segments);

		/*End point*/
		if(col==0)
			iend = num_segments-1;
		else
			iend = col-1;
		thetaend = 2.0 * M_PI * ((float)iend) / ((float)num_segments);

		pini.X = x + r * cos(theta);
		pini.Y = y + r * sin(theta);	
		pini.Z = 0.0;
		pini.H = 1.0;			

		pend.X = x + r * cos(thetaend);
		pend.Y = y + r * sin(thetaend);	
		pend.Z = 0.0;
		pend.H = 1.0;	

		_Kinematics->get2DPosition(pini, pini2D);
		_Kinematics->get2DPosition(pend, pend2D);
		
		p.x = pini2D.x;
		p.y = pini2D.y;
		q.x = pend2D.x;
		q.y = pend2D.y;

		if(type==0)	//Circle
			cvLine(src, p, q, CV_RGB(0,0,0), 1, CV_AA, 0);
		if(type==INTERSECTION_TYPE_L)
			cvLine(src, p, q, CV_RGB(0,255,0), 1, CV_AA, 0);
		if(type==INTERSECTION_TYPE_T)
			cvLine(src, p, q, CV_RGB(0,0,255), 1, CV_AA, 0);
		if(type==INTERSECTION_TYPE_X)
			cvLine(src, p, q, CV_RGB(255,0,0), 1, CV_AA, 0);		
	}
}


