/*
 * Name: StrategyRoles.h
 * @Author: Carlos Agüero (caguero@gsyc.es)
 *  
 * Description: Class that evaluates parameters of all robots (distances to
 * ball, global localization, ...) and using utility functions, assign roles
 * to each robot. The roles are "Kicker", "Defender" and "Supporter". 
 * 
 * Copyright (C) Universidad Rey Juan Carlos
 * All Rights Reserved. 
 */


#ifndef _STRATEGYROLES_H_
#define _STRATEGYROLES_H_

#include "SharedTeamInfo.h"
#include "Component.h"
#include "Singleton.h"
#include "GameController.h"
#include "Debug.h"
#include <debug.h>


// roles and parameters
#define KICKER_ROLE             		0
#define DEFENDER_ROLE   				1
#define SUPPORTER_ROLE   				2
#define OFFLINE							99
#define PENALIZED						100
#define BALL_DISTANCE_FACTOR 			0.3
#define OWN_NET_DISTANCE_FACTOR 		0.8
#define OPPONENT_NET_DISTANCE_FACTOR 	0.8
#define ROLE_CHANGE_COST 				400

// other constants
#define OWN_NET 						1
#define OPPONENT_NET    				2
#define NUM_ROBOTS  					4
#define NUM_ROLES   					3
#define DOG_WIDTH       				250 //(dog width in mm)
#define BALL_WIDTH      				80
#define MAX_UTILITY_VALUE 				99999
//#define OFFLINE_UTILITY_VALUE			99999
//#define PENALIZED_UTILITY_VALUE		88888
#define DEBUG_FREQ 						10
#define MAX_SILENCE_INTERVAL			3000000	// 3 segs

class StrategyRoles : public Component, public Singleton<StrategyRoles>, public DebugIAbs
{
public:
  	StrategyRoles();
	~StrategyRoles();
	
	void updateRoles();
	int getActualRole(int playerId);
	inline int getCurrentRole() { return roles[sharedTeamInfo->getMyId() - 1]; };
	bica::ShapeList getGrDebugAbs();
	void step();
	void getIdealPos(int robotID, int *x, int *y);
	void getGlobalBall(int *x, int *y);
	void getIdealDefensePos(int *x, int *y);
	void getIdealSupporterPos(int *x, int *y);
	void getIdealFreeKickerPos(int *x, int *y);
	void getIdealBarrierPos(int *x, int *y);

private:
	double calculateUtility(int robotId, int role);
	void updateUtility();
	double distance2net(int robot, int net);
	double distance(double x1, double y1, double x2, double y2);
	void printUtilities();
	void updateIdealPositions();
	void updateKickerPos();
	void updateDefenderPos();
	void updateSupporterPos();
	void updateBarrierPos();
	void updateFreeKickerPos();
			
 	int roles[NUM_ROBOTS];
    double utility[NUM_ROBOTS][NUM_ROLES];		//Utility matrix */
    
    SharedTeamInfo	*sharedTeamInfo;
    GameController  *gameController;
    BallDetector	*ballDetector;

	pthread_mutex_t mutex;

    int xKicker, yKicker, xDefender, yDefender, xSupporter, ySupporter;
    int xBarrier, yBarrier, xFreeKicker, yFreeKicker;
};

#endif //_STRATEGYROLES_H_
