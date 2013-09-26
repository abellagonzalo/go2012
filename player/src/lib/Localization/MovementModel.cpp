#include "MovementModel.h"

const float	MovementModel::MAX_X_POS = 3500.0;
const float	MovementModel::MIN_X_POS = -3500.0;
const float	MovementModel::MAX_Y_POS = 2500.0;
const float	MovementModel::MIN_Y_POS = -2500.0;
const float	MovementModel::MAX_THETA_POS = M_PI;
const float	MovementModel::MIN_THETA_POS = -M_PI;

MovementModel::MovementModel()
{
	_Body = Body::getInstance();
}

MovementModel::~MovementModel()
{
}

void
MovementModel::initMovement()
{
	_Body->getGlobalMovement(this->lastbodyx, this->lastbodyy, this->lastbodyt);
}

bool
MovementModel::getMovement(float &movx, float &movy, float &movtheta)
{
	/*Check if the robot has moved*/
	if(!_Body->hasMoved(this->lastbodyx, this->lastbodyy, this->lastbodyt)) {
		/*Get current values of movement*/
		_Body->getGlobalMovement(this->lastbodyx, this->lastbodyy, this->lastbodyt);
		return false;
	}

	/*Calculate movement*/
	_Body->getRelativeMovement(this->lastbodyx, this->lastbodyy, this->lastbodyt, movx, movy, movtheta);

	/*Get current values of movement*/
	_Body->getGlobalMovement(this->lastbodyx, this->lastbodyy, this->lastbodyt);
	
	return true;
}

void
MovementModel::getNewPosition(float movx, float movy, float movtheta, float &posx, float &posy, float &postheta)
{
	float newx, newy, newtheta;

	/*Get new position*/
	_Body->getAbsoluteMovement(movx, movy, movtheta, posx, posy, postheta, newx, newy, newtheta);

	/*Check new position*/
	this->checkValidPosition(newx, newy, newtheta);

	posx = newx;
	posy = newy;
	postheta = newtheta;
}

void
MovementModel::checkValidPosition(float &posx, float &posy, float &postheta)
{
	if(posx > MAX_X_POS)
		posx = MAX_X_POS;
	if(posx < MIN_X_POS)
		posx = MIN_X_POS;
	if(posy > MAX_Y_POS)
		posy = MAX_Y_POS;
	if(posy < MIN_Y_POS)
		posy = MIN_Y_POS;
	if(postheta > MAX_THETA_POS)
		postheta = postheta-2*M_PI;
	else if(postheta < MIN_THETA_POS)
		postheta = postheta+2*M_PI;
}
