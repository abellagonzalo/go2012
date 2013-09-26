/*
 * Name: BGDetector.h
 * @Author: Eduardo Perdices (eperdices@gsyc.es)
 * 
 * Description: Background detector
 *
 * Created on: 21/03/2012
 *
 * Copyright (C) Universidad Rey Juan Carlos
 * All Rights Reserved.
 *
 */

#ifndef BGDETECTOR_H_
#define BGDETECTOR_H_

#include <pthread.h>
#include "Component.h"
#include "Singleton.h"
#include "Kinematics.h"
#include "Perception.h"
#include "ImageHandler.h"
#include "ImageInput.h"
#include "Geometry.h"

#define BGDETECTOR_OWN 1
#define BGDETECTOR_OTHER 2

class BGDetector: public Component, public Singleton<BGDetector> {
public:
	BGDetector();
	~BGDetector();

	void init(const string newName, AL::ALPtr<AL::ALBroker> parentBroker);
	void step();

	void saveGoalFeatures(bool own_goal);
	int compareImages();

private:

	/*Get features*/
	void getFeatures(CvSeq *& keypoints, CvSeq *& descriptors, bool debug, int debug_type);

	/*Match images*/
	int matching(CvSeq * keypoints1, CvSeq * descriptors1, CvSeq * keypoints2, CvSeq * descriptors2, bool debug);

	/*Matching*/
	double compareSURFDescriptors( const float* d1, const float* d2, double best, int length );
	int naiveNearestNeighbor( const float* vec, int laplacian, const CvSeq* model_keypoints, const CvSeq* model_descriptors );
	void findPairs( const CvSeq* objectKeypoints, const CvSeq* objectDescriptors, const CvSeq* imageKeypoints, const CvSeq* imageDescriptors, vector<int>& ptpairs );
	int locatePlanarObject( const CvSeq* objectKeypoints, const CvSeq* objectDescriptors, const CvSeq* imageKeypoints, const CvSeq* imageDescriptors, const CvPoint src_corners[4], CvPoint dst_corners[4] );

	Kinematics * _Kinematics;
	ImageHandler * _ImageHandler;
	Perception * _Perception;

	/*Debug*/
	IplImage* src_gray;
	IplImage * imgSrc;
	IplImage * imgFinal;

	CvSeq * keypoints_last;
	CvSeq * descriptors_last;
	CvSeq * keypoints_own;
	CvSeq * descriptors_own;
	CvSeq * keypoints_other;
	CvSeq * descriptors_other;
};

#endif /* DETECTOR_H_ */
