/*
 * Name: Detector.h
 * @Author: Eduardo Perdices (eperdices@gsyc.es)
 * 
 * Description: Abstract class used by detectors
 *
 * Created on: 31/05/2010
 *
 * Copyright (C) Universidad Rey Juan Carlos
 * All Rights Reserved.
 *
 */

#ifndef DETECTOR_H_
#define DETECTOR_H_

#include <pthread.h>
#include "Component.h"
#include "Singleton.h"
#include "Kinematics.h"
#include "ImageHandler.h"
#include "RegionBuilder.h"
#include "Geometry.h"

class AbstractSample {
public:

	int n, timestamp, size;

	HPoint2D p2D;
	HPoint3D p3D;
};

class Detector {
public:
	Detector();
	~Detector();

	virtual void detect(bool debug) = 0;


	virtual void drawValidRegions(IplImage* src) = 0;
	virtual void drawDetected(IplImage* src) = 0;	

	bool seen();

protected:

	inline long getTime() {
		gettimeofday(&currentTime, NULL);
		return currentTime.tv_sec * 1000000 + currentTime.tv_usec;
	}


	int color;

	float time2forget;
	long lastSeen;
	struct timeval currentTime;

	Kinematics * _Kinematics;
	ImageHandler * _ImageHandler;
	RegionBuilder * _RegionBuilder;
};

#endif /* DETECTOR_H_ */
