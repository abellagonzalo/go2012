/*
 * Name: ImageHandler.h
 * @Author: Eduardo Perdices (eperdices@gsyc.es)
 *  
 * Description: Class that handles the image.
 * 
 * Copyright (C) 2009-2010 Universidad Rey Juan Carlos
 * All Rights Reserved. 
 */

#ifndef _IMAGEHANDLER_H_
#define _IMAGEHANDLER_H_

#include "Singleton.h"
#include "ImageInput.h"
#include "vision/colorFilter/simpleColorFilter/SimpleColorFilter.h"

using namespace std;

class ImageHandler: public Singleton<ImageHandler>
{
public:
	ImageHandler();
	~ImageHandler();

	void setImage(int camera, char * data, SimpleColorFilter * colorFilter);
	void getImageRGB(char * img, bool filter, int color);
	void getImageRGBInverted(char * img, bool filter, int color);
	void getImageHSV(char * img);
	int getColor(int col, int row);

	/*Calc the length of a segment with the color selected from the point (x,y) upward*/
	int getSegmentSizeUp(int x, int y, int maxy, int color);
	/*Calc the length of a segment with the color selected from the point (x,y) downward*/
	int getSegmentSizeDown(int x, int y, int maxy, int color);
	/*Calc the length of a segment with the color selected from the point (x,y) rightward*/
	int getSegmentSizeRight(int x, int y, int maxx, int color);

	/*Check the previous columns of a segment to confirm if it touchs the previus segment*/
	void checkPreviousCols(int x, int &ys, int &ye, int minx, int miny, int maxy, int step, int color);

	/*Obtain the extreme of a post in the image*/
	void scanPostExtremeInImage(int col, int row, int &outcol, int &outrow, int color, bool goDown);

	/*Obtain the extreme of a ball in the image with the step selected. Return true if it reachs the end of the image*/
	bool scanBallExtremeInImage(int col, int row, int &size, int stepx, int stepy, int color);

	/*Show statistics about pixels used*/
	void showStatistics();

private:
	static const int MAX_WRONG_PIXELS = 2;
	static const int MAX_NOT_BALL_PIXELS = 4;

	int camera;
	SimpleColorFilter * colorFilter;

	char image[ImageInput::IMG_WIDTH][ImageInput::IMG_HEIGHT][ImageInput::IMG_CHANNELS_YUV];
	int colors[ImageInput::IMG_WIDTH][ImageInput::IMG_HEIGHT];
	bool filtered[ImageInput::IMG_WIDTH][ImageInput::IMG_HEIGHT];	
};

#endif //_IMAGEHANDLER_H_
