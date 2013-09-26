#ifndef GTLOCALIZATION_H
#define GTLOCALIZATION_H

#include "AbstractLocalization.h"
#include "Singleton.h"
#include "RobotLog.h"

#include <IceE/IceE.h>
#include <debug.h>
#include "Debug.h"
#include <componentsI.h>

class GTLocalization : public AbstractLocalization, public Singleton<GTLocalization>, public LoggableI, public DebugIAbs
{
public:

	GTLocalization();
	~GTLocalization();

	void init(const string newName, AL::ALPtr<AL::ALBroker> parentBroker);
	void step();

	float getReliability();
	void getPosition(double &xpos, double &ypos, double &thetapos);
	void getPosition(vector<tRobotHipothesis> &hipotheses);
	void setPosition(double xpos, double ypos, double thetapos);


	void resetToPosition(int state);

	/*Debug*/

	bica::ShapeList getGrDebugAbs();

	//ALValue getImgDebug();
	bica::ImageDataPtr getImgDebug();
	string getInfo();

	void setInfo(MotionInfo &motion, PerceptionInfo &perception);
	void UpdateInfo();
	void run();

private:

	double x, y, theta;
	double xmod, ymod, thetamod;
};

#endif
