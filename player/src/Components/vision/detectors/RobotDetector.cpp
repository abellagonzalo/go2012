/*
 * Name: RobotDetector.h
 * @Author: Gonzalo Abella (abellagonzalo@gmail.com)
 * 
 * Description: Class to detect robots from the regions
 *
 * Created on: 02/03/2012
 *
 * Copyright (C) Universidad Rey Juan Carlos
 * All Rights Reserved.
 *
 */

#include "RobotDetector.h"

RobotDetector::RobotDetector() : Detector()
{

	if (GameController::getInstance()->getMyColor() == TEAM_BLUE) {
		_teammateColor = ImageInput::CCYAN;
		_opponentColor = ImageInput::CMAGENTA;
	} else {
		_teammateColor = ImageInput::CMAGENTA;
		_opponentColor = ImageInput::CCYAN;
	}



	// Robot model
	robots = new RobotModel();
}

RobotDetector::~RobotDetector()
{
}

void
RobotDetector::init(const string newName, AL::ALPtr<AL::ALBroker> parentBroker) {
	Component::init(newName, parentBroker);
	perception = Perception::getInstance();
	body = Body::getInstance();
	kinematics =  Kinematics::getInstance();


	this->setFreqTime(SHORT_RATE);
}

void
RobotDetector::step()
{
	perception->step();
	body->step();
	kinematics->step();

	if (!isTime2Run())
		return;

	if (GameController::getInstance()->getMyColor() == TEAM_BLUE) {
		_teammateColor = ImageInput::CCYAN;
		_opponentColor = ImageInput::CMAGENTA;
		//cerr<<"Soy cyan"<<endl;
	} else {
		_teammateColor = ImageInput::CMAGENTA;
		_opponentColor = ImageInput::CCYAN;
		//cerr<<"Soy magenta"<<endl;
	}


	startDebugInfo();

	this->detect(true);
	robots->updateFromOdometry();
	//robots->updateFromObservationOpponents(getOpponents());
	//robots->updateFromObservationTeammates(getTeammates());

	endDebugInfo();
}

void
RobotDetector::detect(bool debug)
{
	//cerr<<"========================================================="<<endl;
	// Clear saved elements
	teammates.clear();
	opponents.clear();

	// Validate regions
	_RegionBuilder->validateRegions(_teammateColor);
	_RegionBuilder->validateRegions(_opponentColor);

	// Get robots regions
	list<RobotRegion*> *teammateRegions = _RegionBuilder->getTeammateRegions();
	list<RobotRegion*> *opponentRegions = _RegionBuilder->getOpponentRegions();

	//cerr<<"Mates regions: "<<teammateRegions->size()<<endl;
	//cerr<<"Opponents regions: "<<opponentRegions->size()<<endl;

	// Teammates
	list<RobotRegion*>::iterator teammateRegionIter=teammateRegions->begin();
	while (teammateRegionIter != teammateRegions->end()) {
		HPoint3D p3D;
		RobotRegion region = *(*teammateRegionIter);

		// Calculate 3D position
		_Kinematics->get3DPosition(p3D, region.getGroundHypothesisPixel());

		// Check height of the marker
		if (checkMarkerHeight(p3D, region.getMassCenter())) {
			AbstractSample sample;
			sample.p3D = p3D;
			teammates.push_back(sample);
		}

		teammateRegionIter++;
	}

	// Opponents
	list<RobotRegion*>::iterator opponentRegionIter=opponentRegions->begin();
	while (opponentRegionIter != opponentRegions->end()) {
		HPoint3D p3D;
		RobotRegion region = *(*opponentRegionIter);

		// Calculate 3D position
		_Kinematics->get3DPosition(p3D, region.getGroundHypothesisPixel());

		// Check height of the marker
		if (checkMarkerHeight(p3D, region.getMassCenter())) {
			AbstractSample sample;
			sample.p3D = p3D;
			opponents.push_back(sample);
		}

		opponentRegionIter++;
	}

	// Print.
	list<AbstractSample>::iterator teammateSampleIter=teammates.begin();
	while (teammateSampleIter != teammates.end()) {
		//cerr << "  Tenemos un compañero en " << teammateSampleIter->p3D.X << ", " << teammateSampleIter->p3D.Y << ", " << teammateSampleIter->p3D.Z << endl;
		teammateSampleIter++;
	}
	list<AbstractSample>::iterator opponentSampleIter=opponents.begin();
	while (opponentSampleIter != opponents.end()) {
		//cerr << "  Tenemos un oponente en " << opponentSampleIter->p3D.X << ", " << opponentSampleIter->p3D.Y << ", " << opponentSampleIter->p3D.Z << endl;
		opponentSampleIter++;
	}
	//cerr<<"========================================================="<<endl;
}

bool
RobotDetector::checkMarkerHeight(HPoint3D groundPoint, HPoint2D massCenter) {

	// Check min distance
	HPoint2D minMarkerHeightPixel;
	HPoint3D minMarkerHeight = groundPoint;
	minMarkerHeight.Z = MIN_MARKER_HEIGHT_MM;
	_Kinematics->get2DPosition(minMarkerHeight, minMarkerHeightPixel);

	// Check max distance
	HPoint2D maxMarkerHeightPixel;
	HPoint3D maxMarkerHeight = groundPoint;
	maxMarkerHeight.Z = MAX_MARKER_HEIGHT_MM;
	_Kinematics->get2DPosition(maxMarkerHeight, maxMarkerHeightPixel);

	//cerr<<"["<<minMarkerHeightPixel.y<<","<<maxMarkerHeightPixel.y<<"] mass center = "<<massCenter.y<<endl;

	if ( (minMarkerHeightPixel.y > massCenter.y) &&
			(maxMarkerHeightPixel.y < massCenter.y) ) {
		return true;
	}

	return false;
}

void
RobotDetector::drawValidRegions(IplImage* src)
{

}

void
RobotDetector::drawDetected(IplImage* src) {
	// Teammates
	list<AbstractSample>::iterator teammateSampleIter=teammates.begin();
	while (teammateSampleIter != teammates.end()) {
		drawRobot(src, teammateSampleIter->p3D, _teammateColor);
		teammateSampleIter++;
	}

	// Opponents
	list<AbstractSample>::iterator opponentSampleIter=opponents.begin();
	while (opponentSampleIter != opponents.end()) {
		drawRobot(src, opponentSampleIter->p3D, _opponentColor);
		opponentSampleIter++;
	}
}

void
RobotDetector::drawRobot(IplImage* src, HPoint3D center, int color) {
	HPoint3D topLeftPoint = center;
	topLeftPoint.Y += ROBOT_WIDTH/2;
	topLeftPoint.Z += ROBOT_HEIGHT;
	topLeftPoint.H = 1.0;
	HPoint2D topLeftPixel;
	_Kinematics->get2DPosition(topLeftPoint, topLeftPixel);

	HPoint3D bottomRightPoint = center;
	bottomRightPoint.Y -= ROBOT_WIDTH/2;
	bottomRightPoint.H = 1.0;
	HPoint2D bottomRightPixel;
	_Kinematics->get2DPosition(bottomRightPoint, bottomRightPixel);

	CvPoint topLeftCvPoint, bottomRightCvPoint;
	topLeftCvPoint.x = topLeftPixel.x;
	topLeftCvPoint.y = topLeftPixel.y;
	bottomRightCvPoint.x = bottomRightPixel.x;
	bottomRightCvPoint.y = bottomRightPixel.y;

	CvScalar cvColor;
	if (color == _teammateColor) {
		cvColor = CV_RGB(0,255,255);
	} else if (color == _opponentColor) {
		cvColor = CV_RGB(255,0,255);
	}

	cvRectangle(src, topLeftCvPoint, bottomRightCvPoint, cvColor, 2);
}

