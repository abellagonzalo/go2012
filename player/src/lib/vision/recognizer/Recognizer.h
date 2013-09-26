/*
 * Name: Recognizer.h
 * @Author: Carlos Agüero (caguero@gsyc.es)
 *
 * Description:
 *
 * Copyright (C) 2008-2009 Universidad Rey Juan Carlos
 * All Rights Reserved.
 */

#ifndef _RECOGNIZER_H_
#define _RECOGNIZER_H_

#include <algorithm>
#include "ImageInput.h"
#include "Horizon.h"
#include "Head.h"
#include "Leds.h"

using namespace std;

#define RECOGNIZE_CONFIG_FILE "/home/nao/bica/conf/ballPerception/recognize.conf"
#define HORIZON_FILTER_PARAMS_FILE "/home/nao/bica/conf/ballPerception/colorHorizonHSV.conf"

typedef struct
{
	int n, timestamp, size;
	float pitch, yaw, x, y, maxx, maxy, minx, miny, relativeX, relativeY;
}tobject;

class Recognizer : public ImageInput
{
public:
	Recognizer(Horizon *horizon);
	~Recognizer();

	inline void reset() 		{ obj.n = 0; };
	inline tobject getObject() 	{ return obj; };
	inline tobject getObject2()	{ return obj2; };
	inline char* getRecognized(){return cvRecognized->imageData; };

	virtual void recognize(int* blobs, bool debug = false) = 0;

protected:

	void loadConf();
	void init(bool debug);
	bool preRecognize(int *blob);
	void postRecognize(bool debug);

	Horizon 	*horizon;
	IplImage	*cvRecognized;
	tobject 	obj, obj2;
	Dictionary 	conf;

	// Restrictions
	unsigned int BLOB_MIN_PIXELS, BALL_MIN_PIXELS_UPPER, BALL_MAX_PIXELS_UPPER,
	BALL_MIN_SIZE_LOWER, NET_MIN_PIXELS, NET_MIN_SIZE, BALL_MAX_DIF_EXPECTED_SIZE;
	float BALL_ASPECT_RATIO, BALL_MIN_DENSITY;

	int objectn, objectn2, radius, best_radius, bestX, bestY, lower_blob, NET_MIN_DIST_BASE_TO_HORIZON;
	float objectx, objecty, ratio, density, segmX, segmY, size, objectmaxx, objectmaxy,
	objectminx, objectminy, objectx2, objecty2,	object2maxx, object2maxy, object2minx,
	object2miny, closerDist2previous, previousX, previousY, relX, relY, bestRelX, bestRelY;
	bool seen, onEdge;
	Point2D center, base;

	Leds 		*leds;
	Head 		*head;
};

#endif //_RECOGNIZER_H_
