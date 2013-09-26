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

#include "Detector.h"

Detector::Detector() {

	_Kinematics = Kinematics::getInstance();
	_ImageHandler = ImageHandler::getInstance();
	_RegionBuilder = RegionBuilder::getInstance();

	this->time2forget = 5.0;
	this->lastSeen = 0;
}

Detector::~Detector() {
}

bool
Detector::seen() {

	long ctime = getTime();
	long tforget = (long)(this->time2forget*1000000.0);

	return (ctime - lastSeen) <= tforget;
}

