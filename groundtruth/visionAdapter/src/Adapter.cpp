/*
 * Name: Adapter.cpp
 * @Author: Gonzalo Abella (abellagonzalo@gmail.com)
 * Updated: 25/01/2011 by Gonzalo Abella
 *
 */

#include "Adapter.h"

#include <iostream>


using namespace std;

Adapter::Adapter() {
	_finishThread = false;
}

void 
Adapter::stop() {
	_finishThread = true;
}

void
Adapter::run() {
	RoboCupSSLClient client;
    client.open(true);
	SSL_WrapperPacket packet;

	while(!_finishThread) {
        if (client.receive(packet)) {

			if (packet.has_detection()) {
				setDetection(packet.detection());

				// Set ball
				if (packet.detection().balls_size()>0) {
					setBall(packet.detection().balls(0));
				}
			}

			if (packet.has_geometry()) {
				setGeometry(packet.geometry());
			}
		}
	}
		
	// Close client
	client.close();
}


void
Adapter::setDetection(SSL_DetectionFrame detectionFrame) {
	IceUtil::Mutex::Lock lock(_mutex);
	_detectionFrame = detectionFrame;
}

void 
Adapter::setGeometry(SSL_GeometryData geometryData) {
	IceUtil::Mutex::Lock lock(_mutex);
	_geometryData = geometryData;
}

	
SSL_GeometryData 
Adapter::getGeometry() { 
	IceUtil::Mutex::Lock lock(_mutex);
	return _geometryData; 
}

SSL_DetectionBall
Adapter::getBall() {
	IceUtil::Mutex::Lock lock(_mutex);
	return _ball;
}

bool
Adapter::hasBlueRobot(int i) {
	IceUtil::Mutex::Lock lock(_mutex);
	return _detectionFrame.robots_blue_size()>i;
}

bool
Adapter::hasYellowRobot(int i) {
	IceUtil::Mutex::Lock lock(_mutex);
	return _detectionFrame.robots_yellow_size()>i;
}


SSL_DetectionRobot
Adapter::getBlueRobot(int i) {
	IceUtil::Mutex::Lock lock(_mutex);
	return _detectionFrame.robots_blue(i);
}

SSL_DetectionRobot
Adapter::getYellowRobot(int i) {
	IceUtil::Mutex::Lock lock(_mutex);
	return _detectionFrame.robots_yellow(i);
}

void
Adapter::setBall(SSL_DetectionBall ball) {
	IceUtil::Mutex::Lock lock(_mutex);
	_ball = ball;
}


