/*
 * Name: AbstractRegion.cpp
 * @Author: Eduardo Perdices (eperdices@gsyc.es)
 * 
 * Description: Abstract class used by regions
 *
 * Created on: 23/06/2010
 *
 * Copyright (C) Universidad Rey Juan Carlos
 * All Rights Reserved.
 *
 */

#include "AbstractRegion.h"

AbstractRegion::AbstractRegion() {

	_Segmentation = Segmentation::getInstance();
	_RegionGeometry = RegionGeometry::getInstance();
}

AbstractRegion::~AbstractRegion() {
}

