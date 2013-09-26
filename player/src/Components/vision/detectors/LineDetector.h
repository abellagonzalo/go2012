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

#ifndef LINEDETECTOR_H_
#define LINEDETECTOR_H_

#include "Detector.h"

class ForbiddenArea : public AbstractSample {
public:

	float alphaL, alphaR;
	float dist_start, dist_end;
	HPoint3D ps3DL, ps3DR;
	HPoint3D pe3DL, pe3DR;
	HPoint2D ps2DL, ps2DR;
	HPoint2D pe2DL, pe2DR;
};

class LineSample : public AbstractSample {
public:

	float alpha;
	HPoint2D ps2D, pe2D;
	HPoint3D ps3D, pe3D;
	HPoint3D lineVector;
	bool isMidline;
	bool hasVector;	
	int type;
};

class CircleSample : public AbstractSample {
public:

	LineSample * midLine;
	bool found;	
};

class IntersectionSample : public AbstractSample {
public:

	LineSample * line1;
	LineSample * line2;
	int type;
};

class LineDetector: public Component, public Singleton<LineDetector>, public Detector {
public:
	LineDetector();
	~LineDetector();

	void init(const string newName, AL::ALPtr<AL::ALBroker> parentBroker);
	void step();

	void detect(bool debug);

	list<LineSample> *getLines() {return &(this->lines);};
	CircleSample * getCircle() {return &(this->circle);};
	list<IntersectionSample> * getIntersections() {return &(this->intersections);};

	void drawValidRegions(IplImage* src);
	void drawDetected(IplImage* src);	

	static const int INTERSECTION_TYPE_L = 1;
	static const int INTERSECTION_TYPE_T = 2;
	static const int INTERSECTION_TYPE_X = 3;
	static const int CIRCLE_CENTER = 4;
	static const int LINE_SEGMENT = 5;

	static const float CIRCLE_RADIUS;

private:

	/*Create forbidden area*/
	void createForbiddenArea();
	/*Validate the lines*/
	void validateLines();
	/*Merge line segments to create lines if it's possible*/
	void mergeLineSegments(float parallel_range, int min_candidates, float max_distance, float min_size, int type);
	/*Try to create a circle with the line segments*/
	void searchCircle();
	/*Create lines with single line segments*/
	void searchSingleLineSegments();
	/*Analyze lines detected to put similar lines together*/
	void analyzeLines();
	/*Calculate intersections between the lines*/
	void calculateIntersections();

	/*Merge two lines in a new one if they are aligned*/
	bool mergeLines(HPoint3D &ps1, HPoint3D &pe1, HPoint3D ps2, HPoint3D pe2, float alpha, float max_dist_x);
	/*Return true if the lines are almost perpendicular*/
	int arePerpendicular(HPoint3D p1, HPoint3D p2, HPoint3D q1, HPoint3D q2);
	/*Enlarge a line with the size selected*/
	void enlargeLine(LineSample line, float size, HPoint3D &ps, HPoint3D &pe);
	/*Check if the extremes of the line are near the intersection*/
	int isTIntersection(LineSample line, HPoint3D p);

	/*Clear the samples already created*/
	void clearDetected();

	/*Draw a circle in the image*/
	void drawCircle(IplImage* src, float x, float y, float r, int num_segments, int type);

	static const float FAREA_ANGLE_THRESHOLD;
	static const int FAREA_MIN_SIZE = 1;
	static const float PARALLEL_RANGE_CLEAR;
	static const float PARALLEL_RANGE_UNCLEAR;
	static const float MIN_LINE_SEGMENT_SIZE_CLEAR;
	static const float MIN_LINE_SEGMENT_SIZE_UNCLEAR;
	static const int MIN_NUM_CANDIDATES_CLEAR = 4;
	static const int MIN_NUM_CANDIDATES_UNCLEAR = 2;
	static const float MAX_LINE_DISTANCE_X;
	static const float MAX_LINE_DISTANCE_Y;
	static const float MIN_LINE_RATIO;
	static const float MIN_SIZE_SINGLE_LINESEGMENT;
	static const float MAX_CIRCLE_DISTANCE_SEGMENTS;
	static const float CIRCLE_RADIUS_MAX;
	static const float CIRCLE_RADIUS_MIN;
	static const float CIRCLE_RADIUS_DISTANCE;
	static const int MIN_NUM_CANDIDATES_CIRCLE = 5;
	static const float ANALYZE_MIN_SIZE;
	static const float ANALYZE_PARALLEL;
	static const float ANALYZE_PARALLEL_SEGMENTS;
	static const float ANALYZE_DIST_X;
	static const float ANALYZE_DIST_Y;
	static const float ANALYZE_DIST_SEGMENTS;
	static const float MAX_MIDLINE_DISTANCE;
	static const float INTERSECTION_MARGIN;
	static const float INTERSECTION_T_DISTANCE;
	static const float INTERSECTION_PARALLEL_RANGE;
	static const float INTERSECTION_DIST;

	list<ForbiddenArea> fAreas;

	list<LineRegion> validLines;
	list<LineSample> lines;

	list<CircleSample> circleCenters;
	CircleSample circle;

	list<IntersectionSample> intersections;
};

#endif /* LINEDETECTOR_H_ */
