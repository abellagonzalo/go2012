/*
 * Name: StrategyRoles.cpp
 * @Author: Carlos Agüero (caguero@gsyc.es)
 *  
 * Description: Class that evaluates parameters of all robots (distances to
 * ball, global localization, ...) and using utility functions, assign roles
 * to each robot. The roles are "Kicker", "Defender" and "Supporter". 
 * 
 * Copyright (C) Universidad Rey Juan Carlos
 * All Rights Reserved. 
 */

#include "StrategyRoles.h"

/**
 * Class constructor that initializes utility matrix.
 *
 * @param *wm Pointer to world model for using its constraints
 */
StrategyRoles::StrategyRoles()
{ 
	sharedTeamInfo = SharedTeamInfo::getInstance();
	gameController = GameController::getInstance();
	ballDetector   = BallDetector::getInstance();

	pthread_mutex_init(&mutex, NULL);

	for (int i = 0; i < NUM_ROBOTS; i++) {
		roles[i] = OFFLINE;
		for (int j = 0; j < NUM_ROLES; j++) {
			utility[i][j] = MAX_UTILITY_VALUE;
			sharedTeamInfo->teamData[i].setUtilityGoalie(MAX_UTILITY_VALUE);
			sharedTeamInfo->teamData[i].setUtilityKicker(MAX_UTILITY_VALUE);
			sharedTeamInfo->teamData[i].setUtilityDefender(MAX_UTILITY_VALUE);
			sharedTeamInfo->teamData[i].setUtilitySupporter(MAX_UTILITY_VALUE);
			sharedTeamInfo->teamData[i].setRole(roles[i]);
		}
	}

	setFreqTime(500);	// 2Hz.
}

/**
 * Class destructor.
 */
StrategyRoles::~StrategyRoles()
{
}


/**
 * Method that calculates the utility associated to a given robot and role.
 * Goalkeeper should not be passed as robotId. TimeStamps are used when messages
 * are received. Using those timeStamps, we can monitorize the status of the other
 * teammates and to know if they are running or not. Last information sent by a
 * robot is only used if its timeStamp is valid. For avoid quickly changes among
 * roles, previous roles of each robot are evaluated and the method penalizes
 * role changing.
 *
 * Heuristic
 * ---------
 *
 * Striker: Distance to the ball
 * Defender: Distance to its own net
 * Supporter: Distance to its opponent net
 *
 * @param *gs Pointer to Gs to accessing some data estructures
 * @param robotId Identifier of the robot (2,3,4)
 * @param role Identifier of the role (0,1,2)
 * @return the value of the utility evaluated
 */
double
StrategyRoles::calculateUtility(int robotId, int role)
{
	double utility = 0;
	long elapsed;

	elapsed = getCurrentTime() - (long)(sharedTeamInfo->getRobotInfo(robotId).getTimestamp());
	if ((elapsed > MAX_SILENCE_INTERVAL) || (gameController->isPenalized(robotId))){
		utility = MAX_UTILITY_VALUE;
	} else
	{
		switch (role)
		{
		case KICKER_ROLE:
			if (sharedTeamInfo->getRobotInfo(robotId).getBallTime() < 5000)
				utility = sharedTeamInfo->getRobotInfo(robotId).getBallDistance();
			else
				utility = MAX_UTILITY_VALUE - 1000;
			break;
		case DEFENDER_ROLE:
			utility = distance2net(robotId,OWN_NET);
			break;
		case SUPPORTER_ROLE:
			utility = distance2net(robotId,OPPONENT_NET);
			break;
		default:
			cout << "[StrategyRoles::calculateUtility()] Bad role identifier\n";
			break;
		}
		if (roles[robotId - 1] != role)
			utility += ROLE_CHANGE_COST;
	}
	return utility;
}


/**
 * Method used to recalculate all combinations among robots and roles. The utility
 * matrix is updated. Utility matrix for robot 1 (Goalkeeper) is not used
 *
 * @param *gs Pointer to Gs to accessing some data estructures
 */
void 
StrategyRoles::updateUtility()
{
	// Loop for robots with id=2,3,4
	for (int i = 1; i < NUM_ROBOTS; i++)
		for (int j = 0; j < NUM_ROLES; j++)
			utility[i][j] = calculateUtility(i + 1,j);
}


/**
 * Method that updates the "roles" structure. This structure contains the
 * information of what role is running each robot. In a first step the utility
 * matrix is updated. Next, roles are assigned according to the utility matrix
 * in a priorized way. First role choosen is "Kicker", then "Defender" and at
 * last "Supporter".
 *
 * @param *gs Pointer to Gs to accessing some data estructures
 */
void 
StrategyRoles::updateRoles()
{
	int kicker_robot=0, defender_robot=0, supporter_robot=0, i=0;
	double max_utility;
	int defenderUtility, supporterUtility;

	updateUtility();

	//updateBallVel();

	//Look for robot with max utility for KICKER ROLE
	max_utility = MAX_UTILITY_VALUE + 1;

	for (i = 1; i < NUM_ROBOTS; i++)
		if (utility[i][KICKER_ROLE] < max_utility)
		{
			max_utility = utility[i][KICKER_ROLE];
			kicker_robot = i;
		}

	//Look for robot with max utility for DEFENDER ROLE
	max_utility = MAX_UTILITY_VALUE + 1;

	for (i = 1; i < NUM_ROBOTS; i++)
		if ((utility[i][DEFENDER_ROLE] < max_utility) && (i != kicker_robot))
		{
			max_utility = utility[i][DEFENDER_ROLE];
			defender_robot = i;
		}
	defenderUtility = max_utility;

	//Look for robot with max utility for ATTACKER ROLE
	//Hack: Robot1 + Robot2 + Robot3 = 6
	supporter_robot = 6 - kicker_robot - defender_robot;
	supporterUtility = utility[supporter_robot][SUPPORTER_ROLE];

	//updating roles
	roles[kicker_robot] = KICKER_ROLE;
	if (defenderUtility < MAX_UTILITY_VALUE)
		roles[defender_robot] = DEFENDER_ROLE;
	else
		roles[defender_robot] = OFFLINE;

	if (supporterUtility < MAX_UTILITY_VALUE)
		roles[supporter_robot] = SUPPORTER_ROLE;
	else
		roles[supporter_robot] = OFFLINE;

	for (int i = 1; i < NUM_ROBOTS; i++){
		sharedTeamInfo->teamData[i].setUtilityKicker(utility[i][KICKER_ROLE]);
		sharedTeamInfo->teamData[i].setUtilityDefender(utility[i][DEFENDER_ROLE]);
		sharedTeamInfo->teamData[i].setUtilitySupporter(utility[i][SUPPORTER_ROLE]);
		sharedTeamInfo->teamData[i].setRole(getActualRole(i + 1));
	}
}


/**
 * Method that calculates the optimal position of an specified robot. This method
 * checks the role of this robot, and selects a position according to some
 * heuristic.
 *
 * @param robotID Identifier of the robot (2,3,4)
 * @param *x Coordinate of the optimal x position
 * @param *y Coordinate of the optimal y position
 * @param shared If 'true' the ball estimation will be the shared estimation,
 * if 'false' will be the local estimation
 */
void
StrategyRoles::getIdealPos(int robotID, int *x, int *y)
{
	switch (roles[robotID - 1])
	{
	case KICKER_ROLE:
		getGlobalBall(x,y);
		break;
	case DEFENDER_ROLE:
		getIdealDefensePos(x,y);
		break;
	case SUPPORTER_ROLE:
		getIdealSupporterPos(x,y);
		break;
	}
}

void
StrategyRoles::updateIdealPositions()
{
	updateKickerPos();
	updateDefenderPos();
	updateSupporterPos();
	updateBarrierPos();
	updateFreeKickerPos();
}

void
StrategyRoles::getIdealDefensePos(int *x, int *y)
{
	//pthread_mutex_lock(&mutex);

	*x = xDefender;
	*y = yDefender;

	//pthread_mutex_unlock(&mutex);
}

void
StrategyRoles::getIdealSupporterPos(int *x, int *y)
{
	//pthread_mutex_lock(&mutex);

	*x = xSupporter;
	*y = ySupporter;

	//pthread_mutex_unlock(&mutex);
}

void
StrategyRoles::getIdealFreeKickerPos(int *x, int *y)
{
	//pthread_mutex_lock(&mutex);

	*x = xFreeKicker;
	*y = yFreeKicker;

	//pthread_mutex_unlock(&mutex);
}

void
StrategyRoles::getIdealBarrierPos(int *x, int *y)
{
	*x = xBarrier;
	*y = yBarrier;
}


void
StrategyRoles::getGlobalBall(int *x, int *y)
{
	double myPosX,myPosY,myPosTh,ballXcart,ballYcart;

	ballXcart = ballDetector->ballmodel->estimate.getPositionInRelativeCoordinates().x;
	ballYcart = ballDetector->ballmodel->estimate.getPositionInRelativeCoordinates().y;

	myPosX = sharedTeamInfo->teamData[sharedTeamInfo->getMyId() - 1].getX();
	myPosY = sharedTeamInfo->teamData[sharedTeamInfo->getMyId() - 1].getY();
	myPosTh = sharedTeamInfo->teamData[sharedTeamInfo->getMyId() - 1].getTheta();

	//cerr << "Position (" << myPosX<<", "<<myPosY<<", "<<toDegrees(myPosTh)<<")"<<endl;

	*x = (int) (myPosX + ballXcart*cos(myPosTh) - ballYcart*sin(myPosTh));
	*y = (int) (myPosY + ballXcart*sin(myPosTh) + ballYcart*cos(myPosTh));
}

void
StrategyRoles::updateDefenderPos()
{
	double ownNetX = -3000, ownNetY = 0, ball2corner, alpha, dx, dy, areaCorner = 900;
	int ballX,ballY;

	//pthread_mutex_lock(&mutex);

	//Si la calidad de la pelota o de la auto-localización es mala -> devolver penalty
	/*if ((ballDetector->ballmodel->estimate.getReliability() < ObjectState::HIGH_RELIABLE) ||
			(sharedTeamInfo->teamData[sharedTeamInfo->getMyId() - 1].getPosReliability() == "Low")) {
		xDefender = -1800;
		yDefender = 0;

		//pthread_mutex_unlock(&mutex);
		return;
	}*/

	getGlobalBall(&ballX,&ballY);

	alpha = atan2(ballY - ownNetY, ballX - ownNetX);
	dx = cos(alpha) * areaCorner;
	dy = sin(alpha) * areaCorner;
	dx = ownNetX + dx;

	ball2corner = distance(dx,dy,ballX,ballY);

	dy = sin(alpha) * (areaCorner + ball2corner * BALL_DISTANCE_FACTOR);
	dx = ownNetX + cos(alpha) * (areaCorner + ball2corner * BALL_DISTANCE_FACTOR);

	xDefender = (int)dx;
	yDefender = (int)dy;

	// If it is outside of the field->penalty
	if ((abs(xDefender) > 3000) || (abs(yDefender) > 2000)) {
		xDefender = -1800;
		yDefender = 0;
	}

	//pthread_mutex_unlock(&mutex);
}

void
StrategyRoles::updateBarrierPos()
{
	double ownNetX = -3000, ownNetY = 0, ball2corner, alpha, dx, dy, areaCorner = 900;
	int ballX,ballY;
	int BARRIER_DISTANCE = 800;

	pthread_mutex_lock(&mutex);

	//Si la calidad de la pelota o de la auto-localización es mala -> devolver penalty
	/*if ((ballDetector->ballmodel->estimate.getReliability() < ObjectState::HIGH_RELIABLE) ||
			(sharedTeamInfo->teamData[sharedTeamInfo->getMyId() - 1].getPosReliability() == "Low")) {
		//ToDo: Stop the robot
		xBarrier = -1800;
		yBarrier = 0;

		pthread_mutex_unlock(&mutex);
		return;
	}*/

	getGlobalBall(&ballX,&ballY);

	alpha = atan2(ballY - ownNetY, ballX - ownNetX);
	dx = cos(alpha) * BARRIER_DISTANCE;
	dy = sin(alpha) * BARRIER_DISTANCE;

	xBarrier = ballX - dx;

	//if (ballY > 0) {
	yBarrier = ballY - dy;
	//} else {
	//	yBarrier = ballY + dy;
	//}

	pthread_mutex_unlock(&mutex);
}

void
StrategyRoles::updateFreeKickerPos()
{
	double ownNetX = 3000, ownNetY = 0, ball2corner, alpha, dx, dy, areaCorner = 900;
	int ballX,ballY;
	int BARRIER_DISTANCE = 250;

	pthread_mutex_lock(&mutex);

	//Si la calidad de la pelota o de la auto-localización es mala -> devolver penalty
	/*if ((ballDetector->ballmodel->estimate.getReliability() < ObjectState::HIGH_RELIABLE) ||
			(sharedTeamInfo->teamData[sharedTeamInfo->getMyId() - 1].getPosReliability() == "Low")) {
		//ToDo: Stop the robot
		xBarrier = -1800;
		yBarrier = 0;

		pthread_mutex_unlock(&mutex);
		return;
	}*/

	getGlobalBall(&ballX,&ballY);

	alpha = atan2(ballY - ownNetY, ballX - ownNetX);
	dx = cos(alpha) * BARRIER_DISTANCE;
	dy = sin(alpha) * BARRIER_DISTANCE;

	xFreeKicker = ballX - dx;

	//if (ballY > 0) {
	yFreeKicker = ballY + dy;
	//} else {
	//	yBarrier = ballY + dy;
	//}



	pthread_mutex_unlock(&mutex);
}

void
StrategyRoles::updateKickerPos()
{
	//pthread_mutex_lock(&mutex);
	getGlobalBall(&xKicker, &yKicker);
	//pthread_mutex_unlock(&mutex);
}


void
StrategyRoles::updateSupporterPos()
{
	double distance2lateral, dx, dy, distance2GoalLine;
	int ballX,ballY;

	//pthread_mutex_lock(&mutex);

	//Si la calidad de la pelota o de la auto-localización es mala -> devolver penalty
	/*if ((ballDetector->ballmodel->estimate.getReliability() < ObjectState::HIGH_RELIABLE) ||
			(sharedTeamInfo->teamData[sharedTeamInfo->getMyId() - 1].getPosReliability() == "Low")) {
		xSupporter = 1800;
		ySupporter = 0;

		//pthread_mutex_unlock(&mutex);
		return;
	}*/

	getGlobalBall(&ballX,&ballY);

	if (ballY < 0)
	{
		distance2lateral = 2000 - ballY;
		dy = ballY + (distance2lateral / 2);
	}else{
		distance2lateral = 2000 + ballY;
		dy = ballY - (distance2lateral / 2);
	}

	distance2GoalLine = 3000 - ballX ;
	dx = ballX + (distance2GoalLine / 2);

	if (ballX > distance2GoalLine)
		dx = ballX * 0.5;

	xSupporter = (int)dx;
	ySupporter = (int)dy;

	// If it is outside of the field->penalty
	if ((abs(xSupporter) > 3000) || (abs(ySupporter) > 2000)) {
		xSupporter = 1800;
		ySupporter = 0;
	}

	//pthread_mutex_unlock(&mutex);
}

/*
 * Method that calculates the distance between two points (x1,y1) -> (x2,y2)
 *
 * @param x1 "X" value of the first point
 * @param x2 "X" value of the second point
 * @param y1 "Y" value of the first point
 * @param y2 "Y" value of the second point
 * @return the double value corresponding to the distance
 */
double
StrategyRoles::distance(double x1, double y1, double x2, double y2)
{
	return sqrt(pow(x1 - x2,2) + pow(y1 - y2,2));
}



/**
 * Method that calculates the distance between a robot and a net (the net and
 * robot are selected in parameters)
 *
 * @param *gs Pointer to Gs to accessing some data estructures
 * @param robotId Identifier of the robot (2,3,4)
 * @param net Own or opponent net
 * @return the double value corresponding to the distance between the selected
 * robot and the specified net.
 */
double
StrategyRoles::distance2net(int robotId, int net)
{
	double x;

	if (net == OWN_NET)
		//y = world->getFieldYSize() * -0.5; //DOESN'T WORK �?
		x = -3000;
	else
		//y = world->getFieldYSize() * 0.5; //DOESN'T WORK �?
		x = 3000;

	//		if (gs->getMate(robot - 1)->anchored < 0.40)
	//		{
	//			//printf("Mala localizaci�n calculando distancia a porteria\n");
	////			if (robot == 2)
	////			{
	////				//printf("Porteria propia: (0,%d)\n",world->getFieldYSize() * -0.5);
	////				//printf("distancia a porteria propia: %d\n",10000);
	////			}
	//			return 10000;
	//		}
	//		else
	//		{
	//printf("Buena localizaci�n calculando distancia a porteria\n");
	//			if (robot == 2)
	//			{
	//				//printf("Porteria propia: (0,%d)\n",world->getFieldYSize() * -0.5);
	//				printf("distancia a porteria propia: %f\n",distance(0,y,gs->getMate(robot - 1)->x,gs->getMate(robot - 1)->y));
	//			}
	return distance(x,0, sharedTeamInfo->getRobotInfo(robotId).getX() ,
			sharedTeamInfo->getRobotInfo(robotId).getY());
	//}
}


/**
 * Method that returns the role identifier running in a given player.
 *
 * @param *playerId Identifier of the robot (2,3,4)
 * @return the role of the specified player
 */
int 
StrategyRoles::getActualRole(int playerId)  
{	
	return roles[playerId - 1]; 
}

void
StrategyRoles::step()
{
	ballDetector->step();
	sharedTeamInfo->step();

	if (isTime2Run()) {
		updateRoles();
		updateIdealPositions();
	}
}

bica::ShapeList
StrategyRoles::getGrDebugAbs()
{
	shapeListAbs.clear();

	//pthread_mutex_lock(&mutex);

	// Kicker
	bica::Point3DPtr p2(new bica::Point3D);
	bica::CirclePtr circle2(new bica::Circle);
	p2->x = xKicker;
	p2->y = yKicker;
	p2->z = 0.0f;

	circle2->center = p2;
	circle2->radius = 50.0f;
	circle2->z = 0.0f;
	circle2->color = bica::RED;
	circle2->filled = true;
	circle2->opacity = 125;
	circle2->accKey = "k";
	circle2->label = "Kicker";
	shapeListAbs.push_back(circle2);

	// Defender
	bica::Point3DPtr p(new bica::Point3D);
	bica::CirclePtr circle(new bica::Circle);
	p->x = xDefender;
	p->y = yDefender;
	p->z = 0.0f;

	circle->center = p;
	circle->radius = 50.0f;
	circle->z = 0.0f;
	circle->color = bica::BLUE;
	circle->filled = true;
	circle->opacity = 125;
	circle->accKey = "d";
	circle->label = "Defender";
	shapeListAbs.push_back(circle);

	// Supporter
	bica::Point3DPtr p3(new bica::Point3D);
	bica::CirclePtr circle3(new bica::Circle);
	p3->x = xSupporter;
	p3->y = ySupporter;
	p3->z = 0.0f;

	circle3->center = p3;
	circle3->radius = 50.0f;
	circle3->z = 0.0f;
	circle3->color = bica::YELLOW;
	circle3->filled = true;
	circle3->opacity = 255;
	circle3->accKey = "d";
	circle3->label = "Supporter";
	shapeListAbs.push_back(circle3);

	// Barrier
	bica::Point3DPtr p4(new bica::Point3D);
	bica::CirclePtr circle4(new bica::Circle);
	p4->x = xBarrier;
	p4->y = yBarrier;
	p4->z = 0.0f;

	circle4->center = p4;
	circle4->radius = 50.0f;
	circle4->z = 0.0f;
	circle4->color = bica::RED;
	circle4->filled = true;
	circle4->opacity = 255;
	circle4->accKey = "d";
	circle4->label = "Barrier";
	shapeListAbs.push_back(circle4);

	// Free Kicker
	bica::Point3DPtr p5(new bica::Point3D);
	bica::CirclePtr circle5(new bica::Circle);
	p5->x = xFreeKicker;
	p5->y = yFreeKicker;
	p5->z = 0.0f;

	circle5->center = p5;
	circle5->radius = 50.0f;
	circle5->z = 0.0f;
	circle5->color = bica::BLACK;
	circle5->filled = true;
	circle5->opacity = 255;
	circle5->accKey = "d";
	circle5->label = "FreeKicker";
	shapeListAbs.push_back(circle5);

	//pthread_mutex_unlock(&mutex);

	return shapeListAbs;
}
