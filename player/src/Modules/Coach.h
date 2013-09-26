/**
 * @author fmartin
 *
 * Version : $Id$
 * This file was generated by Aldebaran Robotics ModuleGenerator
 */

#ifndef Coach_H
#define Coach_H


#include "alcore/alptr.h"
#include "alproxies/alledsproxy.h"
#include "alproxies/almemoryproxy.h"
#include "alproxies/alsensorsproxy.h"
#include "alproxies/alsonarproxy.h"
#include "alproxies/alrobotposeproxy.h"
#include "alcommon/alproxy.h"
#include "alcommon/albroker.h"

#include "Component.h"
#include "Body.h"
#include "Perception.h"
#include "BallDetector.h"
#include "W3DDetector.h"
#include "LineDetector.h"
#include "GoalDetector.h"
#include "BGDetector.h"
#include "RobotDetector.h"
#include "Head.h"
#include "Fallen.h"
#include "FollowBall.h"
#include "SearchBall.h"
#include "Striker2.h"
#include "Leds.h"
#include "FSR.h"
#include "Speech.h"
#include "SearchNet.h"
#include "WorldModel.h"
#include "Kinematics.h"
#include "Calibration.h"
#include "pruebag2gpos.h"
#include "PruebaAttention.h"
#include "Go2gpos.h"
#include "Go2gposObs.h"
#include "Go2pos.h"
#include "Localization.h"
#include "BodyDetector.h"
#include "Kick.h"
#include "Attention.h"
#include "SharedTeamInfo.h"
#include "Stratego.h"
#include "MatchPlayer.h"
#include "Music.h"
#include "TextSpeech.h"
#include "Movie.h"
#include "LedsControl.h"
#include "FootContact.h"
#include "Debug.h"
#include "ApproachBall.h"
#include "StrategyRoles.h"
#include "GoalieURJC.h"
#include "PGoalie.h"
#include "AuxRole.h"
#include "AuxPenalty.h"
#include "WorldState.h"
#include "RobotUSDetector.h"
#include "RecorderSession.h"
#include "PlayerSession.h"

// #include "demencia/AleatoryMoveComponent.h"

#include "NaoServerCamera.h"
#include "NaoServerMotors.h"
#include "NaoServerEncoders.h"
#include "NaoServerPose3DMotors.h"
#include "NaoServerPose3DEncoders.h"
#include "SensorRecorderNao.h"

#include "RobotLog.h"

#include "CascadeScheduler.h"
#include "NEKFLocalization.h"
#include "EKFLocalization.h"

#include "GameController.h"

#include "Net/TcpServer.h"
#include "Net/UdpServer.h"

#include <pthread.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <cmath>

#include <IceE/IceE.h>
#include <image.h>
#include <localizationI.h>
#include <teamInfoI.h>
#include <componentsI.h>

using namespace std;
using namespace bica;

namespace AL
{
class ALBroker;
}

using namespace AL;


#define Coach_VERSION_MAJOR "0"
#define Coach_VERSION_MINOR "0"

class Coach : public AL::ALModule, public UdpServer
{

public:

	Coach(AL::ALPtr<AL::ALBroker> pBroker, const std::string& pName );
	virtual ~Coach();

	void dataChanged(const std::string& pDataName, const ALValue& pValue, const std::string& pMessage);
	bool innerTest();
	void UDPmanageRecv(int size);

private:

	void initButtons();

	list<string> getTokens(char *data);
	void setGT(list<string>);					//UDP
	static void* iceServerThread(void *obj);	//Ice

	Body	 	*body;
	Head 		*head;
	Perception	*perception;
	BallDetector *_BallDetector;
	W3DDetector *_W3DDetector;
	LineDetector *_LineDetector;
	GoalDetector *_GoalDetector;
	BGDetector *_BGDetector;
	RobotDetector *_RobotDetector;
	FollowBall	*followball;
	SearchBall	*searchball;
	Striker2		*striker2;
	Fallen		*fallen;
	Leds		*leds;
	FSR			*fsr;
	Speech		*speech;
	SearchNet	*searchnet;
	WorldModel	*worldmodel;
	CascadeScheduler *csched;
	GameController *gc;
	Kinematics 	*kinematics;
	Calibration *calibration;
	pruebag2gpos	 	*Pruebag2gpos;
	PruebaAttention	 	*pruebaAttention;
	Go2gpos	 	*go2gpos;
	Go2pos	 	*go2pos;
	Go2gposObs 	*go2gposobs;
	Localization	*localization;
	BodyDetector	 *bodydetector;
	Kick	 *kick;
	Attention * attention;
	SharedTeamInfo *sharedTeamInfo;
	Stratego	 *stratego;
	MatchPlayer	 *matchPlayer;
	Music 		*music;
	TextSpeech *textspeech;
	Movie *movie;
	LedsControl *ledscontrol;
	FootContact	*footcontact;
	Debug		*debug;
	ApproachBall		*approachBall;
	StrategyRoles *strategyRoles;
	GoalieURJC *goalieURJC;
	PGoalie *pgoalie;
	AuxRole *auxRole;
	AuxPenalty *auxPenalty;
	WorldState		*_WorldState;
	NaoServerCamera	*servercamera;
	NaoServerMotors	*servermotors;
	NaoServerEncoders	*serverencoders;
	NaoServerPose3DMotors	*serverpose3dmotors;
	NaoServerPose3DEncoders	*serverpose3dencoders;
	SensorRecorderNao	*sensorrecordernao;
	RobotUSDetector *robotusdetector;
	RobotLog	*robotLog;
	RecorderSession *recorderSession;
	PlayerSession *playerSession;
	AL::ALPtr<AL::ALProxy> psentinel;
	ALRobotPoseProxy *palrobotpose;
	ALSensorsProxy *psensors;
	ALSonarProxy *psonar;

	// AleatoryMoveComponent *aleatoryMoveComponent;

	pthread_t tIceServer;

	string name1, name2, name3, name4;
	string ip1, ip2, ip3, ip4;
	int icePort1, icePort2, icePort3, icePort4, myIcePort;

};
#endif // Coach_H
