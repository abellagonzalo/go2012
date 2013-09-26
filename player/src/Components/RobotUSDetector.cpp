/*
 * Name: RobotUSDetector.cpp
 * @Author: Alejandro Aporta (a.aporta@alumnos.urjc.es)
 * @Author: Francisco Martín (fmartin@gsyc.es)
 
 * 
 * Description: Class to detect other robots
 *
 * Created on: 04/11/2011
 *
 * Copyright (C) Universidad Rey Juan Carlos
 * All Rights Reserved.
 *
 */

#include "RobotUSDetector.h"
 

RobotUSDetector::RobotUSDetector()
{
	body = Body::getInstance();
	ballDetector = BallDetector::getInstance();
	attention = Attention::getInstance();

}

RobotUSDetector::~RobotUSDetector()
{
}

void
RobotUSDetector::init(const string newName, AL::ALPtr<AL::ALBroker> parentBroker) {
	Component::init(newName, parentBroker);
	current_state = INITIAL_STATE;
	
	try
	{
		pmemory = getParentBroker()->getMemoryProxy();
	}catch( AL::ALError& e )
	{
		cerr<<"RobotUSDetector::init [pmemory]"<<e.toString()<<endl;
	}

	try
	{
		pmotion = getParentBroker()->getMotionProxy();
	}catch( AL::ALError& e )
	{
		cerr<<"RobotUSDetector::init [pmotion]"<<e.toString()<<endl;
	}
	
	setFreqTime(MEDIUM_RATE);
}

void
RobotUSDetector::dowork(void)
{
	MatrixCM RTdiff(4,4),RTUS1(4,4),us1rel(4,1);
	MatrixCM RTTorso(4,4);
	MatrixCM us1Robot(4,1), us2Robot(4,1);
	
	double degX =0.0;
	double degY =-0.17450;
	double degZ =-0.34900;

	double despX =53.7;
	double despY =34.1;
	double despZ =69.8;	
	
	torsoRT = this->pmotion->getTransform("Torso", 2, true);
	
	RTTorso.sete(0,0,torsoRT[0]);
	RTTorso.sete(0,1,torsoRT[1]);
	RTTorso.sete(0,2,torsoRT[2]);
	RTTorso.sete(0,3,torsoRT[3]);
	RTTorso.sete(1,0,torsoRT[4]);
	RTTorso.sete(1,1,torsoRT[5]);
	RTTorso.sete(1,2,torsoRT[6]);
	RTTorso.sete(1,3,torsoRT[7]);
	RTTorso.sete(2,0,torsoRT[8]);
	RTTorso.sete(2,1,torsoRT[9]);
	RTTorso.sete(2,2,torsoRT[10]);
	RTTorso.sete(2,3,torsoRT[11]);
	RTTorso.sete(3,0,torsoRT[12]);
	RTTorso.sete(3,1,torsoRT[13]);
	RTTorso.sete(3,2,torsoRT[14]);
	RTTorso.sete(3,3,torsoRT[15]);

	RTdiff.sete(0,0,cos(degY)*cos(degZ));
	RTdiff.sete(0,1,-cos(degX)*sin(degZ));
	RTdiff.sete(0,2,cos(degX)*sin(degY)*cos(degZ));
	RTdiff.sete(0,3,despX);                         /*Profundidad*/
	RTdiff.sete(1,0,cos(degY)*sin(degZ));
	RTdiff.sete(1,1,cos(degX)*cos(degZ));
	RTdiff.sete(1,2,cos(degX)*sin(degY)*cos(degZ));
	RTdiff.sete(1,3,despY);                                 /*Lateral*/
	RTdiff.sete(2,0,-sin(degY));
	RTdiff.sete(2,1,0.0);
	RTdiff.sete(2,2,cos(degX)*cos(degY));
	RTdiff.sete(2,3,despZ);                         /*Altura*/
	RTdiff.sete(3,0,0.0);
	RTdiff.sete(3,1,0.0);
	RTdiff.sete(3,2,0.0);
	RTdiff.sete(3,3,1.0);
	
	RTUS1 = RTTorso * RTdiff;
	
	/*Medida del sensor en relativas al sensor*/
	us1rel.sete(0,0, sonar_value_right * 1000.0);
	us1rel.sete(1,0,0.0);
	us1rel.sete(2,0,0.0);
	us1rel.sete(3,0,1.0);
	
	/*Sensor 1 en relativas al robot*/
	us1Robot = RTUS1 * us1rel;


	degZ =0.34900;

	RTdiff.sete(0,0,cos(degY)*cos(degZ));
	RTdiff.sete(0,1,-cos(degX)*sin(degZ));
	RTdiff.sete(0,2,cos(degX)*sin(degY)*cos(degZ));
	RTdiff.sete(1,0,cos(degY)*sin(degZ));
	RTdiff.sete(1,1,cos(degX)*cos(degZ));
	RTdiff.sete(1,2,cos(degX)*sin(degY)*cos(degZ));
	RTdiff.sete(2,3,despZ);                         /*Altura*/
	us1rel.sete(0,0, sonar_value_left*1000.0);

	RTUS1 = RTTorso * RTdiff;
	us2Robot = RTUS1 * us1rel;


	//cerr<<"["<<us2Robot.e(0,0)/us2Robot.e(3,0)<<","<<us2Robot.e(1,0)/us2Robot.e(3,0)<<"]\t["<<us1Robot.e(0,0)/us1Robot.e(3,0)<<","<<us1Robot.e(1,0)/us1Robot.e(3,0)<<"]"<<endl;
	left.x = us2Robot.e(0,0)/us2Robot.e(3,0);
	left.y = us2Robot.e(1,0)/us2Robot.e(3,0);
	right.x = us1Robot.e(0,0)/us1Robot.e(3,0);
	right.y = us1Robot.e(1,0)/us1Robot.e(3,0);
}

void
RobotUSDetector::getValues(HPoint2D &rleft, HPoint2D &rright)
{
	rleft = left;
	rright = right;
}


void
RobotUSDetector::initial_state_code(){
	
	attention->setPercentage("Ball", 1.0);
	
	printf("RobotUSDetector:initial_state_code\n");
}

void
RobotUSDetector::test_state_code(){
	
	bool isSeenBall = false;
	
	isSeenBall = ballDetector->isSeen();
	
	if(isSeenBall){
		printf("Ball detectado\n");
	}else
		printf("Ball no detectado.\n");
	
	ObjectState* balls = ballDetector->getObj();
	float rho = balls->getDistance();
	
	printf("Distancia del balon: %f\n",rho);
	
	float ball_x = ballDetector->ballmodel->estimate.getPositionInRelativeCoordinates().x;
	float ball_y = ballDetector->ballmodel->estimate.getPositionInRelativeCoordinates().y;
	
	printf("Coordenadas X Y de la bola: %f - %f\n",ball_x,ball_y);
} 

void
RobotUSDetector::step()
{
	
	

	if (!isTime2Run())
		return;

	//printf("RobotUSDetector AAG\n");
	
	/*
	switch(current_state){
		
		case INITIAL_STATE:
			initial_state_code();
			current_state = TEST_STATE;
		break;
		case TEST_STATE:
			ballDetector->step();
			
			test_state_code();
			
			attention->step();
		
		break;
	}
	*/
	try{
		
		
		sonar_value_right = (double)(pmemory->getData(string("Device/SubDeviceList/US/Right/Sensor/Value"), 1));
		sonar_value_left = (double)(pmemory->getData(string("Device/SubDeviceList/US/Left/Sensor/Value"), 1));

		dowork();
		
	} catch (ALError& e)
	{
		cerr << "RobotUSDetector::step(ALMemory->getData)" << e.toString() << endl;
	}
	
}




