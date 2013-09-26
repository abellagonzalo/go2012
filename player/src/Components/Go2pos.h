#ifndef Go2pos_H
#define Go2pos_H

#include "Component.h"
#include "Body.h"
#include "Singleton.h"
#include "Kinematics.h"
#include "PIDController.h"
#include "RobotUSDetector.h"

//BUILDER COMMENT. DO NOT REMOVE. auxinclude begin
#include <IceE/IceE.h>
#include <componentsI.h>

using namespace bica;
//BUILDER COMMENT. DO NOT REMOVE. auxinclude end

typedef struct
{
	double x;
	double y;
}tVectorObs;

class Go2pos : public Component, public Singleton<Go2pos>, public Go2posManager
{
public:

	Go2pos();
	~Go2pos();

	void step();

private:
	Body *_Body;
	float goalx, goaly, goalt;

//BUILDER COMMENT. DO NOT REMOVE. auxcode begin
public:
	void init(const string newName, AL::ALPtr<AL::ALBroker> parentBroker);
	void setGoalPosXY(double x, double y, double t, float f=1.0);
	virtual void setGoalPos( double x, double y, const Ice::Current& c );
//BUILDER COMMENT. DO NOT REMOVE. auxcode end
private:
	void getVector(float &vx, float &vy);

	float factor;
	PIDController *ctrlW, *ctrlL, *ctrlV;
	RobotUSDetector *_RobotUSDetector;

};

#endif

