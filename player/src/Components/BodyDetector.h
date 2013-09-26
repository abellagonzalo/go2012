#ifndef BODYDETECTOR_H
#define BODYDETECTOR_H

#include "Component.h"
#include "Singleton.h"
#include "Kinematics.h"
#include "ImageInput.h"
#include "Geometry.h"

typedef struct {
	HPoint3D line;
	int maxx;
	int minx;
	int maxy;
	int miny;
} BodySegment;

class BodyDetector : public Component, public Singleton<BodyDetector>
{
public:

	BodyDetector();
	~BodyDetector();

	void init(const string newName, AL::ALPtr<AL::ALBroker> parentBroker);

	void forceStep();
	void step();
	void stop();

	int getMaxRow(int index);

	void drawBody(IplImage* src);

private:

	void printMatrix(vector<float> matrix);
	void calcFoots();	
	/*Create all segments of the body*/
	void createSegments();	
	void createNewSegment(HPoint2D p1, HPoint2D p2);	

	void drawLine(IplImage* src, HPoint2D p1, HPoint2D p2);

	void calcBodyLine();

	static const int FOOTS_X_FRONT = 120;
	static const int FOOTS_X_BACK = 100;
	static const int FOOTS_Y_OUT = 100;
	static const int FOOTS_Y_IN = 50;

	AL::ALPtr<AL::ALMotionProxy> pmotion;

	float eqA, eqB, eqC;

	float escale;

	HPoint3D RFoot[4];
	HPoint2D RFoot2D[4];
	HPoint3D LFoot[4];	
	HPoint2D LFoot2D[4];

	vector<BodySegment> segments;
	HPoint2D borders[ImageInput::IMG_STEPS_V];

	Kinematics * _Kinematics;
};

#endif
