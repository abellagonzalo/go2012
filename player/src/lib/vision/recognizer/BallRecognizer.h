/*
 * Name: BallRecognizer.h
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

#ifndef BALLRECOGNIZER_H_
#define BALLRECOGNIZER_H_

#include "Recognizer.h"
#include "../../Components/Kinematics.h"

class BallRecognizer : public Recognizer {
public:
	BallRecognizer(Horizon *horizon);
	virtual ~BallRecognizer();

	void recognize(int* blobs, bool debug = false);

private:
	void getExpectedSize(int x, int y, int* width, int* height, bool debug);

	Kinematics *kinematics;
	CvMemStorage *storage;
	CvSeqWriter writer;

	static const float BALL_RADIUS = 32.5;
};

#endif /* BALLRECOGNIZER_H_ */
