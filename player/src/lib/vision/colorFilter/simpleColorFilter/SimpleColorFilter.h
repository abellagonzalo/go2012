/*
 * Name: SimpleColorFilter.h
 * @Author: Carlos Agüero (caguero@gsyc.es)
 *
 * Description: Specific implementation of the generic class 'ColorFilter'. The algorith implemented is a
 * simple HSV threshold filter.
 *
 * Copyright (C) 2008-2009 Universidad Rey Juan Carlos
 * All Rights Reserved.
 */

#ifndef _SIMPLECOLORFILTER_H_
#define _SIMPLECOLORFILTER_H_

#include "../ColorFilter.h"
#include "../../VisionDefinitions.h"
#include "../ColorModelConversions.h"

class SimpleColorFilter : public ColorFilter, public Singleton<SimpleColorFilter>
{
public:
	SimpleColorFilter();
	virtual ~SimpleColorFilter();

	int getColor(const int cam, char *image, int col, int row);
	void getColorRGB(const int cam, char *image, int col, int row, char &r, char &g, char &b, bool filter, int color);
	void getColorHSV(const int cam, char *image, int col, int row, char &h, char &s, char &v);

private:
	static const unsigned char sliceOrange[3], sliceBlue[3], sliceYellow[3], sliceGreen[3],	
							   sliceWhite[3], sliceCyan[3], sliceMagenta[3];
};

#endif //_SIMPLECOLORFILTER_H_
