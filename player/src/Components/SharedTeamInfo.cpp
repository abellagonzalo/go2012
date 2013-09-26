/*
 * SharedTeamInfo.cpp
 *
 *  Created on: 06/03/2010
 *      Author: Carlos Agüero
 */

#include "SharedTeamInfo.h"

SharedTeamInfo::SharedTeamInfo()
{

	pthread_mutex_init(&mutex, NULL);

	// Set teammate IPs
	std::ifstream comms("/home/nao/bica/conf/comms.cfg");
	string name1, name2, name3, name4;

	if (comms) {
		comms >> name1 >> ip1;
		comms >> name2 >> ip2;
		comms >> name3 >> ip3;
		comms >> name4 >> ip4;
	}else
		cerr << begin_red << "[SharedTeamInfo::SharedTeamInfo()] Config file (commns.cfg) not found in"
		" /home/nao/bica/conf/ directory\n" << end_color;

	for (int i = 0; i < NUM_ROBOTS; i++){
		teamData[i].setFrom(me);
		teamData[i].setTo(i + 1);
		teamData[i].setTeamId(teamNumber);
		teamData[i].setTimestamp(getCurrentTime());
	}

	char* argvv[] = {"player", 0};
	int argcc = 1;

	ic = Ice::initialize(argcc, argvv);

	// Teammate proxys
	for (int i = 0; i < NUM_ROBOTS; i++)
		teammatesInfoPrx[i] = NULL;

	setFreqTime(500);	// 2Hz.
}

void
SharedTeamInfo::init(const string newName, AL::ALPtr<AL::ALBroker> parentBroker)
{
	worldState		= WorldState::getInstance();
	ballDetector 	= BallDetector::getInstance();

}
string
SharedTeamInfo::getTeamColor(const Ice::Current& c)
{
	return getTeamColor();
}

SharedTeamInfo::~SharedTeamInfo() {
	cerr << "SharedTeamInfo::Destructor()\n";
}

void
SharedTeamInfo::setMyId(int playerNumber)
{
	//cout << "SharedTeamInfo::setMyId()\n";
	//IceUtil::RecMutex::Lock lock(_mutex);
	pthread_mutex_lock(&mutex);

	this->me = playerNumber;
	for (int i = 0; i < NUM_ROBOTS; i++){
		teamData[i].setFrom(me);
	}
	pthread_mutex_unlock(&mutex);
};

void
SharedTeamInfo::updateTeamInfo()
{
	//IceUtil::RecMutex::Lock lock(_mutex);
	pthread_mutex_lock(&mutex);

	Pose2D pos;
	float posReliability;

	// Update my own info

	pos = worldState->getPosition();
	//posX = posY = posTheta = 0;
	posReliability = worldState->getReliability();

	teamData[me - 1].setX(pos.translation.x);
	teamData[me - 1].setY(pos.translation.y);
	teamData[me - 1].setTheta(float(pos.rotation));

	if (posReliability < 0.5)
		teamData[me - 1].setPosReliability("Low");
	else if (posReliability < 0.8)
		teamData[me - 1].setPosReliability("Medium");
	else
		teamData[me - 1].setPosReliability("High");

	//ballDetector->step();
	teamData[me - 1].setBallDistance(
			ballDetector->ballmodel->estimate.getDistance());
	teamData[me - 1].setBallX(
			ballDetector->ballmodel->estimate.getPositionInRelativeCoordinates().x);
	teamData[me - 1].setBallY(
			ballDetector->ballmodel->estimate.getPositionInRelativeCoordinates().y);
	teamData[me - 1].setBallTime(
			ballDetector->ballmodel->elapsedTimeSinceLastObs);
	teamData[me - 1].setBallReliability(
			ObjectState::reliability2string(ballDetector->ballmodel->estimate.getReliability()));
	teamData[me - 1].setTimestamp(getCurrentTime());

	// Send info to the rest of teammates (Goalie not included)
	SwitchOpinionPtr myOpinion(new SwitchOpinion);
	myOpinion->observerRobot = teamData[me - 1].getFrom();
	myOpinion->observedRobot = teamData[me - 1].getTo();
	myOpinion->x = teamData[me - 1].getX();
	myOpinion->y = teamData[me - 1].getY();
	myOpinion->theta = teamData[me - 1].getTheta();
	myOpinion->posReliability = teamData[me - 1].getPosReliability();
	myOpinion->ballDistance = teamData[me - 1].getBallDistance();
	myOpinion->ballX = teamData[me - 1].getBallX();
	myOpinion->ballY = teamData[me - 1].getBallY();
	myOpinion->ballTimeSinceLastSeen = teamData[me - 1].getBallTime();
	myOpinion->ballReliability = teamData[me - 1].getBallReliability();
	myOpinion->utilityGoalie = teamData[me - 1].getUtilityGoalie();
	myOpinion->utilityKicker = teamData[me - 1].getUtilityKicker();
	myOpinion->utilityDefender = teamData[me - 1].getUtilityDefender();
	myOpinion->utilitySupporter = teamData[me - 1].getUtilitySupporter();
	myOpinion->role = teamData[me - 1].getRole();

	pthread_mutex_unlock(&mutex);

	for (int i = 0; i < NUM_ROBOTS; i++) {
		try {
			if (teammatesInfoPrx[i] != NULL) {
				//cout << "Sending new opinion to robot " << i + 1 << endl;
				teammatesInfoPrx[i]->putSwitchInfo(myOpinion);
			}
		} catch (const Ice::Exception& ex) {
			cerr << ex << endl;
			teammatesInfoPrx[i] = NULL;
			cout << "Teammate unconnected (" << i + 1 << ")\n";
			showTeammatesConnected();
		}
	}
}


void
SharedTeamInfo::step()
{
	if (isTime2Run()) {
		updateTeamInfo();
		//showTeammatesConnected();
	}
}


OpinionsList
SharedTeamInfo::getOpinionsList(const Ice::Current& c)
{
	pthread_mutex_lock(&mutex);

	OpinionsList opinions;
	opinions.clear();

	for (int i = 0; i < NUM_ROBOTS; i++) {
		SwitchOpinionPtr myOpinion(new SwitchOpinion);
		myOpinion->observerRobot = me;
		myOpinion->observedRobot = i + 1;
		myOpinion->x = teamData[i].getX();
		myOpinion->y = teamData[i].getY();
		myOpinion->theta = teamData[i].getTheta();
		myOpinion->posReliability = teamData[i].getPosReliability();
		myOpinion->ballDistance = teamData[i].getBallDistance();
		myOpinion->ballX = teamData[i].getBallX();
		myOpinion->ballY = teamData[i].getBallY();
		myOpinion->ballTimeSinceLastSeen = teamData[i].getBallTime();
		myOpinion->ballReliability = teamData[i].getBallReliability();
		myOpinion->utilityGoalie = teamData[i].getUtilityGoalie();
		myOpinion->utilityKicker = teamData[i].getUtilityKicker();
		myOpinion->utilityDefender = teamData[i].getUtilityDefender();
		myOpinion->utilitySupporter = teamData[i].getUtilitySupporter();
		myOpinion->role = teamData[i].getRole();

		opinions.push_back(myOpinion);
	}

	pthread_mutex_unlock(&mutex);

	return opinions;
}

void
SharedTeamInfo::startTeammateInfoPrx()
{
	string teammateIP;
	int teammatePort = 10000;

	for (int i = 0; i < NUM_ROBOTS; i++) {
		if (me != i + 1) {	// me = 2, 3, 4
			switch (i) {
			case 0:
				teammateIP = ip1;
				break;
			case 1:
				teammateIP = ip2;
				break;
			case 2:
				teammateIP = ip3;
				break;
			case 3:
				teammateIP = ip4;
				break;
			default:
				cerr << "SharedTeamInfo::startTeammateInfoPrx() error, inccorrect i value (" << i << ")\n";
			}
			teammatePort = 10000 + i + 1; // 10001, 10002, 10003, 10004

			// Create the proxy
			try {
				cout << "Creando proxy para " << teammateIP << ":" << teammatePort << endl;
				std::stringstream endpoint;
				endpoint << "TeamInfoProvider:default -h " << teammateIP << " -p " << teammatePort;
				Ice::ObjectPrx base = ic->stringToProxy(endpoint.str());
				base = base->ice_timeout(3000);

				// Get a oneway proxy.
				Ice::ObjectPrx oneway;
				try {
					oneway = base->ice_oneway();
				} catch (const Ice::NoEndpointException&) {
					cerr << "No endpoint for oneway invocations" << endl;
				}

				teammatesInfoPrx[i] = TeamInfoProviderPrx::uncheckedCast(oneway);
				if (!teammatesInfoPrx[i])
					throw "Invalid proxy";
				else {
					teammatesInfoPrx[i]->hello(me);	// Send hello to a teammate
				}
			} catch (const Ice::Exception& ex) {
				teammatesInfoPrx[i] = NULL;
				cerr << ex << endl;
				//status = 1;
			} catch (const char* msg) {
				cerr << msg << endl;
				//status = 1;
			}
		}
	}

	showTeammatesConnected();
}


void
SharedTeamInfo::putSwitchInfo(const SwitchOpinionPtr& opinion, const Ice::Current& c)
{
	pthread_mutex_lock(&mutex);

	if ((opinion->observerRobot >= 1) && (opinion->observerRobot <= NUM_ROBOTS)) {
		teamData[opinion->observerRobot - 1].setX(opinion->x);
		teamData[opinion->observerRobot - 1].setY(opinion->y);
		teamData[opinion->observerRobot - 1].setTheta(opinion->theta);
		teamData[opinion->observerRobot - 1].setPosReliability(opinion->posReliability);
		teamData[opinion->observerRobot - 1].setBallDistance(opinion->ballDistance);
		teamData[opinion->observerRobot - 1].setBallX(opinion->ballX);
		teamData[opinion->observerRobot - 1].setBallY(opinion->ballY);
		teamData[opinion->observerRobot - 1].setBallTime(opinion->ballTimeSinceLastSeen);
		teamData[opinion->observerRobot - 1].setBallReliability(opinion->ballReliability);
		teamData[opinion->observerRobot - 1].setTimestamp(getCurrentTime());
	} else {
		cerr << "SharedTeamInfo::ice_response() Incorrect observerRobot ID received (" <<
				opinion->observerRobot << "\n";
	}

	pthread_mutex_unlock(&mutex);
}

void
SharedTeamInfo::hello(int robotId, const Ice::Current& c)
{
	string teammateIP;
	int teammatePort = 10000;
	std::stringstream endpoint;

	if (me == robotId) {
		cerr << "SharedTeamInfo::hello() Incorrect robotId received (it is my own ID): " << robotId << endl;
	} else {
		//cout << "Creating proxy for robot " << robotId << endl;
		switch (robotId) {
		case 1:
			teammateIP = ip1;
			break;
		case 2:
			teammateIP = ip2;
			break;
		case 3:
			teammateIP = ip3;
			break;
		case 4:
			teammateIP = ip4;
			break;
		default:
			cerr << "SharedTeamInfo::hello() error, inccorrect robotId value (" << robotId << ")\n";
		}
		teammatePort = 10000 + robotId; // 10002, 10003, 10004

		// Create the proxy
		try {
			//cout << "Creando proxy para " << teammateIP << ":" << teammatePort << endl;
			endpoint << "TeamInfoProvider:default -h " << teammateIP << " -p " << teammatePort;
			Ice::ObjectPrx base = ic->stringToProxy(endpoint.str());
			base = base->ice_timeout(5000);

			// Get a oneway proxy.
			Ice::ObjectPrx oneway;
			try {
				oneway = base->ice_oneway();
			} catch (const Ice::NoEndpointException&) {
				cerr << "No endpoint for oneway invocations" << endl;
			}

			teammatesInfoPrx[robotId - 1] = TeamInfoProviderPrx::uncheckedCast(oneway);
			if (!teammatesInfoPrx[robotId - 1])
				throw "Invalid proxy";
		} catch (const Ice::Exception& ex) {
			//cerr << ex << endl;
			//status = 1;
		} catch (const char* msg) {
			//cerr << msg << endl;
			//status = 1;
		}
	}

	cout << "New teammate connected (" << (robotId) << ")\n";
	showTeammatesConnected();
}

void
SharedTeamInfo::showTeammatesConnected()
{
	// Show teammates connected
	cout << "Teammates detected: [ ";
	for (int i = 0; i < NUM_ROBOTS; i++) {
		if ((me != i + 1) && (teammatesInfoPrx[i] != NULL)){ // me = 2, 3, 4
			cout << i + 1 << " ";
		}
	}
	cout << "]\n";
}
