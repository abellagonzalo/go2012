#include "GTLocalization.h"


GTLocalization::GTLocalization()
{
	x = y = theta = 0.0;
	setFreqTime(300);
}

GTLocalization::~GTLocalization()
{

}

void
GTLocalization::init(const string newName, AL::ALPtr<AL::ALBroker> parentBroker)
{
	Component::init(newName, parentBroker);

	xmod = ymod = 1.0;
	thetamod = 0.0;

	if(this->getPlayerColor() == TEAM_BLUE)
	{
		xmod = -1.0;
		ymod = -1.0;
		//thetamod = M_PI;
	}
}

float
GTLocalization::getReliability()
{
	return 1.0;
}

void
GTLocalization::step(void)
{
	//fprintf(stderr, "X = %lf\tY = %lf\tTheta = %lf\n", x, y, theta*180.0/3.1415);
}


void
GTLocalization::getPosition(vector<tRobotHipothesis> &hipotheses)
{
	tRobotHipothesis aux;
	getPosition(aux.x, aux.y, aux.t);
	aux.prob = 1.0;

	hipotheses.clear();
	hipotheses.push_back(aux);
}

void
GTLocalization::getPosition(double &xpos, double &ypos, double &thetapos)
{
	xpos = x;
	ypos = y;
	thetapos = theta;
}

void
GTLocalization::setPosition(double xpos, double ypos, double thetapos)
{
	x = -xpos*xmod;;
	y = ypos*ymod;
	theta = thetapos + thetamod;

	while(theta>M_PI) theta = theta - 2 * M_PI;
	while(theta<-M_PI) theta = theta + 2 * M_PI;
}

bica::ImageDataPtr
GTLocalization::getImgDebug()
{
	return 0;
}

string 
GTLocalization::getInfo()
{
	return "";
}


bica::ShapeList
GTLocalization::getGrDebugAbs()
{
	shapeListAbs.clear();

	bica::Point3DPtr src(new bica::Point3D);
	bica::Point3DPtr dst(new bica::Point3D);
	bica::ArrowPtr a(new bica::Arrow);

	src->x = (float)x;
	src->y = (float)y;
	src->z = 600.0;
	dst->x = (float)x + 600.0 * cos(theta);
	dst->y = (float)y + 600.0 * sin(theta);
	dst->z = 600.0;

	a->src = src;
	a->dst = dst;
	a->width = 10.0;
	a->color = bica::BLUE;
	a->filled = true;
	a->opacity = 255;
	a->accKey = "c";
	a->label = "RobotGT";
	shapeListAbs.push_back(a);

	return shapeListAbs;
}
void
GTLocalization::resetToPosition(int state)
{
	//Reset depending on the game situation and the game player
}


void
GTLocalization::setInfo(MotionInfo &motion, PerceptionInfo &perception)
{
}

void
GTLocalization::UpdateInfo()
{
}

void
GTLocalization::run()
{
}
