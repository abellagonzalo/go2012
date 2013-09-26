/*
 * Name: SimpleColorFilter.cpp
 * @Author: Carlos Agüero (caguero@gsyc.es)
 *
 * Description: Specific implementation of the generic class 'ColorFiltfer'. The algorith implemented is a
 * simple HSV threshold filter.
 *
 * Copyright (C) 2008-2009 Universidad Rey Juan Carlos
 * All Rights Reserved.
 */

#include "SimpleColorFilter.h"

// This values are in BGR, not RGB.
const unsigned char SimpleColorFilter::sliceOrange[3] 	=	{29., 198., 255.};
const unsigned char SimpleColorFilter::sliceBlue[3]		= 	{255., 205., 73.};
const unsigned char SimpleColorFilter::sliceYellow[3] 	= 	{94., 218., 212.};
const unsigned char SimpleColorFilter::sliceGreen[3]  	= 	{0., 110., 0.};
const unsigned char SimpleColorFilter::sliceWhite[3]  	= 	{255., 255., 255.};
const unsigned char SimpleColorFilter::sliceCyan[3]		=	{255., 255., 0.};
const unsigned char SimpleColorFilter::sliceMagenta[3]	=	{255., 0., 255.};

/**
 * Class constructor that initializes the specific color filter with a set of HSV thresholds and
 * with the parameters of the images to filter.
 *
 * @param hmin Sets the lower threshold of the H filter's component.
 * @param hmax Sets the upper threshold of the H filter's component.
 * @param smin Sets the lower threshold of the S filter's component.
 * @param smax Sets the upper threshold of the S filter's component.
 * @param vmin Sets the lower threshold of the V filter's component.
 * @param vmax Sets the upper threshold of the V filter's component.
 * @param width Sets the width of the image to filter.
 * @param height Sets the height of the image to filter.
 * @param channels Sets the number of the channels of the image to filter.
 */
SimpleColorFilter::SimpleColorFilter() : ColorFilter ()
{
}


/**
 * Class destructor.
 **/
SimpleColorFilter::~SimpleColorFilter()
{
}

/**
 * Obtain the color of the pixel selected
 * @param image Source image to filter.
 **/
int
SimpleColorFilter::getColor(const int cam, char *image, int col, int row)
{
	unsigned char filt;
	int pixel = row * width + col;
	int posSrc = pixel * 2;	

	if (pixel % 2 == 0) {
		if (cam == ImageInput::UPPER_CAMERA)
			filt = lutYUVupper->getValue(image[posSrc], image[posSrc + 1], image[posSrc + 3]);
		else
			filt = lutYUVlower->getValue(image[posSrc], image[posSrc + 1], image[posSrc + 3]);
	}else{
		if (cam == ImageInput::UPPER_CAMERA)
			filt = lutYUVupper->getValue(image[posSrc], image[posSrc - 1], image[posSrc + 1]);
		else
			filt = lutYUVlower->getValue(image[posSrc], image[posSrc - 1], image[posSrc + 1]);
	}

	if (filt & GREEN_MASK) 				//Green
		return ImageInput::CGREEN;

	if (filt & ORANGE_MASK) 			//Orange
		return ImageInput::CORANGE;

	if (filt & BLUE_MASK)				//Blue
		return ImageInput::CBLUE;

	if (filt & YELLOW_MASK) 			//Yellow
		return ImageInput::CYELLOW;

	if (filt & WHITE_MASK)				//White
		return ImageInput::CWHITE;

	if (filt & CYAN_MASK)				//Cyan
		return ImageInput::CCYAN;

	if (filt & MAGENTA_MASK)			//Magenta
		return ImageInput::CMAGENTA;	


	return ImageInput::CUNKNOWN;
}

/**
 * Obtain the color in RGB of the pixel selected
 * @param image Source.
 **/
void
SimpleColorFilter::getColorRGB(const int cam, char *image, int col, int row, char &r, char &g, char &b, bool filter, int color)
{
	unsigned char rt=0, gt=0, bt=0, intensity;	
	int pixcolor;
	int pixel, posSrc;

	if(!filter) {
		pixel = row * width + col;
		posSrc = pixel * 2;

		/*Get real pixel color*/
		if (pixel % 2 == 0)
			ColorModelConversions::yuv2rgb(image[posSrc], image[posSrc + 1], image[posSrc + 3], rt, gt, bt);
		else
			ColorModelConversions::yuv2rgb(image[posSrc], image[posSrc - 1], image[posSrc + 1], rt, gt, bt);
	} else {
		/*Filter pixel*/
		pixcolor = this->getColor(cam, image, col, row);
		switch(pixcolor) {
		case ImageInput::CGREEN:
			rt = sliceGreen[2];
			gt = sliceGreen[1];
			bt = sliceGreen[0];
			break;
		case ImageInput::CORANGE:
			rt = sliceOrange[2];
			gt = sliceOrange[1];
			bt = sliceOrange[0];
			break;
		case ImageInput::CBLUE:
			rt = sliceBlue[2];
			gt = sliceBlue[1];
			bt = sliceBlue[0];
			break;
		case ImageInput::CYELLOW:
			rt = sliceYellow[2];
			gt = sliceYellow[1];
			bt = sliceYellow[0];
			break;
		case ImageInput::CWHITE:
			rt = sliceWhite[2];
			gt = sliceWhite[1];
			bt = sliceWhite[0];
			break;
		case ImageInput::CCYAN:
			rt = sliceCyan[2];
			gt = sliceCyan[1];
			bt = sliceCyan[0];
			break;
		case ImageInput::CMAGENTA:
			rt = sliceMagenta[2];
			gt = sliceMagenta[1];
			bt = sliceMagenta[0];
			break;
		default:

			pixel = row * width + col;
			posSrc = pixel * 2;

			/*Get pixel in gray scale*/
			if (pixel % 2 == 0)
				ColorModelConversions::yuv2rgb(image[posSrc], image[posSrc + 1], image[posSrc + 3], rt, gt, bt);
			else
				ColorModelConversions::yuv2rgb(image[posSrc], image[posSrc - 1], image[posSrc + 1], rt, gt, bt);

			intensity = 0.11 * bt + 0.59 * gt + 0.30 * rt;
			rt = intensity;
			gt = intensity;
			bt = intensity;
			break;			
		} 

		/*If a color is selected, we only filter that color*/
		if(color > 0) {
			if(color != pixcolor && pixcolor != ImageInput::CUNKNOWN) {
				pixel = row * width + col;
				posSrc = pixel * 2;

				/*Get pixel in gray scale*/
				if (pixel % 2 == 0)
					ColorModelConversions::yuv2rgb(image[posSrc], image[posSrc + 1], image[posSrc + 3], rt, gt, bt);
				else
					ColorModelConversions::yuv2rgb(image[posSrc], image[posSrc - 1], image[posSrc + 1], rt, gt, bt);

				intensity = 0.11 * bt + 0.59 * gt + 0.30 * rt;
				rt = intensity;
				gt = intensity;
				bt = intensity;
			}
		}
	}

	r = (char) bt;
	g = (char) gt;
	b = (char) rt;
}

/**
 * Obtain the color in RGB of the pixel selected
 * @param image Source.
 **/
void
SimpleColorFilter::getColorHSV(const int cam, char *image, int col, int row, char &h, char &s, char &v)
{
	unsigned char rt=0, gt=0, bt=0;	
	float ht=0, st=0, vt=0;	
	int pixel, posSrc;

	pixel = row * width + col;
	posSrc = pixel * 2;

	/*Get real pixel color*/
	if (pixel % 2 == 0)
		ColorModelConversions::yuv2rgb(image[posSrc], image[posSrc + 1], image[posSrc + 3], rt, gt, bt);
	else
		ColorModelConversions::yuv2rgb(image[posSrc], image[posSrc - 1], image[posSrc + 1], rt, gt, bt);

	/*Convert to HSV*/
	ColorModelConversions::rgb2hsv(rt, gt, bt, ht, st, vt);

	ht = 255.0 * ht / 360.0;
	st = st * 100.0;
	vt = vt;

	h = (char) (unsigned char) ht;
	s = (char) (unsigned char) st;
	v = (char) (unsigned char) vt;
}
