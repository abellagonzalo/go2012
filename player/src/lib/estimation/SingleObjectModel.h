/**
 * @class SingleObjectModel.h
 *
 * Contains all current knowledge about the object.
 *
 * @author <A href="mailto:caguero@gsyc.es">Carlos Agüero</A>
 */

#ifndef __SingleObjectModel_h_
#define __SingleObjectModel_h_

#include "RobotPose.h"
#include "progeo.h"
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <iostream>
#include <sys/time.h>
#include "estimation/JPDAF.h"
#include "ObjectState.h"
#include "Debug.h"


class SingleObjectModel: public DebugIRel
{
public:

	SingleObjectModel(int numObjects, int maxNFeatures);
	void predict();
	void updateFromOdometry();
	void updateFromObservation(list<AbstractSample> features);
	bica::ShapeList getGrDebugRel();

	/** The state of the object estimated from own observations;
      it is propagated even if the object is not seen */
	ObjectState estimate;

	/** Number of miliseconds since the last observation */
	long elapsedTimeSinceLastObs;

	/** JPDAF **/
	JPDAF *jpdaf;

	float dx, dy;

private:

	void updateEstimate();

	void updateReliability()
	{
		if (estimate.getQuality() > ObjectState::HIGH_RELIABILITY_QUALITY)
			estimate.setReliability(ObjectState::MOST_RELIABLE);
		else if (estimate.getQuality() > ObjectState::MEDIUM_RELIABILITY_QUALITY)
			estimate.setReliability(ObjectState::HIGH_RELIABLE);
		else if (estimate.getQuality() > ObjectState::LOW_RELIABILITY_QUALITY)
			estimate.setReliability(ObjectState::MEDIUM_RELIABLE);
		else if (estimate.getQuality() > ObjectState::VERY_LOW_RELIABILITY_QUALITY)
			estimate.setReliability(ObjectState::LOW_RELIABLE);
		else
			estimate.setReliability(ObjectState::UNRELIABLE);
	}

	float calculateSingleObjectQuality( int object );

	inline long getCurrentTime() {
		gettimeofday(&currentTime, NULL);
		return currentTime.tv_sec * 1000000 + currentTime.tv_usec;
	}

	struct timeval 	currentTime;
	Vector2<double> lastPos;
	long lastMeasurement, elapsed;
};

/* =============================================
 * @class BallModel
 *
 * Model of the ball
 * ============================================= */
typedef SingleObjectModel BallModel;

#endif //__ObjectModel_h_
