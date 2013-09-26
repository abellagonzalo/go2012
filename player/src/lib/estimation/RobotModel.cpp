/*
 * Name: RobotModel.cpp
 * @Author: Carlos Agüero (caguero@gsyc.es)
 *
 * Description:
 *
 * Created on: 22/03/2011
 *
 * Copyright (C) Universidad Rey Juan Carlos
 * All Rights Reserved.
 *
 */

#include "RobotModel.h"

RobotModel::RobotModel()
{
	gameController = GameController::getInstance();
	kinematics = Kinematics::getInstance();

	this->opponentsJPDAF = new JPDAF (JPDAF::DYNAMIC_OBJ, 4, 3);
	this->teammatesJPDAF = new JPDAF (JPDAF::DYNAMIC_OBJ, 3, 3);
}

void
RobotModel::predict()
{
	this->opponentsJPDAF->predict();
	this->teammatesJPDAF->predict();
}

void
RobotModel::updateFromOdometry()
{
	this->opponentsJPDAF->correctFromOdometry();
	this->teammatesJPDAF->correctFromOdometry();
}

void
RobotModel::updateFromObservationOpponents(list<AbstractSample> features)
{
	this->opponentsJPDAF->correct( features );
}

void
RobotModel::updateFromObservationTeammates(list<AbstractSample> features)
{
	this->teammatesJPDAF->correct( features );
}

bica::ShapeList
RobotModel::getGrDebugRel()
{
	shapeListRel.clear();

	for (unsigned i = 0; i < 4; i++ ) { // Num opponent robots
		// Robot
		bica::Point3DPtr p(new bica::Point3D);
		p->x = cvmGet(this->opponentsJPDAF->objects[i]->state_post, 0, 0);
		p->y = cvmGet(this->opponentsJPDAF->objects[i]->state_post, 1, 0);
		p->z = 0.0f;

		bica::EllipsePtr estEllipse(new bica::Ellipse);
		estEllipse->center = p;
		estEllipse->width = 4 * sqrt(cvmGet(this->opponentsJPDAF->objects[i]->error_cov_post, 0, 0));
		estEllipse->length = 4 * sqrt(cvmGet(this->opponentsJPDAF->objects[i]->error_cov_post, 1, 1));
		estEllipse->angle =  toDegrees(atan2(cvmGet(this->opponentsJPDAF->objects[i]->state_post, 1, 0),
				cvmGet(this->opponentsJPDAF->objects[i]->state_post, 0, 0)));
		estEllipse->z = 0.0f;
		estEllipse->color = bica::RED;
		estEllipse->filled = true;
		estEllipse->opacity = 125;
		estEllipse->accKey = "o";
		stringstream ss;//create a stringstream
		ss << "opp" << i;// << "L";
		estEllipse->label = ss.str();
		shapeListRel.push_back(estEllipse);
	}

	for (unsigned i = 0; i < 3; i++ ) { // Num teammates robots
		// Robot
		bica::Point3DPtr p(new bica::Point3D);
		p->x = cvmGet(this->teammatesJPDAF->objects[i]->state_post, 0, 0);
		p->y = cvmGet(this->teammatesJPDAF->objects[i]->state_post, 1, 0);
		p->z = 0.0f;

		bica::EllipsePtr estEllipse(new bica::Ellipse);
		estEllipse->center = p;
		estEllipse->width = 4 * sqrt(cvmGet(this->teammatesJPDAF->objects[i]->error_cov_post, 0, 0));
		estEllipse->length = 4 * sqrt(cvmGet(this->teammatesJPDAF->objects[i]->error_cov_post, 1, 1));
		estEllipse->angle =  toDegrees(atan2(cvmGet(this->teammatesJPDAF->objects[i]->state_post, 1, 0),
				cvmGet(this->teammatesJPDAF->objects[i]->state_post, 0, 0)));
		estEllipse->z = 0.0f;
		estEllipse->color = bica::BLUE;
		estEllipse->filled = true;
		estEllipse->opacity = 125;
		estEllipse->accKey = "t";
		stringstream ss;//create a stringstream
		ss << "teammate" << i;// << "L";
		estEllipse->label = ss.str();
		shapeListRel.push_back(estEllipse);
	}

	return shapeListRel;
}
