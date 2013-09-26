/*
 * SharedTeamInfo.h
 *
 *  Created on: 06/03/2010
 *      Author: Carlos Agüero
 */

#ifndef SHAREDTEAMINFO_H_
#define SHAREDTEAMINFO_H_

#include "Component.h"
#include "Singleton.h"
#include "../lib/estimation/RobotPose.h"
#include "../lib/estimation/ObjectState.h"
#include "WorldState.h"
#include "BallDetector.h"
#include <fstream>

#include <IceE/IceE.h>
#include <teamInfoI.h>

using namespace bica;

class WorldState;

class RobotInformation
{
protected:
	int 		from;
	int 		to;
	int			x;
	int			y;
	float		theta;
	string		posReliability;
	float		ballDistance;
	float		ballX;
	float		ballY;
	long		ballTime;
	string		ballReliability;
	float		utilityGoalie;
	float		utilityKicker;
	float		utilityDefender;
	float		utilitySupporter;
	unsigned	timestamp;
	short		teamId;
	short		role;

public:
	RobotInformation():
		timestamp(0) { }

	inline void setFrom (int from) { this->from = from; };
	inline void setTo (int to) { this->to = to; };
	inline void setX (int x) { this->x = x; };
	inline void setY (int y) { this->y = y; };
	inline void setTheta (float theta) { this->theta = theta; };
	inline void setPosReliability (string posReliability) { this->posReliability = posReliability; };
	inline void setBallDistance (float ballDistance) { this->ballDistance = ballDistance; };
	inline void setBallX (float ballX) { this->ballX = ballX; };
	inline void setBallY (float ballY) { this->ballY = ballY; };
	inline void setBallTime (long ballTime) { this->ballTime = ballTime; };
	inline void setBallReliability (string ballReliability) { this->ballReliability = ballReliability; };
	inline void setUtilityGoalie (float utilityGoalie) { this->utilityGoalie = utilityGoalie; };
	inline void setUtilityKicker (float utilityKicker) { this->utilityKicker = utilityKicker; };
	inline void setUtilityDefender (float utilityDefender) { this->utilityDefender = utilityDefender; };
	inline void setUtilitySupporter(float utilitySupporter) { this->utilitySupporter = utilitySupporter; };
	inline void setTimestamp (const unsigned newTimestamp) { timestamp = newTimestamp; };
	inline void setTeamId (short newTeamId) { teamId = newTeamId; };
	inline void setRole (short newRole) { role = newRole; };

	inline int getFrom() { return from; };
	inline int getTo() { return to; };
	inline int getX() { return x; };
	inline int getY() { return y; };
	inline float getTheta() { return theta; };
	inline string getPosReliability() { return posReliability; };
	inline float getBallDistance() { return ballDistance; };
	inline float getBallX() { return ballX; };
	inline float getBallY() { return ballY; };
	inline long getBallTime() { return ballTime; };
	inline string getBallReliability() { return ballReliability; };
	inline float getUtilityGoalie() { return utilityGoalie; };
	inline float getUtilityKicker() { return utilityKicker; };
	inline float getUtilityDefender() { return utilityDefender; };
	inline float getUtilitySupporter() { return utilitySupporter; };
	inline unsigned getTimestamp() { return timestamp; };
	inline short getTeamId () { return teamId; };
	inline short getRole () { return role; };

	const RobotInformation& operator=(const RobotInformation& other)
	{
		from		= other.from;
		to			= other.to;
		x			= other.x;
		y			= other.y;
		theta		= other.theta;
		posReliability	= other.posReliability;
		ballDistance	= other.ballDistance;
		ballX		= other.ballX;
		ballY		= other.ballY;
		ballTime		= other.ballTime;
		ballReliability	= other.ballReliability;
		utilityGoalie	= other.utilityGoalie;
		utilityKicker	= other.utilityKicker;
		utilityDefender	= other.utilityDefender;
		utilitySupporter	= other.utilitySupporter;
		timestamp	= other.timestamp;
		teamId		= other.teamId;
		role		= other.role;
		return *this;
	}
};


class SharedTeamInfo : public Component, public Singleton<SharedTeamInfo>, public TeamInfoProvider {
//public bica::AMI_TeamInfoProvider_getSwitchInfo {
public:
	SharedTeamInfo ();
	virtual ~SharedTeamInfo();

	void init(const string newName, AL::ALPtr<AL::ALBroker> parentBroker);

	void step();

	inline RobotInformation getRobotInfo(const short robotId) { return teamData[robotId - 1]; };
	inline int getMyId() { return me; };
	void setMyId(int playerNumber);
	inline void setTeamNumber(int teamNumber) { this->teamNumber = teamNumber; };
	void setTeamColor(string newColorTeam) { colorTeam = newColorTeam; };
	string getTeamColor() { return colorTeam; };
	inline Ice::CommunicatorPtr getIceCommunicator () { return ic; };
	void startTeammateInfoPrx();

	virtual string getTeamColor(const Ice::Current& c);
	virtual void putSwitchInfo(const SwitchOpinionPtr& opinion, const Ice::Current& c);
	virtual	OpinionsList getOpinionsList(const Ice::Current& c);
	virtual void hello(int robotId, const Ice::Current& c);

	static const short NUM_ROBOTS	=	4;
	RobotInformation	teamData[NUM_ROBOTS];
	pthread_mutex_t mutex;

private:
	void updateTeamInfo();
	void showTeammatesConnected();

	Ice::CommunicatorPtr ic;
	WorldState			*worldState;
	BallDetector		*ballDetector;
	short 				teamNumber,	me;
	string 				colorTeam, ip1, ip2, ip3, ip4;
	TeamInfoProviderPrx teammatesInfoPrx[NUM_ROBOTS];
	//IceUtil::RecMutex _mutex; // Recursive mutex
};

#endif /* SHAREDTEAMINFO_H_ */
