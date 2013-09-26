#ifndef Kinematics_H
#define Kinematics_H

#include "Component.h"
#include "almath/tools/almath.h"
#include "alproxies/almotionproxy.h"
#include "Singleton.h"
#include "ImageInput.h"
#include "vision/colorFilter/simpleColorFilter/SimpleColorFilter.h"
#include "Dictionary.h"
#include "Matrix.h"
#include "progeo.h"
#include <pthread.h>

#define UPPER_CAMERA_KINEMATICS_FILE "/home/nao/bica/kinematics/cameraUpper.conf"
#define LOWER_CAMERA_KINEMATICS_FILE "/home/nao/bica/conf/kinematics/cameraLower.conf"

typedef struct {
	float x;
	float y;
	float z;
	float pan;
	float tilt;
	float roll;
	float foax;
	float foay;
	float foaz;
	float RT[12];
} TKinematics;

class Kinematics : public Component, public Singleton<Kinematics>
{
public:

	Kinematics();
	~Kinematics();

	void init(const string newName, AL::ALPtr<AL::ALBroker> parentBroker);

	void forceStep();
	void step();
	void stop();

	TPinHoleCamera * getCamera();
	TKinematics * getKinematics();

	/*Clone current kinematics*/
	void cloneKinematics(TKinematics * kinematics_out);
	void cloneKinematics(TKinematics kinematics, TKinematics * kinematics_out);

	/*Change between optical coordinates and pixels*/
	void pixel2Optical(TPinHoleCamera * camera, HPoint2D * p);
	void optical2Pixel(TPinHoleCamera * camera, HPoint2D * p);

	/*Obtain a projection in 2D from a position in 3D*/
	void get2DPosition(HPoint3D in, HPoint2D &res);
	void get2DPosition(TPinHoleCamera * camera, HPoint3D in, HPoint2D &res);

	/*Obtain a position in 3D from a 2D image point and a plane*/
	void get3DPosition(HPoint3D &res, HPoint2D in);
	void get3DPosition(TPinHoleCamera * camera, HPoint3D &res, HPoint2D in);

	void get3DPositionX(HPoint3D &res, HPoint2D in, float X);
	void get3DPositionX(TPinHoleCamera * camera, HPoint3D &res, HPoint2D in, float X);
	void get3DPositionY(HPoint3D &res, HPoint2D in, float Y);
	void get3DPositionY(TPinHoleCamera * camera, HPoint3D &res, HPoint2D in, float Y);
	void get3DPositionZ(HPoint3D &res, HPoint2D in, float Z);
	void get3DPositionZ(TPinHoleCamera * camera, HPoint3D &res, HPoint2D in, float Z);

	/*Get neck pan tilt from a 3D point to be centered in the image*/
	void getNeckAngles(float x, float y, float z, float &pan, float &tilt);

	/*Get a camera with different extrinsic parameters*/
	void configureCamera(TPinHoleCamera * camera, float px, float py, float theta);

	/*Get a camera with different extrinsic parameters using different kinematics*/
	void configureCamera(TKinematics kinematics, TPinHoleCamera * camera, float px, float py, float theta);

	/*Draw lines*/
	int drawLine(TPinHoleCamera * camera, HPoint2D * p1, HPoint2D * p2);

	/*Obtain the camera position in 3D*/
	void get3DCameraPosition(HPoint3D &res);

	/*Obtain the 3D trapezoid of the frustum (field of view of the camera)*/
	void getFrustum (HPoint3D &upperLeft3D, HPoint3D &upperRight3D,
			HPoint3D &lowerLeft3D, HPoint3D &lowerRight3D);

	/*Return if a point (on the ground plane) is inside the polygon delimited by the frustum*/
	bool pointInPolygon (int nvert, float *vertx, float *verty, float testx, float testy);
	bool groundPointInImage(HPoint3D *testpoint);

	void updateIntrinsicParams(float u0, float v0, float fdistx, float fdisty, float roll);
	void saveCameraIntrinsicParams();
private:

	void loadCameraIntrinsicParams(const char *upper_file, const char *lower_file);

	void updateKinematics();
	void updateCamera();

	AL::ALPtr<AL::ALMotionProxy> pmotion;

	float upper_u0, upper_v0, upper_fdistx, upper_fdisty, upper_roll;
	float lower_u0, lower_v0, lower_fdistx, lower_fdisty, lower_roll;
	int use_lower;
	float escale;
	TPinHoleCamera camera;
	TKinematics mykinematics;
	Dictionary camIntrinsicsUpper, camIntrinsicsLower;

	vector<float> torsoRT;
	MatrixCM RTrt;

	pthread_mutex_t mutex;
};

#endif
