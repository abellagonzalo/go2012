/*
 * Name: Segmentation.cpp
 * @Author: Eduardo Perdices (eperdices@gsyc.es)
 *
 * Description: Class to segment the image into Segments.
 *
 * Copyright (C) 2009-2010 Universidad Rey Juan Carlos
 * All Rights Reserved.
 */

#include "Segmentation.h"


Segmentation::Segmentation()
{
	_ImageHandler = ImageHandler::getInstance();
	_BodyDetector = BodyDetector::getInstance();
}

Segmentation::~Segmentation()
{
}

list<Segment> * 
Segmentation::getSegments(int col)
{
	return &(this->segments[col]);
}

list<Segment> *
Segmentation::getGoalVSegments(int col)
{
	return &(this->goalVSegments[col]);
}

list<Segment> *
Segmentation::getGoalHSegments(int row)
{
	return &(this->goalHSegments[row]);
}

void
Segmentation::analyzeImage(Horizon * horizon)
{
	int i;

	/*Reset Segments*/
	for(i=0;i<ImageInput::IMG_STEPS_V;i++) {
		this->segments[i].clear();
		this->goalVSegments[i].clear();		
	}
	for(i=0;i<ImageInput::IMG_STEPS_H;i++) {
		this->goalHSegments[i].clear();		
	}	

	/*Scan vertically to find segments (goals, lines and ball)*/
	this->calcVerticalSegments(horizon);

	/*Scan horizontally to find goal segments*/
	this->calcHorizontalSegments(horizon);
}

void
Segmentation::calcVerticalSegments(Horizon * horizon)
{
	int col, row;
	int marginx = ImageInput::IMG_MARGIN_STEP;
	int marginy = 1;
	int counter, ccolor, size;
	int rowini, rowend, maxrowend;
	int step = ImageInput::IMG_MINISTEP;
	bool checkPrevius = false;

	maxrowend = ImageInput::IMG_HEIGHT-1-marginy;

	counter = 0;
	for(col=marginx;col<ImageInput::IMG_WIDTH;col=col+ImageInput::IMG_STEP) {

		/*Get the field border in this column*/
		rowini = horizon->getFieldBorderCol(counter);
		if(rowini<marginy) 
			rowini = marginy;

		/*Row ends where robot's body starts or at the end of the image*/
		rowend = _BodyDetector->getMaxRow(counter);
		if(rowend > maxrowend)
			rowend = maxrowend;

		checkPrevius = false;
		row = marginy;

		/*Scan segments over the field border searching for goals*/
		while(row < rowini && row < rowend) {

			ccolor = _ImageHandler->getColor(col, row);
			if(ccolor == ImageInput::CYELLOW || ccolor == ImageInput::CBLUE) {

				/*If we skipped a pixel, check it*/
				if(checkPrevius) {
					if(ccolor == _ImageHandler->getColor(col, row-1))
						row = row-1;
				}

				/*Get the size of the segment*/
				size = _ImageHandler->getSegmentSizeDown(col, row, rowend, ccolor);
				if(size >= MIN_SEGMENT_SIZE_GOAL) {
					this->createSegment(&(this->goalVSegments[counter]), col, row, size, ccolor, counter, true);
				}

				row+=size;
				checkPrevius = false;
			} else {
				row+=step;
				checkPrevius = true;
			}
		}

		/*Scan segments under the field border*/
		// Escaneamos toda la imagen
		row = rowini/2.0;
		//row = 0;
		while(row < rowend) {
			ccolor = _ImageHandler->getColor(col, row);
			/*Get the size of the segment*/
			size = _ImageHandler->getSegmentSizeDown(col, row, rowend, ccolor);
			if(size >= MIN_SEGMENT_SIZE) {
				this->createSegment(&(this->segments[counter]), col, row, size, ccolor, counter, true);
				/*If it's not green, check the columns between the current colums*/
				if(ccolor != ImageInput::CGREEN && counter > 0) {
					list<Segment>::reverse_iterator last = this->segments[counter].rbegin();
					_ImageHandler->checkPreviousCols((*last).x, (*last).miny, (*last).maxy, col-ImageInput::IMG_STEP, rowini, rowend, ImageInput::IMG_STEP/4, ccolor);
				}
			}

			row+=size;
		}

		counter++;
	}
}

void
Segmentation::calcHorizontalSegments(Horizon * horizon)
{

	int col, row;
	int marginx = 1;
	int marginy = ImageInput::IMG_MARGIN_STEP;
	int counter, ccolor, size;
	int rowh, colend;
	bool goalFound;
	int step = ImageInput::IMG_MINISTEP;
	bool checkPrevius = false;

	/*Get the position of the horizon*/
	rowh = horizon->getHorizonRow();
	goalFound = true;
	counter = 0;

	/*Scan over the horizon or until we don't find goals*/
	for(row=marginy;row<ImageInput::IMG_HEIGHT && (row < rowh || goalFound);row=row+ImageInput::IMG_STEP) {

		goalFound = false;
		checkPrevius = false;
		colend = ImageInput::IMG_WIDTH -1 - marginx;	/*TODO: Descartar partes de BodyDetector*/
		col = marginx;

		while(col < colend) {
		
			ccolor = _ImageHandler->getColor(col, row);
			if(ccolor == ImageInput::CYELLOW || ccolor == ImageInput::CBLUE) {

				/*If we skipped a pixel, check it*/
				if(checkPrevius) {
					if(ccolor == _ImageHandler->getColor(col-1, row))
						col = col-1;
				}

				/*Get the size of the segment*/
				size = _ImageHandler->getSegmentSizeRight(col, row, colend, ccolor);
				if(size >= MIN_SEGMENT_SIZE_GOAL) {
					this->createSegment(&(this->goalHSegments[counter]), col, row, size, ccolor, counter, false);
					goalFound = true;
				}

				col+=size;
				checkPrevius = false;
			} else {
				col+=step;
				checkPrevius = true;
			}
		}	

		counter++;
	}
}

void
Segmentation::createSegment(list<Segment> * list, int x, int y, int size, int color, int col, bool vertical)
{
	Segment segment;

	segment.x = x;	
	segment.y = y;
	if(vertical) {
		segment.yend = y + (size-1);
		segment.xend = x;
	} else {
		segment.yend = y;
		segment.xend = x + (size-1);
	}
	segment.miny = segment.y;
	segment.maxy = segment.yend;
	segment.length = size;
	segment.color = color;	
	segment.indexcol = col;

	/*Save the segment in the list*/
	list->push_back(segment);
}

void
Segmentation::drawSegments(IplImage* src)
{
	CvPoint p, q, r1, r2, s1, s2;
	int col, row;

	/*Draw segments*/
	for(col=0;col<ImageInput::IMG_STEPS_V;col++) {
		for(list<Segment>::iterator seg = this->segments[col].begin(); seg != this->segments[col].end(); seg++) {
						
			p.x = (*seg).x;
			p.y = (*seg).y;
			q.x = (*seg).x;
			q.y = (*seg).yend;

			r1.x = (*seg).x-2;
			r1.y = (*seg).miny;
			r2.x = (*seg).x;
			r2.y = (*seg).miny;

			s1.x = (*seg).x-2;
			s1.y = (*seg).maxy;
			s2.x = (*seg).x;
			s2.y = (*seg).maxy;

			this->drawLine(src, p, q, (*seg).color);

			if((*seg).color == ImageInput::CWHITE) {
				this->drawLine(src, r1, r2, (*seg).color);
				this->drawLine(src, s1, s2, (*seg).color);
			}	
		}
	}

	/*Draw vertical goal segments*/
	for(col=0;col<ImageInput::IMG_STEPS_V;col++) {
		for(list<Segment>::iterator seg = this->goalVSegments[col].begin(); seg != this->goalVSegments[col].end(); seg++) {
			p.x = (*seg).x;
			p.y = (*seg).y;
			q.x = (*seg).x;
			q.y = (*seg).yend;

			this->drawLine(src, p, q, (*seg).color);
		}
	}
	/*Draw horizontal goal segments*/
	for(row=0;row<ImageInput::IMG_STEPS_H;row++) {
		for(list<Segment>::iterator seg = this->goalHSegments[row].begin(); seg != this->goalHSegments[row].end(); seg++) {
			p.x = (*seg).x;
			p.y = (*seg).y;
			q.x = (*seg).xend;
			q.y = (*seg).y;

			this->drawLine(src, p, q, (*seg).color);
		}
	}
}

void
Segmentation::drawLine(IplImage* src, CvPoint p, CvPoint q, int color) {

	if(color == ImageInput::CUNKNOWN) {
		cvLine(src, p, q, CV_RGB(255,0,0), 1, CV_AA, 0);
	}
	if(color == ImageInput::CORANGE) {
		cvLine(src, p, q, CV_RGB(0,95,255), 1, CV_AA, 0);
	}
	if(color == ImageInput::CGREEN) {
		cvLine(src, p, q, CV_RGB(0,255,0), 1, CV_AA, 0);
	}
	if(color == ImageInput::CBLUE) {
		cvLine(src, p, q, CV_RGB(0,255,255), 1, CV_AA, 0);
	}
	if(color ==  ImageInput::CYELLOW) {
		cvLine(src, p, q, CV_RGB(255,255,0), 1, CV_AA, 0);
	}
	if(color == ImageInput::CWHITE) {
		cvLine(src, p, q, CV_RGB(0,0,0), 1, CV_AA, 0);
	}
	if(color == ImageInput::CCYAN) {
		cvLine(src, p, q, CV_RGB(0, 255, 255), 1, CV_AA, 0);
	}
	if(color == ImageInput::CMAGENTA) {
		cvLine(src, p, q, CV_RGB(255, 0, 255), 1, CV_AA, 0);
	}
}

