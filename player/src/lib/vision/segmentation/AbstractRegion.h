/*
 * Name: AbstractRegion.h
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

#ifndef ABSTRACTREGION_H_
#define ABSTRACTREGION_H_

#include "ImageInput.h"
#include "Segment.h"
#include "Segmentation.h"
#include "Region.h"
#include "RegionGeometry.h"

using namespace std;

class AbstractRegion {
public:
	AbstractRegion();
	~AbstractRegion();

	/*Check if the region is valid*/
	virtual bool validate(Region * region) = 0;

	/*Draw the region in the image*/
	virtual void drawRegion(IplImage* src) = 0;

protected:

	RegionGeometry * _RegionGeometry;
	Segmentation * _Segmentation;
};

#endif /* ABSTRACTREGION_H_ */
