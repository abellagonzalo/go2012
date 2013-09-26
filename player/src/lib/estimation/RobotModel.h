/**
 * @class RobotModel.h
 *
 * Contains all current knowledge about other robots on the field
 *
 * @author <A href="mailto:caguero@gsyc.es">Carlos Agüero</A>
 */

#ifndef __RobotModel_h_
#define __RobotModel_h_

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

class RobotModel: public DebugIRel {

public:

	RobotModel();
	void predict();
	void updateFromOdometry();
	void updateFromObservationOpponents(list<AbstractSample> features);
	void updateFromObservationTeammates(list<AbstractSample> features);
	bica::ShapeList getGrDebugRel();

	/** JPDAF **/
	JPDAF *opponentsJPDAF;
	JPDAF *teammatesJPDAF;

	Kinematics *kinematics;
private:

	GameController *gameController;
};

#endif //__RobotModel_h_
