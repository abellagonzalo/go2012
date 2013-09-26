/*
 * Name: Adapter.h
 * @Author: Gonzalo Abella (abellagonzalo@gmail.com)
 * Updated: 25/01/2011 by Gonzalo Abella
 *
 */

#ifndef Adapter_M
#define Adapter_M

#include <Ice/Ice.h>
#include <IceUtil/Thread.h>
#include <IceUtil/Mutex.h>

#include "robocup_ssl_client.h"
#include "messages_robocup_ssl_detection.pb.h"
#include "messages_robocup_ssl_geometry.pb.h"
#include "messages_robocup_ssl_wrapper.pb.h"


class Adapter : public IceUtil::Thread {

public:
	Adapter();
	virtual void run();
	void stop();
	SSL_GeometryData getGeometry();
	SSL_DetectionBall getBall();
	bool hasBlueRobot(int i);
	bool hasYellowRobot(int i);
	SSL_DetectionRobot getBlueRobot(int i);
	SSL_DetectionRobot getYellowRobot(int i);

private:
	SSL_DetectionFrame _detectionFrame;
	SSL_GeometryData _geometryData;
	bool _finishThread;
	IceUtil::Mutex _mutex;
	SSL_DetectionBall _ball;
	SSL_DetectionRobot _robot;

	void setDetection(SSL_DetectionFrame detectionFrame);
	void setGeometry(SSL_GeometryData geometryData);
	void setBall(SSL_DetectionBall ball);

};

typedef IceUtil::Handle<Adapter> AdapterPtr;

#endif
