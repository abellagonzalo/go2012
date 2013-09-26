/*
 * Name: BallRecognizer.cpp
 * @Author: Carlos Agüero (caguero@gsyc.es)
 * 
 * Description: 
 *
 * Created on: 06/02/2010
 *
 * Copyright (C) Universidad Rey Juan Carlos
 * All Rights Reserved.
 *
 */

#include "BallRecognizer.h"

BallRecognizer::BallRecognizer(Horizon *horizon): Recognizer (horizon)
  {
	kinematics = Kinematics::getInstance();
	storage = cvCreateMemStorage();
  }

BallRecognizer::~BallRecognizer()
{
}

void
BallRecognizer::getExpectedSize(int x, int y, int* width, int* height, bool debug )
{
	HPoint2D in = {x, y, 1.0f};
	HPoint3D out;
	HPoint2D points2D[6];
	HPoint3D* points3D[6];

	kinematics->get3DPositionZ(out, in, BALL_RADIUS);
	relX = out.X;
	relY = out.Y;

	HPoint3D p1_3D = {out.X + BALL_RADIUS, out.Y + BALL_RADIUS, out.Z, 1.0f};
	HPoint3D p2_3D = {out.X + BALL_RADIUS, out.Y - BALL_RADIUS, out.Z, 1.0f};
	HPoint3D p3_3D = {out.X - BALL_RADIUS, out.Y + BALL_RADIUS, out.Z, 1.0f};
	HPoint3D p4_3D = {out.X - BALL_RADIUS, out.Y - BALL_RADIUS, out.Z, 1.0f};
	HPoint3D p5_3D = {out.X, out.Y, out.Z + BALL_RADIUS, 1.0f};
	HPoint3D p6_3D = {out.X, out.Y, out.Z - BALL_RADIUS, 1.0f};
	points3D[0] = &p1_3D;
	points3D[1] = &p2_3D;
	points3D[2] = &p3_3D;
	points3D[3] = &p4_3D;
	points3D[4] = &p5_3D;
	points3D[5] = &p6_3D;

	cvStartWriteSeq ( CV_32SC2, sizeof(CvSeq), sizeof(CvPoint), storage, &writer);

	for (int i = 0; i < 6; i++) {
		kinematics->get2DPosition(*points3D[i], points2D[i]);
		CvPoint p = {points2D[i].x, points2D[i].y};
		CV_WRITE_SEQ_ELEM(p, writer);

		if (debug) {
			cvCircle(cvRecognized, cvPoint(points2D[i].x,points2D[i].y),1, CV_RGB(255.,255.,255.), -1);
		}
	}

	CvSeq* seq = cvEndWriteSeq( &writer );
	CvRect boundingBox = cvBoundingRect(seq);

	*width = boundingBox.width;
	*height = boundingBox.height;

	if (debug) {
		cvRectangle(cvRecognized, cvPoint(boundingBox.x, boundingBox.y), cvPoint(boundingBox.x + *width,
				boundingBox.y + *height), CV_RGB(255.,255.,255.));
	}
}

void
BallRecognizer::recognize(int* blobs, bool debug)
{
	init(debug);
	kinematics->step();

	/*for (int::const_iterator it=blobs->begin(); it!=blobs->end(); ++it)
	{
		CvBlob *blob=(*it).second;
		if (!preRecognize(blob)) continue;
		// Get the density of the blob (number of pixels / size of the blob)
		density = blob->area / size;*/

		// ---------- Debug -------------
		/*cout << "-----\nPelota\n Area: " << blob->area << endl;
		cout << "Center: (" << blob->centroid.x << "," << blob->centroid.y << ")\n";
		cout << "Under horizon?: " << horizon->isUnderHorizon(center) << endl;
		cout << "Size: " << size << endl;
		cout << "Density: " << density << endl;
		cout << "Aspect ratio: " << ratio << endl;
		cout << "On edge? " << onEdge << endl;
		cout << "Bounds: (" << blob->minx << "," << blob->miny << ")(" << blob->maxx << "," << blob->maxy << ")\n";*/
		// ---------- Fin Debug .........

		/*if (!horizon->isUnderHorizon(center)) continue;
		if (size < BALL_MIN_PIXELS_UPPER) continue;
		//if (size >= BALL_MAX_PIXELS_UPPER) continue;
		if ((!onEdge) && (ratio >= BALL_ASPECT_RATIO)) continue;
		if ((onEdge) && (density < BALL_MIN_DENSITY)) continue;
		int expectedWidth, expectedHeight;
		getExpectedSize(blob->centroid.x, blob->centroid.y, &expectedWidth, &expectedHeight, debug);
		if ( fabs(segmX - expectedWidth) > BALL_MAX_DIF_EXPECTED_SIZE) continue;
		if ( fabs(segmY - expectedHeight) > BALL_MAX_DIF_EXPECTED_SIZE) continue;

		float newDistance = sqrt(pow(center.x - previousX, 2) + pow(center.y - previousY, 2));
		if (newDistance < closerDist2previous){	// Closer Blob to last iteration
			objectn = (int)blob->area;

			closerDist2previous = newDistance;

			bestX = blob->centroid.x;
			bestY = blob->centroid.y;
			best_radius = radius;
			bestRelX = relX;
			bestRelY = relY;
			seen = true;
		}

		if (debug)
			cvCircle(cvRecognized, cvPoint(blob->centroid.x,blob->centroid.y),radius, CV_RGB(247.,217.,126.), -1);
	}

	leds->setLed(Leds::LEYEL_LED, Leds::COLOR_ORANGE, seen);
	leds->setLed(Leds::LEYEI_LED, Leds::COLOR_ORANGE, seen);*/

	if (seen) {
		objectx = ((float)bestX - (width / 2.0)) / (width / 2.0);
		objecty = ((float)bestY - (height / 2.0)) / (height / 2.0);

		if (debug) {
			cvCircle(cvRecognized, cvPoint(bestX,bestY),best_radius, CV_RGB(255.,198.,29.), -1);
		}
	}

	postRecognize(debug);
}
