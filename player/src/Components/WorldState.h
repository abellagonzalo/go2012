#ifndef WorldState_H
#define WorldState_H

#include "Component.h"
#include "Localization.h"
#include "BallDetector.h"
#include "GoalDetector.h"
#include "RobotDetector.h"
#include "SharedTeamInfo.h"
#include "GTLocalization.h"
#include "RobotLog.h"

#include "Singleton.h"

typedef struct
{
	int id;
	Pose2D pos;
	float prob;
}tState;

class SharedTeamInfo;

class WorldState : public Component, public Singleton<WorldState>, public LoggableI
{
public:

	WorldState();
	~WorldState();

	void step();

	inline ObjectState* getOpponentLeft() { return opponentLeft; };
	inline ObjectState* getOpponentRight() { return opponentRight; };
	inline ObjectState* getOwnLeft() { return ownLeft; };
	inline ObjectState* getOwnRight() { return ownRight; };

	inline Pose2D getPosition() {return myPos;};
	inline float getReliability() {return myReliability;};

	Vector2<double> getGoalLVector() {return vleft;};
	Vector2<double> getGoalRVector() {return vright;};

	void inMyField();
	void inOppField();
private:


	void updateLocalHipotheses();
	void updateBall();
	void updateRobots();
	void updateWorld();
	void updateGVector();
	void updateGoalie();

	void UpdateOwnGoal(ObjectState *left, ObjectState *right);
	void UpdateOpponentGoal(ObjectState *left, ObjectState *right);

	void normalize();
	void setGoals();

	void ProcessPosts(ObjectState *left, ObjectState *right, list<AbstractSample> *mates, list<AbstractSample> *opponents);
	void ProcessPosts(ObjectState *post, list<AbstractSample> *mates, list<AbstractSample> *opponents);

	ObjectState* opponentLeft;
	ObjectState* opponentRight;
	ObjectState* ownLeft;
	ObjectState* ownRight;

	Localization *_Localization;
	BallDetector *_BallDetector;
	GoalDetector *_GoalDetector;
	RobotDetector *_RobotDetector;
	SharedTeamInfo	*_SharedTeamInfo;
	GTLocalization *_GTLocalization;

	Pose2D myPos;
	float myReliability;

	Vector2<double> vleft;
	Vector2<double> vright;

	map<int, tState> hyps;
};

#endif

