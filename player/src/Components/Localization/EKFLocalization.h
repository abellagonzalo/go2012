#ifndef EKFLOCALIZATION_H
#define EKFLOCALIZATION_H

#include "AbstractLocalization.h"
#include "WorldModel.h"
#include "MovementModel.h"
#include "Singleton.h"
#include "Matrix.h"
#include "Pose2D.h"

#include "EKF_3D.h"
#include "MarkovGrid.h"

#include "GameController.h"

#include "Perception.h"
#include "LineDetector.h"
#include "GoalDetector.h"

#include <IceE/IceE.h>
#include <debug.h>
#include "Debug.h"
#include <componentsI.h>

#include <pthread.h>

#include "alproxies/almotionproxy.h"

using namespace goalDescription;
using namespace bica;
//BUILDER COMMENT. DO NOT REMOVE. auxinclude end



class EKFLocalization : public AbstractLocalization, public Singleton<EKFLocalization>, public DebugIRel, public DebugIAbs, public NEKFLineDbgManager
{
public:

	EKFLocalization();
	~EKFLocalization();

	void init(const string newName, AL::ALPtr<AL::ALBroker> parentBroker);
	void step();

	void initGrid();
	void resetUniform();

	void run();

	void resetToPosition(int state);
	void addPosition(float x, float y, float theta);

	float getReliability();
	void getPosition(double &xpos, double &ypos, double &thetapos);
	void getPosition(vector<tRobotHipothesis> &hipotheses);
	float getQuality();

	void setInfo(MotionInfo &motion, PerceptionInfo &perception);
	void UpdateInfo();


	virtual void setPos( float x, float y, float t, const Ice::Current& c );

	string getDbgInfo();
	/*Debug*/
	//ALValue getImgDebug();
	bica::ImageDataPtr getImgDebug();
	string getInfo();

	bica::ShapeList getGrDebugAbs();
	bica::ShapeList getGrDebugRel();
private:

	void predict();
	void correct();

	void updateGrid();
	void updateEKF(EKF_3D &filter);

	void updateLm(HPoint3D *wmlm,  PostInfo &perception, bool ambiguous=false);
	void updateCircle(HPoint3D *wmlm, CircleSample *circle);

	void ApplyLandmarkObs(EKF_3D *filter, HPoint2D obs, HPoint2D lm, float ang_std_dev, float dist_std_dev);

	float getGoalCircleProb(float x, float y);

	float getGoalPostDistanceStdDev(HPoint2D & rel);
	float getGoalPostAngleStdDev(HPoint2D & rel);

	void correctLines(EKF_3D &filter);
	float getDistance(wmLine* wmline, Vector2<double> ps3D, Vector2<double> pe3D);
	float getDiffLong(wmLine* wmline, Vector2<double> ps3D, Vector2<double> pe3D);
	void getEqLine(Vector2<double> p, Vector2<double> vd, double &A, double &B, double &C);
	bool getInterseccLines (double A, double B, double C, double D, double E, double F, Vector2<double> &i);
	float getDistance(Vector2<double> p1, Vector2<double> p2);
	bool inSegment(HPoint3D p1, HPoint3D p2, Vector2<double> pt);

	wmLine *getMatchingWorldLine(LineSample detline, Pose2D &robotPose);
	Pose2D getIdealPose(Pose2D &robotPose, LineSample &detline, wmLine *line);

	void initEKFtoGrid(EKF_3D &resekf);

	tEKF getBestEKF();
	float getQ(EKF_3D *ekf);

	void testGameEvents();
	void removeFilters();

	void WTDreset2Initial(RoboCupGameControlData *gc, float MT, float MO);
	void WTDreset2Ready(RoboCupGameControlData *gc, float MT, float MO);
	void WTDreset2Set(RoboCupGameControlData *gc, float MT, float MO);
	void WTDreset2Playing(RoboCupGameControlData *gc, float MT, float MO);
	void WTDresetFromPenalised(RoboCupGameControlData *gc, float MT, float MO);
	void WTDreset2Penalty(RoboCupGameControlData *gc, float MT, float MO);

	int GameEvent;

	MarkovGrid *mgrid;

	tEKF ekf;

	long tsekf;
	double lastov;

	double last_quality;

	WorldModel *_WorldModel;
	MovementModel  *_MovementModel;

	GoalDetector *_GoalDetector;
	LineDetector *_LineDetector;

	pthread_mutex_t mutex;
	float dbgx,dbgy,dbgt;
	list<HPoint3D> linesdbg;
	tdbg posdbg;

	AL::ALPtr<AL::ALMotionProxy> pmotion;
	vector<float> lastpos;

	int id;
	MotionInfo _motion;
	PerceptionInfo _perception;

};

#endif
