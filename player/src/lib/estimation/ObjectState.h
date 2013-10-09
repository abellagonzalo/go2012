/**
 * @file ObjectState.h
 *
 * Declaration of class ObjectModel
 *
 * @author <A href="mailto:timlaue@informatik.uni-bremen.de">Tim Laue</A>
 * @author <A href="mailto:caguero@gsyc.es">Carlos Agüero</A>
 */

#ifndef __ObjectState_h_
#define __ObjectState_h_

#include "RobotPose.h"
#include "progeo.h"
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <iostream>
#include <sys/time.h>
#include "estimation/JPDAF.h"

using namespace std;

/** ======================================================
 * @class ObjectState
 *
 * Base class for a model of an object
 * =======================================================*/
class ObjectState
{
public:
	// Reliability type
	enum ObjectReliability {
		UNRELIABLE,
		LOW_RELIABLE,
		MEDIUM_RELIABLE,
		HIGH_RELIABLE,
		MOST_RELIABLE
	};

	/* Constants for time elapsed since last observation */
	static const long 	NEVER		= 600 * 1000;		//60'
	static const long	VERY_LONG_TIME	= 30 * 1000;	// 30"
	static const long	LONG_TIME	= 10 * 1000;	// 10"
	static const long	RECENTLY	= 2 * 1000;		// 2"
	static const long	CURRENTLY	= 200;			// 0.2"

	/* Constants for position uncertainty */
	static const float 	MAX_UNCERTAINTY_AREA = 3000.0f * 3000.0f * GEOMETRY_PI;

	// Very low reliability quality means the object is inside the area of an
	//ellipse of 2m. x 2m.
	static const float	VERY_LOW_RELIABILITY_QUALITY		=
			1.0f - ((2000.0f * 2000.0f * GEOMETRY_PI) / (3000.0f * 3000.0f * GEOMETRY_PI));

	// Low reliability quality means the object is inside the area of an
	//ellipse of 1,5m. x 1,5m.
	static const float	LOW_RELIABILITY_QUALITY		=
			1.0f - ((1500.0f * 1500.0f * GEOMETRY_PI) / (3000.0f * 3000.0f * GEOMETRY_PI));

	// Medium reliability quality means the object is inside the area of an
	// ellipse of 1m. x 1m.
	static const float MEDIUM_RELIABILITY_QUALITY	=
			1.0f - ((1000.0f * 1000.0f * GEOMETRY_PI) / (3000.0f * 3000.0f * GEOMETRY_PI));

	// High reliability quality means the object is inside the area of an
	// ellipse of 0.5m. x 0.5m
	static const float HIGH_RELIABILITY_QUALITY		=
			1.0f - ((500.0f * 500.0f * GEOMETRY_PI) / (3000.0f * 3000.0f * GEOMETRY_PI));

	ObjectState() {
		reliabilityString[UNRELIABLE] = "Unreliable";
		reliabilityString[LOW_RELIABLE] = "Low";
		reliabilityString[MEDIUM_RELIABLE] = "Medium";
		reliabilityString[HIGH_RELIABLE] = "High";
		reliabilityString[MOST_RELIABLE] = "Most";
	}


	Vector2<double> getVelocityInFieldCoordinates(const RobotPose& rp) const {
		double c(rp.getCos());
		double s(rp.getSin());
		return Vector2<double>(velocity.x*c - velocity.y*s, velocity.x*s + velocity.y*c);
	}

	Vector2<double> getPositionInRelativeCoordinates() const {
		return position;
	}

	float getQuality() const
	{
		return quality;
	}

	Vector2<double> getVelocityInRelativeCoordinates() const {
		return velocity;
	}

	double getAngle() const
	{
		return atan2(position.y, position.x);
	}

	double getDistance() const
	{
		return position.abs();
	}

	void setPositionAndVelocityInRelativeCoordinates(const Vector2<double>& position,
			const Vector2<double>& velocity = Vector2<double>(0.0,0.0))
	{
		this->position = position;
		this->velocity = velocity;
	}

	void setQuality(double newQuality)
	{
		this->quality = newQuality;
	}

	void setReliability(ObjectReliability newReliability)
	{
		this->reliability = newReliability;
	}

	ObjectReliability getReliability() const
	{
		return reliability;
	}

	std::string getReliabilityString() const {
		return reliabilityString.find(reliability)->second;
	}

	const ObjectState& operator=(const ObjectState& other)
	{
		position	= other.position;
		velocity	= other.velocity;
		quality		= other.quality;
		reliability = other.reliability;
		return *this;
	}

protected:
	/** The position of the object relative to the robot (in mm)*/
	Vector2<double> position;

	/** The velocity of the object relative to the robot (in mm/s)*/
	Vector2<double> velocity;

	/** The quality of the object's estimation (0 = poor and 1 = excelent */
	double quality;

	/** Reliability of the object's estimation */
	ObjectReliability reliability;

	/** Reliability to string */
	std::map<ObjectReliability, std::string> reliabilityString;
};

#endif //__ObjectState_h_
