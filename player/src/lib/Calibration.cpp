#include "Calibration.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>

using namespace std;

Calibration::Calibration()
{
	_Perception = Perception::getInstance();
	_Kinematics = Kinematics::getInstance();
	_WorldModel = WorldModel::getInstance();
	_ImageHandler = ImageHandler::getInstance();

	this->robotx = 0.0;
	this->roboty = 0.0;
	this->robott = 0.0;
	this->hasChanged = 0;

	this->mylines = new wmLine3D[CALIBRATION_MAX_LINES];

	this->imgSrc = cvCreateImage(cvSize(ImageInput::IMG_WIDTH, ImageInput::IMG_HEIGHT), ImageInput::BYTES_PER_CHANNEL, ImageInput::IMG_CHANNELS);
}

Calibration::~Calibration()
{
	delete this->mylines;
	cvReleaseImage(&imgSrc);
}

void 
Calibration::init()
{
}

bica::ImageDataPtr
Calibration::getCalibrationImg(const Ice::Current& c)
{
	bica::ImageDataPtr reply(new bica::ImageData);
	int imgSize = this->imgSrc->width*this->imgSrc->height*this->imgSrc->nChannels;

	IceUtil::Time t = IceUtil::Time::now();
	reply->timeStamp.seconds = (long)t.toSeconds();
	reply->timeStamp.useconds = (long)t.toMicroSeconds() - reply->timeStamp.seconds*1000000;
	reply->pixelData.resize(imgSize);

	HPoint2D p1, p2, p3, p4;
	CvPoint p, q;
	int i, numl;

	_Perception->step();
	_Kinematics->step();

	_ImageHandler->getImageRGB(this->imgSrc->imageData, false, 0);

	/*Get the world model*/
	numl = _WorldModel->getCalibrationLines(this->mylines, CALIBRATION_MAX_LINES);

	/*Configure our camera*/
	_Kinematics->configureCamera(&(this->camera), robotx, roboty, robott);

	if(this->hasChanged) {
		this->camera.u0 = this->u0;
		this->camera.v0 = this->v0;
		this->camera.fdistx = this->fdistx;
		this->camera.fdisty = this->fdisty;
		this->camera.roll = this->roll;
		update_camera_matrix(&(this->camera));
	}

	/*Draw lines*/
	for(i=0;i<numl;i++) {
		/*Project 3D points in 2D*/
		_Kinematics->get2DPosition(&(this->camera), this->mylines[i].p1, p1);
		_Kinematics->get2DPosition(&(this->camera), this->mylines[i].p2, p2);
		_Kinematics->get2DPosition(&(this->camera), this->mylines[i].p3, p3);
		_Kinematics->get2DPosition(&(this->camera), this->mylines[i].p4, p4);
		if(_Kinematics->drawLine(&(this->camera), &p1, &p2)) {	
			p.x = p1.x; p.y = p1.y; q.x = p2.x; q.y = p2.y;
			cvLine(this->imgSrc, p, q, CV_RGB(255,0,0), 1, CV_AA, 0);
		}
		if(_Kinematics->drawLine(&(this->camera), &p3, &p4)) {
			p.x = p3.x; p.y = p3.y; q.x = p4.x; q.y = p4.y;
			cvLine(this->imgSrc, p, q, CV_RGB(255,0,0), 1, CV_AA, 0);
		}
	}

	memmove( &(reply->pixelData[0]), (char *) this->imgSrc->imageData, imgSize);//copy data to reply

	return reply;
}

bica::CalibrationParamsPtr
Calibration::getCalibrationParams(const Ice::Current& c)
{
	bica::CalibrationParamsPtr params(new bica::CalibrationParams);

	this->u0 = _Kinematics->getCamera()->u0;
	this->v0 = _Kinematics->getCamera()->v0;
	this->fdistx = _Kinematics->getCamera()->fdistx;
	this->fdisty = _Kinematics->getCamera()->fdisty;
	this->roll = _Kinematics->getCamera()->roll;

	params->robotx = robotx;
	params->roboty = roboty;
	params->robott = robott;
	params->u0 = u0;
	params->v0 = v0;
	params->fdistx = fdistx;
	params->fdisty = fdisty;
	params->roll = roll;

	return params;
}

void
Calibration::saveCalibrationParams (const Ice::Current& c)
{

	_Kinematics->updateIntrinsicParams(this->u0, this->v0, this->fdistx, this->fdisty, this->roll);

	_Kinematics->saveCameraIntrinsicParams();
}

void
Calibration::setCalibrationParams (const CalibrationParamsPtr& newParams, const Ice::Current& c)
{
	this->hasChanged = 1;

	this->robotx = newParams->robotx;
	this->roboty = newParams->roboty;
	this->robott = newParams->robott;
	this->u0 = newParams->u0;
	this->v0 = newParams->v0;
	this->fdistx = newParams->fdistx;
	this->fdisty = newParams->fdisty;
	this->roll = newParams->roll;
}

