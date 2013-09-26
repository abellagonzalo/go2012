#ifndef CALIBRATION_H
#define CALIBRATION_H

#include "Singleton.h"
#include "Kinematics.h"
#include "Perception.h"
#include "ImageInput.h"
#include "WorldModel.h"
#include "ImageHandler.h"
#include <string>

#include <IceE/IceE.h>
#include <image.h>

using namespace std;
using namespace bica;

class Calibration: public Singleton<Calibration>, public CalibrationProvider
{
public:
	Calibration();
	~Calibration();

	void init();

	virtual bica::ImageDataPtr getCalibrationImg(const Ice::Current& c);
	virtual bica::CalibrationParamsPtr getCalibrationParams(const Ice::Current& c);
	virtual void setCalibrationParams (const CalibrationParamsPtr& newParams, const Ice::Current& c);
	virtual void saveCalibrationParams (const Ice::Current& c);

private:

	static const int CALIBRATION_MAX_LINES = 50;

	Kinematics *_Kinematics;
	Perception *_Perception;
	WorldModel *_WorldModel;
	ImageHandler * _ImageHandler;

	IplImage * imgSrc;
	TPinHoleCamera camera;
	wmLine3D * mylines;
	float robotx;
	float roboty;
	float robott;
	float u0;
	float v0;
	float fdistx;
	float fdisty;
	float roll;
	int hasChanged;
};

#endif
