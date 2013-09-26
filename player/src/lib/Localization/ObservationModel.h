#ifndef OBSERVATIONMODEL_H
#define OBSERVATIONMODEL_H

#include "Component.h"
#include "Singleton.h"
#include "Perception.h"
#include "ImageHandler.h"
#include "ImageInput.h"
#include "Geometry.h"
#include "Kinematics.h"
#include "LineDetector.h"
#include "GoalDetector.h"
#include "../../estimation/GoalsModel.h"

using namespace goalDescription;

typedef struct {
	HPoint3D p3D;
	HPoint2D p2D;
	int color;
	int type;
	bool isgoal;
	bool valid;
} TImageObject;

class ObservationModel : public Singleton<ObservationModel>
{
public:

	ObservationModel();
	~ObservationModel();

	/*Get parameters from the image, return false if there are not objects*/
	bool getImageParameters(vector<TImageObject> &objects, bool &hasGoals);

	/*Get the reliability of the current image*/
	float getImageReliability();

	/*Debug image*/
	ImageDataPtr getImgPoints();

	/*Image parameters*/
	static const int MAX_SELECTED_PLINES = 12;

private:

	/*Image procesing*/
	void getPointsFromLine(vector<TImageObject> &points, double xini, double yini, double xfin, double yfin, int color, bool extremes);

	void selectRandom(vector<TImageObject> &points, int maxpoints, vector<TImageObject> &objects, int size);
	int getNextPosition(vector<TImageObject> &points, int from, int size);
	void drawRectangle(IplImage* src, CvPoint p, CvScalar clr, int val);

	ImageHandler * _ImageHandler;
	Kinematics * _Kinematics;
	LineDetector 	*_LineDetector;
	GoalDetector 	*_GoalDetector;

	float current_reliability;

	/*Debug*/
	ImageDataPtr imgPoints;
	IplImage * imgSrc;
};

#endif
