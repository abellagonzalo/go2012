/*
 * Name: ColorFilter.h
 * @Author: Carlos Agüero (caguero@gsyc.es)
 *
 * Description: Abstract class that specify a generic interface for any HSV color filter. The class needs
 *  the H, S and V thresholds and also the parameters of the image to filter. The method
 * getFiltered() returns a pointer to the last filtered image. It could be useful for debugging purpose.
 *
 * Copyright (C) Universidad Rey Juan Carlos
 * All Rights Reserved.
 */

#ifndef _COLORFILTER_H_
#define _COLORFILTER_H_

#include <string>
#include <iostream>
#include "../VisionDefinitions.h"
#include "../ImageInput.h"
#include "../../Dictionary.h"
#include "../Lut.h"

using namespace std;

class ColorFilter : public ImageInput
{
public:

	ColorFilter();
	~ColorFilter();

	void getFilterParams(const int cam, HSVClrParams& orange, HSVClrParams& blue, HSVClrParams& yellow,
			HSVClrParams& green, HSVClrParams& white, HSVClrParams& cyan, HSVClrParams& magenta);

	void setFilterParams(const int cam, const HSVClrParams orange, const HSVClrParams blue, const HSVClrParams yellow,
			const HSVClrParams green, const HSVClrParams white, const HSVClrParams cyan, const HSVClrParams magenta);

	void loadFilterParams(const int cam, const char *file);

	void saveFilterParams(const char *file);

	void reloadLut(const int cam, const HSVClrParams orange, const HSVClrParams blue, const HSVClrParams yellow,
			const HSVClrParams green, const HSVClrParams white, const HSVClrParams cyan, const HSVClrParams magenta);

	virtual int getColor(const int cam, char *image, int i, int j) = 0;

protected:

	static const string COLOR_FILTER_PARAMS_FILE_LOWER;
	static const string COLOR_FILTER_PARAMS_FILE_UPPER;

	Lut *lutYUVupper, *lutYUVlower;
	HSVClrParams orangeUpper, blueUpper, yellowUpper, greenUpper, whiteUpper, cyanUpper, magentaUpper;
	HSVClrParams orangeLower, blueLower, yellowLower, greenLower, whiteLower, cyanLower, magentaLower;
	Dictionary colorConfigLower, colorConfigUpper;
	string colorFileNameLower, colorFileNameUpper;
};

#endif //_COLORFILTER_H_
