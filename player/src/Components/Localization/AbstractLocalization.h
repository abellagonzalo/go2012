#ifndef ABSTRACTLOCALIZATION_H
#define ABSTRACTLOCALIZATION_H

#include "Component.h"
#include "image.h"
#include "GameController.h"

typedef struct
{
	int id;
	double x,y,t;
	float prob;
}tRobotHipothesis;

typedef struct
{
	float reliability;
	float x;
	float y;
}PostInfo;

typedef struct
{
	long p0ElapsedTimeSinceLastObs;
	long p1ElapsedTimeSinceLastObs;
	PostInfo p0r;
	PostInfo p0l;
	PostInfo p1r;
	PostInfo p1l;
}PerceptionInfo;

typedef struct
{
	float movx;
	float movy;
	float movt;
}MotionInfo;



class AbstractLocalization : public Component
{
public:

	AbstractLocalization();
	~AbstractLocalization();

	virtual void init(const string newName, AL::ALPtr<AL::ALBroker> parentBroker) = 0;

	virtual void step() = 0;

	/*Get the reliability of the current localization*/
	virtual float getReliability() = 0;

	/*Reset the localization algorithm depending on the gamestate*/
	virtual void resetToPosition(int state) = 0;

	/*Reset the localization algorithm*/
	void reset();

	/*Get player color*/
	int getPlayerColor();

	/*Get the position of the robot calculated*/
	virtual void getPosition(double &xpos, double &ypos, double &thetapos) = 0;

	/*Get the position of the robot calculated*/
	virtual void getPosition(vector<tRobotHipothesis> &hipotheses) = 0;

	/*Get info about the current localization*/
	virtual string getInfo() = 0;

	/*Get an image to debug the localization*/
	//virtual ALValue getImgDebug() = 0;
	virtual bica::ImageDataPtr getImgDebug() = 0;

	//set the input
	virtual void setInfo(MotionInfo &motion, PerceptionInfo &perception) = 0;

	//Do all work
	virtual void run() = 0;

	static const int	reset2Initial 		= 	0;
	static const int	reset2Ready 		= 	1;
	static const int	reset2Set 			= 	2;
	static const int	reset2Playing 		= 	3;
	static const int	resetFromPenalised 	= 	4;
	static const int	reset2Penalty 		= 	5;

protected:

	int	resetLocalization;
	GameController * _GameController;
};

#endif
