/*
 * Name: Region.h
 * @Author: Eduardo Perdices (eperdices@gsyc.es)
 *
 * Description: Class to collect up several Segments with the same color.
 *
 * Copyright (C) 2009-2010 Universidad Rey Juan Carlos
 * All Rights Reserved.
 */

#include "Region.h"

Region::Region()
{
	this->init = NULL;
}

Region::~Region()
{
}

int
Region::calcMoment00() {

	Segment * seg;
	int tot = 0;

	seg = this->init;
	while(seg!=NULL) {
		tot += seg->length;
		seg = seg->next;
	}

	/*If it happens, something is wrong with total_size*/
	if(tot != this->total_size) {
		cerr << "  Error in region->tot != total_size " << tot << " != " << this->total_size << endl;
		cerr << "  El color de region es " << color << endl;
		this->total_size = tot;
	}

  return tot;
}

int
Region::calcMoment10() {

	Segment * seg;
	int tot = 0;

	seg = this->init;
	while(seg!=NULL) {
		tot += seg->x * seg->length;
		seg = seg->next;
	}

  return tot;
}

int
Region::calcMoment01() {

	Segment * seg;
	int tot = 0;
	int ymin, ymax;

	seg = this->init;
	while(seg!=NULL) {
		ymin = seg->y;
		ymax = seg->y + seg->length;
		tot += (ymax * (ymax - 1) - ymin * (ymin - 1))/2;
		seg = seg->next;
	}

  return tot;
}

float
Region::calcMoment11(int val_x, int val_y) {

	Segment * seg;
	float tot = 0;
	float ymin, ymax, sum;

	seg = this->init;
	while(seg!=NULL) {
		ymin = (float) seg->y;
		ymax = (float) (seg->y + seg->length);
		sum = gaussianSum(ymax) - gaussianSum(ymin);
		tot += seg->x * sum - seg->length * seg->x * val_y - val_x * sum + seg->length * val_x * val_y;
		seg = seg->next;
	}

  return tot;
}

int
Region::calcMoment20(int val_x) {

	Segment * seg;
	float tot = 0;

	seg = this->init;
	while(seg!=NULL) {
		tot += seg->length * (seg->x-val_x) * (seg->x-val_x);
		seg = seg->next;
	}

  return tot;
}

float
Region::calcMoment02(int val_y) {

	Segment * seg;
	float tot = 0;
	float ymin, ymax;

	seg = this->init;
	while(seg!=NULL) {
		ymin = (float) seg->y;
		ymax = (float) (seg->y + seg->length);
		tot += gaussianSum2(ymax) - gaussianSum2(ymin) - 2*val_y * (gaussianSum(ymax) - gaussianSum(ymin)) + seg->length * val_y*val_y;
		seg = seg->next;
	}

  return tot;
}


