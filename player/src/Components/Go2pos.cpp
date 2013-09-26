#include "Go2pos.h"

Go2pos::Go2pos()
{
	_Body = Body::getInstance();
	_RobotUSDetector = RobotUSDetector::getInstance();
	factor = 1.0;

	ctrlW = new PIDController("W", 0.001, 0.5, 0.0, 1.0);
	ctrlW->setPIDKs(0.7, 0.15, 0.001);
	ctrlL = new PIDController("L", 0.001, 0.8, 0.0, 1.0);
	ctrlL->setPIDKs(1.0, 0.1, 0.1);
	ctrlV = new PIDController("V", 0.001, 0.8, 0.0, 1.0);
	ctrlV->setPIDKs(1.0, 0.1, 0.1);

}

Go2pos::~Go2pos()
{

}

void
Go2pos::getVector(float &vx, float &vy)
{
	HPoint2D rl, rr;
	_RobotUSDetector->getValues(rl, rr);

	float rhor, rhol;

	rhor = sqrt(rr.x*rr.x+rr.y*rr.y);
	rhol = sqrt(rl.x*rl.x+rl.y*rl.y);

	vx = goalx;
	vy = goaly;
	if((rr.x<500.0)&&((rr.x+200.0)<goalx))
	{

		if(vx > 200.0)
			vx = (vx/(fabs(vx))) * 100.0;//vx - rr.x;
		else
			vx = -(vx/(fabs(vx))) * 100.0;//vx - rr.x;
		vy = vy - (10.0*rr.y);
	}

	if((rl.x<500.0)&&((rl.x+200.0)<goalx))
	{
		if(vx > 200.0)
			vx = (vx/(fabs(vx))) * 100.0;//vx - rr.x;
		else
			vx = -(vx/(fabs(vx))) * 100.0;//vx - rr.x;

		vy = vy - (10.0*rl.y);
	}

	vx =vx/300.0;
	vy =vy/300.0;


}

void
Go2pos::step(void)
{
	_RobotUSDetector->step();

	if (isTime2Run()) {

		startDebugInfo();
		float vx, vy;

		getVector(vx, vy);

		float v,w,l;

		ctrlW->setReference(goalt);
		ctrlL->setReference(vy);///300.0);
		ctrlV->setReference(vx);///300.0);


		w = ctrlW->getOutput();
		l = ctrlL->getOutput();
		v = ctrlV->getOutput();

		//_Body->setVel(0.0,0.0,0.0);
		_Body->setVel(v,w,l);
		endDebugInfo();
	}
	_Body->step();
}

//BUILDER COMMENT. DO NOT REMOVE. auxcode begin

void
Go2pos::init(const string newName, AL::ALPtr<AL::ALBroker> parentBroker) {
	Component::init(newName, parentBroker);
	goalx = goaly = 0.0;

	setFreqTime(SHORT_RATE);

}

void
Go2pos::setGoalPosXY(double x, double y, double t, float f)
{
	goalx = x;
	goaly = y;
	goalt = t;
	factor = f;
}

void
Go2pos::setGoalPos( double x, double y, const Ice::Current& c)
{
	setGoalPosXY(x, y, 0.0);
}
//BUILDER COMMENT. DO NOT REMOVE. auxcode end
