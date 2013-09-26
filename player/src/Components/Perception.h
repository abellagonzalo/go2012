#ifndef PERCEPTION_H
#define PERCEPTION_H

#include <iostream>
#include <cmath>
#include <pthread.h>
#include "Component.h"
#include "alvision/alvisiondefinitions.h"
#include "alproxies/alvideodeviceproxy.h"
#include "alvision/alimage.h"

#include "Singleton.h"
#include "ImageInput.h"
#include "Kinematics.h"
#include "BodyDetector.h"
#include "vision/colorFilter/simpleColorFilter/SimpleColorFilter.h"
#include "vision/recognizer/Horizon.h"
#include "ImageHandler.h"
#include "Segmentation.h"
#include "RegionBuilder.h"
#include "BallDetector.h"
#include "LineDetector.h"
#include "GoalDetector.h"
#include "RobotDetector.h"

#include <IceE/IceE.h>
#include <image.h>
#include <camera.h>

using namespace std;
using namespace bica;

class BallDetector;
class GoalDetector;
class RobotDetector;


class Perception : public Component, public Singleton<Perception>, public ImageSelector
{
public:

	Perception();
	~Perception();

	void init(const string newName, AL::ALPtr<AL::ALBroker> parentBroker);
	void step();

	void shortStep(); //For Wave3D
	void forceStep(); //For BGDetector

	ImageHandler *getImageHandler() {return _ImageHandler;};

	inline SimpleColorFilter* getColorFilter() { return this->colorFilter; };
	inline Horizon* getHorizon() { return _Horizon; };
	inline int getCam() { return cam; };
	inline void lock() { pthread_mutex_lock(&mutex); };
	inline void unlock() { pthread_mutex_unlock(&mutex); };
	void setCam(const int cam);

	// External methods
	bool setCamParam(const int param, const int value);
	int getCamParam(const int param);
	void loadExternalImg(char *img);
	//ALValue loadExternalImg(ALValue newImg, const string& typeImage);
	void setHSVFilter(const int cameraSel, const string objectSel,
			const float _hmin, const float _hmax, const float _smin,
			const float _smax, const float _vmin, const float _vmax);

	virtual bica::ImageDataPtr getImageData(const Ice::Current& c);
	virtual jderobot::ImageDataPtr getImageDataJderobot(const Ice::Current& c);
	//virtual void getImageData_async(const bica::AMD_ImageProvider_getImageDataPtr& cb,
	//			const Ice::Current& c);
	virtual ImageDescriptionPtr getImageDescription(const Ice::Current& c);
	virtual string test(const Ice::Current& c);
	virtual bica::ImageDataPtr getImageDataWithFeatures(FeaturesType type, const Ice::Current& c);
	virtual HSVFilterPtr getHSVFilter (CameraType cam, ObjectsType obj, const Ice::Current& c);
	virtual void setHSVFilter (CameraType cam, ObjectsType obj, const HSVFilterPtr& newFilter, const Ice::Current& c);
	virtual void setCam (CameraType cam, const Ice::Current& c);
	//virtual void setImageType(FeaturesType type, const Ice::Current& c );

	long getWaitingTime();
	
	long getImageTs() {return image_ts;};

private:

	void initCamera();
	bool newImage(bool every=true);
	void loadCameraParams(string upper_file, string lower_file);

	/*Draw debug objects*/
	void drawSegments(IplImage* src);
	void drawRegions(IplImage* src);
	void drawDetected(IplImage* src);

	static const string UPPER_CAMERA_CONFIG_FILE;
	static const string LOWER_CAMERA_CONFIG_FILE;

	int cam;
	bool camera_initialized;
	string fLEMname;
	ALValue imgDebug, filterParams;
	char * colorSrc;
	IplImage *cvImage, *cvAux;
	pthread_mutex_t mutex, mutexstep;
	Dictionary	camConfUpper, camConfLower;
	SimpleColorFilter *colorFilter;

	bool imageTaken;

	AL::ALVideoDeviceProxy *pcamera;
	bica::ImageDescriptionPtr imgDescription;

	Horizon		* _Horizon;
	Kinematics	* _Kinematics;
	BodyDetector	* _BodyDetector;
	ImageHandler * _ImageHandler;
	Segmentation * _Segmentation;
	RegionBuilder * _RegionBuilder;
	BallDetector *_BallDetector;
	LineDetector *_LineDetector;
	GoalDetector *_GoalDetector;
	RobotDetector *_RobotDetector;

	long long lastTimeStamp;
	long wtime;

	//capture thread
	pthread_t capture_thread;
	bool newImageTaken;
	char imgBuff[ImageInput::IMG_WIDTH * ImageInput::IMG_HEIGHT * ImageInput::IMG_CHANNELS_YUV];
	long image_ts;
	long imagereq_ts;
protected:
	static void * EntryPoint(void*);
	void Capture();

};

#endif
