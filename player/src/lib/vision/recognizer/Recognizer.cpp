/*
 * Name: Recognizer.cpp
 * @Author: Carlos Agüero (caguero@gsyc.es)
 *
 * Description:
 *
 * Copyright (C) 2008-2009 Universidad Rey Juan Carlos
 * All Rights Reserved.
 */

#include "Recognizer.h"

Recognizer::Recognizer(Horizon *horizon): ImageInput()
  {
	conf.createDictionary();
	loadConf();

	this->horizon = horizon;

	cvRecognized = cvCreateImage( cvSize( ImageInput::IMG_WIDTH, ImageInput::IMG_HEIGHT), 8, ImageInput::IMG_CHANNELS);

	obj.x = obj.y = obj.yaw = obj.pitch = previousX = previousY = 0.0;
	obj.timestamp = obj.size = 0;

	head = Head::getInstance();
	leds = Leds::getInstance();
  }


Recognizer::~Recognizer()
{
	cvReleaseImage(&cvRecognized);
}


void
Recognizer::loadConf()
{
	conf.loadFromFile(RECOGNIZE_CONFIG_FILE);

	if (!((conf.getValueUnsignedInt("BLOB_MIN_PIXELS", &BLOB_MIN_PIXELS)) &&
			(conf.getValueUnsignedInt("BALL_MIN_PIXELS_UPPER", &BALL_MIN_PIXELS_UPPER)) &&
			(conf.getValueUnsignedInt("BALL_MAX_PIXELS_UPPER", &BALL_MAX_PIXELS_UPPER)) &&
			(conf.getValueUnsignedInt("BALL_MIN_SIZE_LOWER", &BALL_MIN_SIZE_LOWER)) &&
			(conf.getValueFloat("BALL_ASPECT_RATIO", &BALL_ASPECT_RATIO)) &&
			(conf.getValueFloat("BALL_MIN_DENSITY", &BALL_MIN_DENSITY)) &&
			(conf.getValueUnsignedInt("BALL_MAX_DIF_EXPECTED_SIZE", &BALL_MAX_DIF_EXPECTED_SIZE)) &&
			(conf.getValueUnsignedInt("NET_MIN_PIXELS", &NET_MIN_PIXELS)) &&
			(conf.getValueUnsignedInt("NET_MIN_SIZE", &NET_MIN_SIZE)) &&
			(conf.getValueInt("NET_MIN_DIST_BASE_TO_HORIZON", &NET_MIN_DIST_BASE_TO_HORIZON))))
		cerr << begin_red << "[Recognizer::loadConf()]: Problem loading recognition parameters " << RECOGNIZE_CONFIG_FILE << end_color << endl;
}


void
Recognizer::init(bool debug)
{
	objectn = objectn2 = lower_blob = 0;
	objectx = objecty = objectmaxx = objectmaxy = objectminx = objectminy = 0.0;
	objectx2 = objecty2 = object2maxx = object2maxy = object2minx = object2miny = 0.0;
	closerDist2previous = FLT_MAX;

	seen = false;

	if (debug)
		cvSetZero(cvRecognized);
}

bool
Recognizer::preRecognize(int *blob)
{
	/*if (blob->area < BLOB_MIN_PIXELS) return false;	// Go to next iteration in case of small blobs

	// Pixel centered on the middle of the blob
	center.x = blob->centroid.x;
	center.y = blob->centroid.y;
	center.h = 1;	//???

	// Get the segment X and Y of the blob
	segmX = max((unsigned int)1, blob->maxx - blob->minx);
	segmY = max((unsigned int)1, blob->maxy - blob->miny);
	size = segmX * segmY;

	// Get the radius of the bigger circunference contained in the blob and the aspect ratio
	radius = max(segmX,segmY) / 2;
	ratio = fabs(max(segmX,segmY) / min(segmX,segmY));

	// Is the blob on an edge?
	onEdge = (blob->miny == 0) || (blob->maxy == (unsigned int)(height - 1)) ||
			(blob->minx == 0) || (blob->maxx == (unsigned int)(width - 1));
*/
	return true;
}

void
Recognizer::postRecognize(bool debug)
{
	struct timeval now_tv;

	obj.n = objectn;
	if (objectn > 0){

		gettimeofday(&now_tv, NULL);
		double now = (double)now_tv.tv_sec;

		previousX = bestX;
		previousY = bestY;

		obj.x = objectx;
		obj.y = objecty;
		obj.size = objectn;
		obj.maxx = objectmaxx;
		obj.minx = objectminx;
		obj.maxy = objectmaxy;
		obj.miny = objectminy;

		obj.relativeX = bestRelX;
		obj.relativeY = bestRelY;

		obj.timestamp = (((int)(now)) % 10000);

		if(fabs(obj.x) < 0.6)
			obj.yaw = head->getPan();
		if(fabs(obj.y) < 0.6)
			obj.pitch = head->getTilt();

		obj.size = objectn;
		//if((fabs(objectx )< 0.6) && (fabs(objecty )< 0.6))
		//else
		//	obj.n = 0;

		/*Second blob*/
		obj2.x = objectx2;
		obj2.y = objecty2;
		obj2.size = objectn2;
		obj2.maxx = object2maxx;
		obj2.minx = object2minx;
		obj2.maxy = object2maxy;
		obj2.miny = object2miny;
	}
}
