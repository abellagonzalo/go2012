/**
 * @class GoalsModel.h
 *
 * Contains all current knowledge about the goals.
 *
 * @author <A href="mailto:caguero@gsyc.es">Carlos Agüero</A>
 */

#ifndef __GoalsModel_h_
#define __GoalsModel_h_

#include "RobotPose.h"
#include "progeo.h"
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <iostream>
#include <sys/time.h>
#include "estimation/JPDAF.h"
#include "ObjectState.h"
#include "Debug.h"
#include "GameController.h"
#include "Kinematics.h"

namespace goalDescription
{
class GoalPostSample : public AbstractSample {
public:

	int numAcc;
	int column;
	int minx;
	int maxx;
	int miny;
	int maxy;
	int color;
	bool minFound;
	bool maxFound;
	int type;
	bool valid;
};

class GoalSample : public AbstractSample {
public:

	GoalPostSample * post1;
	GoalPostSample * post2;
	int type;
	int color;
	bool found;
};

static const int GOAL_NOT_FOUND = 0;
static const int GOAL_LEFT_POST = 1;
static const int GOAL_RIGHT_POST = 2;
static const int GOAL_UNKNOWN_POST = 3;
static const int GOAL_COMPLETE = 4;
}

using namespace std;
using namespace goalDescription;

class GoalsModel: public DebugIRel {

public:


	GoalsModel();
	void predict();
	void updateFromOdometry();
	void updateFromObservation(int color, list<AbstractSample> features);
	//void updateFromObservation(GoalSample feature);
	bica::ShapeList getGrDebugRel();
	void generateVirtualNet(HPoint3D p0, HPoint3D p1, HPoint3D &vp0, HPoint3D &vp1 );

/*	ObjectState* getBlueLeft();
	ObjectState* getBlueRight();
	ObjectState* getYellowLeft();
	ObjectState* getYellowRight();
	ObjectState* getOwnLeft() {return ownLeftEstimate;};
	ObjectState* getOwnRight() {return ownRightEstimate;};
	ObjectState* getOpponentLeft() {return opponentLeftEstimate;};
	ObjectState* getOpponentRight() {return opponentRightEstimate;};

	long getBlueElapsedTimeSinceLastObs();
	long getYellowElapsedTimeSinceLastObs();
	long getOwnElapsedTimeSinceLastObs();
	long getOpponentElapsedTimeSinceLastObs();
*/
	/** The state of the object estimated from own observations;
	      it is propagated even if the object is not seen */

	//ToDo: Remove
/*	ObjectState opponentNetCenterEstimate;
	ObjectState ownNetCenterEstimate;
*/

	//ToDo: Remove
	/** Post estimations of each net */
/*	ObjectState *opponentLeftEstimate;
	ObjectState *opponentRightEstimate;
	ObjectState opponentP0Estimate;
	ObjectState opponentP1Estimate;
	ObjectState *ownLeftEstimate;
	ObjectState *ownRightEstimate;
	ObjectState ownP0Estimate;
	ObjectState ownP1Estimate;
*/
	// New interface
	ObjectState p0NetCenterEstimate;
	ObjectState p1NetCenterEstimate;
	ObjectState p0LeftEstimate;
	ObjectState p0RightEstimate;
	ObjectState p1LeftEstimate;
	ObjectState p1RightEstimate;



	/** Number of miliseconds since the last observation */
	/*long opponentElapsedTimeSinceLastObs;
	long ownElapsedTimeSinceLastObs;
*/
	long p0ElapsedTimeSinceLastObs;
	long p1ElapsedTimeSinceLastObs;


	/** JPDAF **/
	JPDAF *blueJPDAF;
	JPDAF *yellowJPDAF;
	JPDAF *opponentJPDAF, *ownJPDAF;
	int p0l, p0r, p1l, p1r;

	Kinematics *kinematics;

private:
	enum GoalType {
		P0,
		P1
	};

	void updatePostReliability(ObjectState *post)
	{
		if (post->getQuality() > ObjectState::HIGH_RELIABILITY_QUALITY)
			post->setReliability(ObjectState::MOST_RELIABLE);
		else if (post->getQuality() > ObjectState::MEDIUM_RELIABILITY_QUALITY)
			post->setReliability(ObjectState::HIGH_RELIABLE);
		else if (post->getQuality() > ObjectState::LOW_RELIABILITY_QUALITY)
			post->setReliability(ObjectState::MEDIUM_RELIABLE);
		else if (post->getQuality() > ObjectState::VERY_LOW_RELIABILITY_QUALITY)
			post->setReliability(ObjectState::LOW_RELIABLE);
		else
			post->setReliability(ObjectState::UNRELIABLE);
	}

	float evaluatePosts (int *posts);
	void labelPosts(int &p0l, int &p0r, int &p1l, int &p1r);

	void updateEstimate();
	float calculateSingleObjectQuality( int object );
	float calculateNetQuality(GoalType net);
	ObjectState::ObjectReliability calculateNetReliability(GoalType net);
	void getGoalCenter(GoalType net, int *x, int *y);
	void fitPosts(GoalType net);
	void fitNets();

	inline long getCurrentTime() {
		gettimeofday(&currentTime, NULL);
		return currentTime.tv_sec * 1000000 + currentTime.tv_usec;
	}

	struct timeval 	currentTime;
//	long opponentLastMeasurement;
//	long ownLastMeasurement;

	long p0LastMeasurement;
	long p1LastMeasurement;


	ShapeList auxList;
	GameController *gameController;
};

#endif //__GoalsModel_h_

