/*
 * Name: ImageHandler.h
 * @Author: Eduardo Perdices (eperdices@gsyc.es)
 *  
 * Description: Class that handles the image.
 * 
 * Copyright (C) 2009-2010 Universidad Rey Juan Carlos
 * All Rights Reserved. 
 */

#include "ImageHandler.h"

ImageHandler::ImageHandler()
{
}


ImageHandler::~ImageHandler()
{
}

void
ImageHandler::setImage(int camera, char * data, SimpleColorFilter * colorFilter)
{
	this->camera = camera;
	this->colorFilter = colorFilter;
	memcpy(this->image, data, ImageInput::IMG_WIDTH * ImageInput::IMG_HEIGHT * ImageInput::IMG_CHANNELS_YUV);
	memset(this->filtered, 0, sizeof(bool)*ImageInput::IMG_WIDTH*ImageInput::IMG_HEIGHT);
}

void
ImageHandler::getImageRGB(char * img, bool filter, int color)
{
	char r, g, b;
	int row, col;
	int pixel;

	for(col = 0;col < ImageInput::IMG_WIDTH; col++) {
		for(row = 0;row < ImageInput::IMG_HEIGHT; row++) {		
			/*Get the RGB value*/
			this->colorFilter->getColorRGB(this->camera, (char *)this->image, col, row, r, g, b, filter, color);
			
			pixel = (row * ImageInput::IMG_WIDTH + col)*3;

			img[pixel] = r;
			img[pixel+1] = g;
			img[pixel+2] = b;
		}		
	}
}

void
ImageHandler::getImageRGBInverted(char * img, bool filter, int color)
{
	char r, g, b;
	int row, col;
	int pixel;

	for(col = 0;col < ImageInput::IMG_WIDTH; col++) {
		for(row = 0;row < ImageInput::IMG_HEIGHT; row++) {		
			/*Get the RGB value*/
			this->colorFilter->getColorRGB(this->camera, (char *)this->image, col, row, r, g, b, filter, color);
			
			pixel = (row * ImageInput::IMG_WIDTH + col)*3;

			img[pixel] = b;
			img[pixel+1] = g;
			img[pixel+2] = r;
		}		
	}
}

void
ImageHandler::getImageHSV(char * img)
{
	char h, s, v;
	int row, col;
	int pixel;

	for(col = 0;col < ImageInput::IMG_WIDTH; col++) {
		for(row = 0;row < ImageInput::IMG_HEIGHT; row++) {		
			/*Get the HSV value*/
			this->colorFilter->getColorHSV(this->camera, (char *)this->image, col, row, h, s, v);
			
			pixel = (row * ImageInput::IMG_WIDTH + col)*3;

			img[pixel] = h;
			img[pixel+1] = s;
			img[pixel+2] = v;
		}		
	}
}

int
ImageHandler::getColor(int col, int row)
{
	/*Check values*/
	if(col < 0 || col >= ImageInput::IMG_WIDTH) {
		//cerr << "Error: Column out of range, " << col << endl;
		return 0;
	}
	if(row < 0 || row >= ImageInput::IMG_HEIGHT) {
		//cerr << "Error: Row out of range, " << row << endl;
		return 0;
	}

	/*Check if the color has been already calculated*/
	if(this->filtered[col][row])
		return this->colors[col][row];

	/*Filter the color*/
	this->colors[col][row] = this->colorFilter->getColor(this->camera, (char *)this->image, col, row);
	this->filtered[col][row] = true;

	return this->colors[col][row];
}

int
ImageHandler::getSegmentSizeUp(int x, int y, int miny, int color)
{
	int row;
	int numwrong = 0;
	int finaly;
	bool wrongfound = false;
	int step = ImageInput::IMG_MINISTEP;

	if(y<=miny - 1)
		return 1;

	row = y - step;
	while(row>=miny) { 
		/*Check if the color is the same*/
		if(this->getColor(x, row) == color) {
			numwrong = 0;
			row-=step;
			continue;
		}
	
		/*If there is only one pixel left, exit*/
		if(numwrong == MAX_WRONG_PIXELS-1) {
			wrongfound = true;
			break;
		}

		/*Check the previous pixel*/
		if(this->getColor(x, row+1) != color)
			numwrong+=step;
		else
			numwrong++;

		/*If we found the max wrong, exit*/
		if(numwrong == MAX_WRONG_PIXELS) {
			wrongfound = true;
			break;
		}

		if(numwrong == MAX_WRONG_PIXELS-1)
			row--;
		else
			row-=step;
	}
	finaly = row;

	if(wrongfound)
		finaly = row+MAX_WRONG_PIXELS;

	if(row<miny)
		finaly = miny;

	return y - finaly + 1;
}

int
ImageHandler::getSegmentSizeDown(int x, int y, int maxy, int color)
{
	int row;
	int numwrong = 0;
	int finaly;
	bool wrongfound = false;
	int step = ImageInput::IMG_MINISTEP;

	if(y>=maxy - 1)
		return 1;

	row = y + step;
	while(row<=maxy) { 
		/*Check if the color is the same*/
		if(this->getColor(x, row) == color) {
			numwrong = 0;
			row+=step;
			continue;
		}
	
		/*If there is only one pixel left, exit*/
		if(numwrong == MAX_WRONG_PIXELS-1) {
			wrongfound = true;
			break;
		}

		/*Check the previous pixel*/
		if(this->getColor(x, row-1) != color)
			numwrong+=step;
		else
			numwrong++;

		/*If we found the max wrong, exit*/
		if(numwrong == MAX_WRONG_PIXELS) {
			wrongfound = true;
			break;
		}

		if(numwrong == MAX_WRONG_PIXELS-1)
			row++;
		else
			row+=step;
	}
	finaly = row;

	if(wrongfound)
		finaly = row-MAX_WRONG_PIXELS;

	if(row>maxy)
		finaly = maxy;

	return finaly - y + 1;
}

int
ImageHandler::getSegmentSizeRight(int x, int y, int maxx, int color)
{
	int col;
	int numwrong = 0;
	int finalx;
	bool wrongfound = false;
	int step = ImageInput::IMG_MINISTEP;

	if(x>=maxx - 1)
		return 1;

	col = x + step;
	while(col<=maxx) { 
		/*Check if the color is the same*/
		if(this->getColor(col, y) == color) {
			numwrong = 0;
			col+=step;
			continue;
		}
	
		/*If there is only one pixel left, exit*/
		if(numwrong == MAX_WRONG_PIXELS-1) {
			wrongfound = true;
			break;
		}

		/*Check the previous pixel*/
		if(this->getColor(col-1, y) != color)
			numwrong+=step;
		else
			numwrong++;

		/*If we found the max wrong, exit*/
		if(numwrong == MAX_WRONG_PIXELS) {
			wrongfound = true;
			break;
		}

		if(numwrong == MAX_WRONG_PIXELS-1)
			col++;
		else
			col+=step;
	}
	finalx = col;

	if(wrongfound)
		finalx = col-MAX_WRONG_PIXELS;

	if(col>maxx)
		finalx = maxx;

	return finalx - x + 1;
}

void
ImageHandler::checkPreviousCols(int x, int &ys, int &ye, int minx, int miny, int maxy, int step, int color)
{
	int col, size;

	/*Cover the columns before the current one*/
	for(col=x-step; col>minx; col-=step) {
		/*Save the min row with this color in the columns covered*/
		if(ys > miny) {
			if(this->getColor(col, ys) == color) {
		    	size = this->getSegmentSizeUp(col, ys, miny, color);
				if(size > 1)
				  ys = ys - (size - 1);
			}
		}

		/*Save the max row with this color in the columns covered*/
		if(ye < maxy) {
			if(this->getColor(col, ye) == color) {
				size = this->getSegmentSizeDown(col, ye, maxy, color);
				if(size > 1)
				  ye = ye + (size - 1);
			}
		}
	}
}

void
ImageHandler::scanPostExtremeInImage(int col, int row, int &outcol, int &outrow, int color, bool goDown)
{
	int margin, minx, maxx, miny, maxy;
	int change, currenty, currentx;
	int changeRight, changeLeft;
	int evenNumber=0;

	margin = 1;
	minx = margin;
	maxx = ImageInput::IMG_WIDTH - 1 - margin;
	miny = margin;
	maxy = ImageInput::IMG_HEIGHT - 1 - margin;

	currentx = col;
	currenty = row;
	if(goDown)
		change = 1;		
	else
		change = -1;

	/*Scan image to get the real extremes*/
	while(currenty + change <= maxy && currenty + change >= miny) {
		if(this->getColor(currentx, currenty + change) == color) {
			currenty += change;
			continue;
		}
		/*Check closed pixels to avoid noise*/
		evenNumber++;
		if(evenNumber%2 == 0) {
			/*First the right*/
			if(currentx + 1 <= maxx) {
				if(this->getColor(currentx + 1, currenty + change) == color) {
					currenty += change;
					currentx += 1;
					continue;
				}
			}
			if(currentx - 1 >= minx) {
				if(this->getColor(currentx - 1, currenty + change) == color) {
					currenty += change;
					currentx -= 1;
					continue;
				}
			}
			if(currentx + 2 <= maxx) {
				if(this->getColor(currentx + 2, currenty + change) == color) {
					currenty += change;
					currentx += 2;
					continue;
				}
			}
			if(currentx - 2 >= minx) {
				if(this->getColor(currentx - 2, currenty + change) == color) {
					currenty += change;
					currentx -= 2;
					continue;
				}
			}
		} else {
			/*First the left*/
			if(currentx - 1 >= minx) {
				if(this->getColor(currentx - 1, currenty + change) == color) {
					currenty += change;
					currentx -= 1;
					continue;
				}
			}
			if(currentx + 1 <= maxx) {
				if(this->getColor(currentx + 1, currenty + change) == color) {
					currenty += change;
					currentx += 1;
					continue;
				}
			}
			if(currentx - 2 >= minx) {
				if(this->getColor(currentx - 2, currenty + change) == color) {
					currenty += change;
					currentx -= 2;
					continue;
				}
			}
			if(currentx + 2 <= maxx) {
				if(this->getColor(currentx + 2, currenty + change) == color) {
					currenty += change;
					currentx += 2;
					continue;
				}
			}
		}
		break;
	}

	/*Get the central column of the bottom extreme. We can't do it with the top extreme because of the crossbar*/
	if(goDown) {
		changeRight = changeLeft = 0;

		while(currentx + changeRight < maxx) {
			if(this->getColor(currentx + changeRight + 1, currenty) == color)
				changeRight++;
			else
				break;
		}

		while(currentx + changeLeft > minx) {
			if(this->getColor(currentx + changeLeft - 1, currenty) == color)
				changeLeft--;
			else
				break;
		}

		currentx = currentx + (changeRight+changeLeft)/2;
	}

	/*Save the extreme*/
	outcol = currentx;
	outrow = currenty;
}

bool
ImageHandler::scanBallExtremeInImage(int col, int row, int &size, int stepx, int stepy, int color)
{
	int margin, minx, maxx, miny, maxy;
	int currentx, currenty;
	bool exit = false;
	bool border = false;
	int numNotDetected = 0;
	int length = 0;

	margin = 1;
	minx = margin;
	maxx = ImageInput::IMG_WIDTH - 1 - margin;
	miny = margin;
	maxy = ImageInput::IMG_HEIGHT - 1 - margin;

	currentx = col;
	currenty = row;

	while(!exit) {
		
		currentx += stepx;
		currenty += stepy;

		/*Check borders*/
		if(currentx < minx || currentx > maxx || currenty < miny || currenty > maxy) {

			/*If the last pixel was ball, save length*/
			if(numNotDetected==0) {
				size = length;
				border = true;
			} else {
				border = false;			
			}
	
			exit = true;
			continue;
		} 

		/*Get the color of the pixel*/
		if(this->getColor(currentx, currenty) == color)
			numNotDetected=0;
		else {
			/*If it's the first pixel different to the ball, save length*/
			if(numNotDetected==0)
				size = length;

			numNotDetected++;

			/*Exit, if there are too many not detected*/
			if(numNotDetected > MAX_NOT_BALL_PIXELS) {
				exit = true;
			}
		}	

		length++;
	}

	return border;
}

void
ImageHandler::showStatistics()
{
	int col, row;
	int cont=0;

	for(col=0;col<ImageInput::IMG_WIDTH;col++)
		for(row=0;row<ImageInput::IMG_HEIGHT;row++)
			if(this->filtered[col][row])
				cont++;

	cout << "Se ha utilizado un " << (float)cont/(float)(ImageInput::IMG_WIDTH*ImageInput::IMG_HEIGHT) << " por ciento de la imagen" << endl;
}
