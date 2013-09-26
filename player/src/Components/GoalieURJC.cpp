#include "GoalieURJC.h"

//BUILDER COMMENT. DO NOT REMOVE. auxinclude begin

#define PASQUARE(a) ((a)*(a))
//BUILDER COMMENT. DO NOT REMOVE. auxinclude end

GoalieURJC::GoalieURJC()
{
	_BallDetector = BallDetector::getInstance();
	_GoalDetector = GoalDetector::getInstance();
	_WorldState = WorldState::getInstance();
	_Attention = Attention::getInstance();
	state = Initial;
}

GoalieURJC::~GoalieURJC()
{

}

void GoalieURJC::Initial_state_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Initial_state_code
//BUILDER COMMENT. DO NOT REMOVE. end Initial_state_code
}

void GoalieURJC::Block_state_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Block_state_code

	_Attention->setPercentage("Ball", 1.0);
	_Attention->setPercentage("Goal", 0.0);

return;
/*

	//cerr<<"POS!!!!"<<endl;
	HPoint2D ball_position, goal_center, goalie_pos;
	HPoint3D ball_vector, circle_center;
	HPoint3D intersection1, intersection2;
	double radius;
	double goalx = -3000.0;
	double circle_offset = 1200.0;
	double radius_offset = 300.0;
	Pose2D robotpos;
	float vw;


	if(_WorldState->getReliability()>0.5)
	{
		robotpos = _WorldState->getPosition();

	if(_BallDetector->ballmodel->elapsedTimeSinceLastObs < ObjectState::LONG_TIME) {
		ObjectState* balls = _BallDetector->getObj();
		Vector2<double> pos = balls->getPositionInRelativeCoordinates();

		ball_position.x = cos(robotpos.rotation)*pos[0] - sin(robotpos.rotation)*pos[1] + robotpos.translation.x;
		ball_position.y = sin(robotpos.rotation)*pos[0] + cos(robotpos.rotation)*pos[1] + robotpos.translation.y;
		vw = balls->getAngle();

		//cerr<<"["<<ball_position.x<<", "<<ball_position.y<<"]"<<endl;
	} else {
		ball_position.x = 0.0;
		ball_position.y = 0.0;
		vw =  -robotpos.rotation;
	}
*/
/*
	Vector from ball_position to goal center
	goal_center.x = goalx;
	goal_center.y = 0.0;
	this->calcVector2D(ball_position, goal_center, ball_vector);

	Intersection vector and circle
	circle_center.X = goalx - circle_offset;
	circle_center.Y = 0.0;
	circle_center.Z = 0.0;
	radius = circle_offset + radius_offset;
	this->calIntersectionCircleVector(ball_vector, circle_center, radius, intersection1, intersection2);

	Check intersection
	if(intersection1.H == 0 || intersection2.H == 0) {
		Default position
		goalie_pos.x = goalx + radius_offset;
		goalie_pos.y = 0.0;
	} else {
		Calculate closest intersection
		if(intersection1.X >= goalx) {
			goalie_pos.x = intersection1.X;
			goalie_pos.y = intersection1.Y;
		} else if(intersection2.X >= goalx){
			goalie_pos.x = intersection2.X;
			goalie_pos.y = intersection2.Y;
		} else {
			Default position
			goalie_pos.x = goalx + radius_offset;
			goalie_pos.y = 0.0;
		}
	}

	//cerr<<"["<<goalie_pos.x<<", "<<goalie_pos.y<<"]"<<endl;

	}else
	{
		_Body->setVelV(0.0);
		_Body->setVelL(0.0);

		if(_BallDetector->ballmodel->elapsedTimeSinceLastObs < ObjectState::LONG_TIME) {
			ObjectState* balls = _BallDetector->getObj();
			vw = balls->getAngle();

			//cerr<<"["<<ball_position.x<<", "<<ball_position.y<<"]"<<endl;
		} else
			vw =  0.0;

	}

	_Attention->setPercentage("Ball", 0.5);
	_Attention->setPercentage("Goal", 0.5);

	_Go2gpos->setGoalPosRelAng(goalie_pos.x, goalie_pos.y, vw);
*/

	//_Body->setVelW(vw/2.0);
//BUILDER COMMENT. DO NOT REMOVE. end Block_state_code
}

void GoalieURJC::Despeje_state_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Despeje_state_code

	//cerr<<"DESPEJE!!!!"<<endl;
//BUILDER COMMENT. DO NOT REMOVE. end Despeje_state_code
}

bool GoalieURJC::Initial2Block0_transition_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Initial2Block0_transition_code
return true;
//BUILDER COMMENT. DO NOT REMOVE. end Initial2Block0_transition_code
}

bool GoalieURJC::Block2Despeje0_transition_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Block2Despeje0_transition_code
return false;

	Pose2D robotpos;
	if(_WorldState->getReliability()>0.5)
	{
		robotpos = _WorldState->getPosition();
		HPoint2D ball_position;

		if(_BallDetector->ballmodel->elapsedTimeSinceLastObs < ObjectState::LONG_TIME)
		{
			ObjectState* balls = _BallDetector->getObj();
			Vector2<double> pos = balls->getPositionInRelativeCoordinates();

			static Vector2<double> posabs = Pose2D::relative2FieldCoord(robotpos, pos.x, pos.y);

			if ((posabs.x<-2200.0) && (fabs(posabs.y)<1200.0))
				return true;
			else
				return false;
		}else
		{
			//cerr<<"No veo la pelota"<<endl;
			return false;
		}
	}else
		return false;

//BUILDER COMMENT. DO NOT REMOVE. end Block2Despeje0_transition_code
}

bool GoalieURJC::Despeje2Block0_transition_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Despeje2Block0_transition_code
	Pose2D robotpos;

	if(_WorldState->getReliability()>0.3)
	{
		robotpos = _WorldState->getPosition();
		HPoint2D ball_position;

	if(_BallDetector->ballmodel->elapsedTimeSinceLastObs > ObjectState::LONG_TIME)
	{
		return true;
	}

	ObjectState* balls = _BallDetector->getObj();
	Vector2<double> pos = balls->getPositionInRelativeCoordinates();

	static Vector2<double> posabs = Pose2D::relative2FieldCoord(robotpos, pos.x, pos.y);

	if ((posabs.x>-2000.0) || (fabs(posabs.y)>1500.0))
		return true;
	else
		return false;
	}else
	{
		return true;
	}
//BUILDER COMMENT. DO NOT REMOVE. end Despeje2Block0_transition_code
}

void
GoalieURJC::step(void)
{
	switch (state)
	{
	case Initial:

		if (isTime2Run()) {
			Initial_state_code();

			if (Initial2Block0_transition_code()) {
				state = Block;
			}
		}

		break;
	case Block:
		_BallDetector->step();
		_GoalDetector->step();
		_WorldState->step();

		if (isTime2Run()) {
			Block_state_code();

			if (Block2Despeje0_transition_code()) {
				state = Despeje;
			}
		}

		_Attention->step();
		break;
	case Despeje:
		_GoalDetector->step();
		_Attention->step();
		_BallDetector->step();

		if (isTime2Run()) {
			Despeje_state_code();

			if (Despeje2Block0_transition_code()) {
				state = Block;
			}
		}

		break;
	default:
		cout << "[GoalieURJC::step()] Invalid state\n";
	}
}

//BUILDER COMMENT. DO NOT REMOVE. auxcode begin


void GoalieURJC::calcVector2D(HPoint2D p1, HPoint2D p2, HPoint3D &v)
 {
		/*Get the Ax + By + C = 0 parameters*/
		v.X = p1.y - p2.y;                              //y1*z2 - z1*y2
		v.Y = p2.x - p1.x;                              //z1*x2 - x1*z2
		v.Z = p1.x*p2.y - p1.y*p2.x;    //x1*y2 - y1*x2
		v.H = 1.0;
 }

 /*Return the intersection between a circle (radius and P_central) and a vector*/
 void GoalieURJC::calIntersectionCircleVector(HPoint3D v, HPoint3D p_c, double r, HPoint3D &int1, HPoint3D &int2)
 {
		/*Solve equations:
		(x-px)^2 + (y - py)^2 = r^2
		Ax + By + C = 0*/

		double i,j,k, A_2;
		double a,b,c;
		double tmp;

		if(v.X == 0.0) {
			/*Avoid div by 0*/
		  v.X = 0.000001;
		}

		i = -2*p_c.X;
		j = -2*p_c.Y;
		k = PASQUARE(p_c.X) + PASQUARE(p_c.Y) - PASQUARE(r);

		A_2 = PASQUARE(v.X);
		a = PASQUARE(-v.Y)/A_2 + 1;
		b = -2*v.Z*-v.Y/A_2  - v.Y*i/v.X + j;
		c = PASQUARE(v.Z)/A_2 - v.Z*i/v.X + k;

		/*Solve a*Y^2 + b+Y + c = 0*/
		tmp = PASQUARE(b) - 4*a*c;
		if(tmp<0) {
			/*No intersection*/
		  int1.H = 0.0;
		  int2.H = 0.0;
		  return;
		}

		tmp = sqrt(tmp);
		int1.Y = (-b + tmp)/(2*a);
		int2.Y = (-b - tmp)/(2*a);

		/*Get X Coordinate*/
		int1.X = (-v.Y*int1.Y - v.Z)/v.X;
		int2.X = (-v.Y*int2.Y - v.Z)/v.X;

		int1.H = 1.0;
		int2.H = 1.0;
 }
//BUILDER COMMENT. DO NOT REMOVE. auxcode end

