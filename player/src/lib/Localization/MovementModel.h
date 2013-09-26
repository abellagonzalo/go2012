#ifndef MOVEMENTMODEL_H
#define MOVEMENTMODEL_H

#include "Component.h"
#include "Singleton.h"
#include "Body.h"
#include "almath/tools/almath.h"

class MovementModel : public Singleton<MovementModel>
{
public:

	MovementModel();
	~MovementModel();

	/*Init the global movement of the robot*/
	void initMovement();

	/*If true, return the relative moment of the robot, else, the robot is not moving*/
	bool getMovement(float &movx, float &movy, float &movtheta);

	/*Obtain the new position of a robot taking into account the relative movement and the orientation*/
	void getNewPosition(float movx, float movy, float movtheta, float &posx, float &posy, float &postheta);

	/*Check if the position of the robot is valid, if it's not, the position is corrected*/
	void checkValidPosition(float &posx, float &posy, float &postheta);

private:

	Body *_Body;

	static const float MAX_X_POS;
	static const float MIN_X_POS;
	static const float MAX_Y_POS;
	static const float MIN_Y_POS;
	static const float MAX_THETA_POS;
	static const float MIN_THETA_POS;

	float lastbodyx;
	float lastbodyy;
	float lastbodyt;
};

#endif
