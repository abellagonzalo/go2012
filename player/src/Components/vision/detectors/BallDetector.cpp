/*
 * Name: BallDetector.h
 * @Author: Eduardo Perdices (eperdices@gsyc.es)
 * @Author: Carlos Agüero (caguero@gsyc.es)
 * 
 * Description: Class to detect balls from the Regions
 *
 * Created on: 31/05/2010
 *
 * Copyright (C) Universidad Rey Juan Carlos
 * All Rights Reserved.
 *
 */

#include "BallDetector.h"
#include "Kinematics.h"

#include <iterator>
#include "estimation/kalman/kalman.h"


const float BallDetector::MIN_VALID_RATIO = 0.8;
const float BallDetector::BALL_RADIUS = 32.5;
const float BallDetector::MIN_VALID_RATIO_RADIUS = 0.7;

BallDetector::BallDetector() : Detector(), BallAttentionClient()
{

	this->storage = cvCreateMemStorage();
	this->color = ImageInput::CORANGE;

	// Ball model
	ballmodel = new BallModel(1, 3);

	lastBodyX = lastBodyY = lastBodyTh = 0.0f;
	lastBallTimestamp = 0;
	measurement = false;
	_gtReceived = false;
}

BallDetector::~BallDetector()
{
}

void
BallDetector::init(const string newName, AL::ALPtr<AL::ALBroker> parentBroker) {
	Component::init(newName, parentBroker);
	perception = Perception::getInstance();
	body = Body::getInstance();
	kinematics =  Kinematics::getInstance();
	_GTLocalization = GTLocalization::getInstance();
	goalDetector = GoalDetector::getInstance();

	this->setFreqTime(SHORT_RATE);

	//For attention
	initClient();

	ballVMlog = fopen((string("/tmp/vmBall.log").c_str()), "w");
	vmIters = 0;
	vmMeanCycleTime = 0.0;
}

float lastMovX = 0.0;
float lastMovY = 0.0;
float lastMovTh = 0.0;

kalman::algorithm::MatrixCMPair filter = kalman::algorithm::pair_generator();

struct Prediction {
	double x, y, theta, time, mobility, friction;
};

kalman::matrices::MatrixCM predictionToMatrix(Prediction pred) {
	double data [6] = {pred.x, pred.y, pred.theta, pred.time, pred.mobility, pred.friction};
	return kalman::matrices::MatrixCM(6, 1, data);
}

void BallDetector::predict_step() {
	// std::cerr << "predict_step" << std::endl;
	float movX, movY, movTh;
	body->getRelativeMovement(lastBodyX, lastBodyY, lastBodyTh, movX, movY, movTh);
	body->getGlobalMovement(lastBodyX, lastBodyY, lastBodyTh);

	Prediction prediction = {movX, movY, movTh, perception->getImageTs(), 0, .95};
	Prediction noise = {.01, .01, 3.1416/3600., 0, 3., .01};

	filter = kalman::algorithm::predict(filter, predictionToMatrix(prediction), predictionToMatrix(noise));
}

std::ostream& operator<<(std::ostream &output, const HPoint2D& p) {
	output << "{" << p.x << ", " << p.y << ", " << p.h << "}";
	return output;
}

std::ostream& operator<<(std::ostream &output, const HPoint3D& p) {
	output << "{" << p.X << ", " << p.Y << ", " << p.Z << ", " << p.H << "}";
	return output;
}

std::ostream& operator<<(std::ostream &output, const AbstractSample& as) {
  output << "n=" << as.n <<" timestamp=" << as.timestamp << " size=" << as.size 
  			 << " p2D=" << as.p2D << " p3D=" << as.p3D;
  return output;
}

std::ostream& operator<<( std::ostream &output, const BallSample& bs) {
	output << (AbstractSample)bs << " ratio=" << bs.ratio << " radius=" << bs.radius
				 << " radius_expected=" << bs.radius_expected;
	return output;
}

struct Observation {
	double x, y;
};

kalman::matrices::MatrixCM observationToMatrix(Observation obs) {
	double data [2] = {obs.x, obs.y};
	return kalman::matrices::MatrixCM(2, 1, data);
}

void BallDetector::update_step() {
	// std::cerr << "Ball update" << std::endl;
	// std::ostream_iterator<BallSample> out_it (std::cout, "\n");
	// std::copy(balls.begin(), balls.end(), out_it);

	if (balls.empty()) return;
	//std::cerr << "update_step" << std::endl;

	std::list<BallSample>::iterator ball = balls.begin();
	Observation observation = {ball->p3D.X, ball->p3D.Y};
	Observation noise = {1., 1.};

	filter = kalman::algorithm::update(filter, observationToMatrix(observation), observationToMatrix(noise));

	// filter.first.print();
	// filter.second.print();
}

void
BallDetector::step()
{

	perception->step();

	body->step();
	kinematics->step();

	if (!isTime2Run())
		return;

	startDebugInfo();

	this->detect(true);

	predict_step();

	long beginVMcycleTime = getCurrentTime();
	ballmodel->predict();
	ballmodel->updateFromOdometry();
	long endVMcycleTime = getCurrentTime() - beginVMcycleTime;
	vmIters++;
	vmMeanCycleTime = ((vmMeanCycleTime * (vmIters - 1)) + endVMcycleTime) / (vmIters);
	ostringstream s;
	s << "[" << "Ball Visual Memory" << "] Mean cycle time of " << vmMeanCycleTime / 1000.0 << " ms.\n";
	if(ballVMlog != NULL)
	{
		fprintf(ballVMlog, "%s", s.str().c_str());
		fflush(ballVMlog);
	}

	update_step();
	/*Save time*/
	if(this->balls.size() > 0)
	{
		updateFromObservation();
		this->lastSeen = this->getTime();
	}


	Vector2<double> ball = ballmodel->estimate.getPositionInRelativeCoordinates();

	HPoint2D simg;
	simg.x = ImageInput::IMG_WIDTH/2;
	simg.y = ImageInput::IMG_HEIGHT/2;
	simg.h = 1.0;
	HPoint3D s3D;
	kinematics->get3DPosition(s3D, simg);

	double quality = ballmodel->estimate.getQuality();

	// Log
	beginLog();
	addToLog ( "BallX", ComponentSnapshot::LOG_DOUBLE, ballmodel->estimate.getPositionInRelativeCoordinates().x );
	addToLog ( "BallY", ComponentSnapshot::LOG_DOUBLE, ballmodel->estimate.getPositionInRelativeCoordinates().y );
	addToLog ( "BallQuality", ComponentSnapshot::LOG_DOUBLE, quality );
	addToLog ( "BallXunc", ComponentSnapshot::LOG_DOUBLE, cvmGet(ballmodel->jpdaf->objects[0]->error_cov_post, 0, 0));
	addToLog ( "BallYunc", ComponentSnapshot::LOG_DOUBLE, cvmGet(ballmodel->jpdaf->objects[0]->error_cov_post, 1, 1));

	if ( _gtReceived ) {
		double ballX, ballY;
		getGTBallRel( ballX, ballY );

		double gtballx, gtbally;
		getGTBallAbs( gtballx, gtbally);

		addToLog ( "GTBallRelX", ComponentSnapshot::LOG_DOUBLE, ballX );
		addToLog ( "GTBallRelY", ComponentSnapshot::LOG_DOUBLE, ballY );

		addToLog ( "GTBallAbsX", ComponentSnapshot::LOG_DOUBLE, gtballx );
		addToLog ( "GTBallAbsY", ComponentSnapshot::LOG_DOUBLE, gtbally );
	}
	endLog( getName() );


	updateAP();
	endDebugInfo();
}

void
BallDetector::detect(bool debug)
{
	/*Clear saved elements*/
	this->clearDetected();

	/*Validate the regions*/
	_RegionBuilder->validateRegions(this->color);

	/*Validate balls*/
	this->validateBalls();
}

void
BallDetector::validateBalls()
{
	BallSample ballSample;
	bool removeBall;
	list<BallRegion*> * bregions = _RegionBuilder->getBallRegions();

	for(list<BallRegion*>::iterator ball = bregions->begin(); ball != bregions->end(); ball++) {

		/*Calculate the ball radius if the ball is valid*/
		if(!this->getBallRadius(*ball, ballSample))
			continue;

		/*Get expected radius*/
		this->getExpectedRadius(ballSample);

		/*Check difference between radius and expected radius*/
		if(!this->checkRadius(ballSample))
			continue;

		this->balls.push_back(ballSample);
	}

	/*Check balls in the same position*/
	if(this->balls.size() >= 2) {
		list<BallSample>::iterator ball1 = this->balls.begin();
		while(ball1 != this->balls.end()) {

			removeBall = false;

			list<BallSample>::iterator ball2 = ball1;
			ball2++;
			while(ball2 != this->balls.end()) {

				/*If they are very close, delete the worst*/
				if((int)calcDistance2D((*ball1).p2D, (*ball2).p2D) < MIN_DIST_BALLS) {

					/*Detele the smaller ball*/
					if((*ball1).radius >= (*ball2).radius) {
						ball2 = this->balls.erase(ball2);
						continue;
					} else {
						removeBall = true;
						break;
					}
				} else
					ball2++;
			}

			if(removeBall)
				ball1 = this->balls.erase(ball1);
			else
				ball1++;
		}
	}
}

bool
BallDetector::getBallRadius(BallRegion* ballRegion, BallSample &ball)
{
	bool border1, border2;
	int dcol=0, drow=0, diag1=0, diag2=0;
	int size1, size2;
	int sum1, sum2, sum3, sum4;

	/*Scan up and down*/
	border1 = _ImageHandler->scanBallExtremeInImage(ballRegion->cent_x, ballRegion->cent_y, size1, 0, -1, this->color);
	border2 = _ImageHandler->scanBallExtremeInImage(ballRegion->cent_x, ballRegion->cent_y, size2, 0, 1, this->color);

	if(!border1 && !border2) {
		sum1 = size1 + size2 + 1;	/*Calc distance taking into account the central point*/
		drow = (size2 - size1)/2;
	} else
		sum1 = -1;

	/*Scan left and right*/
	border1 = _ImageHandler->scanBallExtremeInImage(ballRegion->cent_x, ballRegion->cent_y, size1, -1, 0, this->color);
	border2 = _ImageHandler->scanBallExtremeInImage(ballRegion->cent_x, ballRegion->cent_y, size2, 1, 0, this->color);

	if(!border1 && !border2) {
		sum2 = size1 + size2 + 1;
		dcol = (size2 - size1)/2;
	} else
		sum2 = -1;

	/*Return false if both are in the border*/
	if(sum1 < 0 && sum2 < 0)
		return false;

	/*Both inside the image*/
	if(sum1 >= 0 && sum2 >= 0) {
		/*Save the values in the ball*/
		return this->saveValues(ballRegion, dcol, drow, sum1, sum2, ball);
	}

	/*If we have reached one border, scan the diagonals*/
	/*Scan diag up-left and diag down-right*/
	border1 = _ImageHandler->scanBallExtremeInImage(ballRegion->cent_x, ballRegion->cent_y, size1, -1, -1, this->color);
	border2 = _ImageHandler->scanBallExtremeInImage(ballRegion->cent_x, ballRegion->cent_y, size2, 1, 1, this->color);

	if(!border1 && !border2) {
		sum3 = (int)((float)(size1 + size2) * GEOMETRY_SQRT_2) + 1; /*Mult size*sqrt(2) to obtain the real length*/
		diag1 = (size2 - size1)/2;
	} else
		sum3 = -1;

	/*Scan diag up-right and diag down-left*/
	border1 = _ImageHandler->scanBallExtremeInImage(ballRegion->cent_x, ballRegion->cent_y, size1, 1, -1, this->color);
	border2 = _ImageHandler->scanBallExtremeInImage(ballRegion->cent_x, ballRegion->cent_y, size2, -1, 1, this->color);

	if(!border1 && !border2) {
		sum4 = (int)((float)(size1 + size2) * GEOMETRY_SQRT_2) + 1; /*Mult size*sqrt(2) to obtain the real length*/
		diag2 = (size2 - size1)/2;
	} else
		sum4 = -1;

	/*Return false if the diagonals are in the border*/
	if(sum3 < 0 && sum4 < 0)
		return false;

	return this->saveValues(ballRegion, diag1, drow, std::max(sum1, sum2), std::max(sum3, sum4), ball);
}

bool
BallDetector::saveValues(BallRegion* ballRegion, int col, int row, int sum1, int sum2, BallSample &ball)
{
	float ratio;

	/*Check the ratio of the scan*/
	if(!this->checkBallRatio(sum1, sum2, ratio))
		return false;

	/*Correct the center*/
	ball.p2D.x = ballRegion->cent_x + col;
	ball.p2D.y = ballRegion->cent_y + row;
	ball.p2D.h = 1.0;

	ball.ratio = ratio;

	/*Get the radius*/
	ball.radius = ((float)(std::max(sum1, sum2)))/2.0;

	return true;	
}

bool
BallDetector::checkBallRatio(int sum1, int sum2, float &ratio) {

	/*Both more than 1*/
	if(sum1 <= 1 || sum2 <= 1)
		return false;

	/*One of them more than MIN_BALL_SUM*/
	if(sum1 < MIN_BALL_SUM && sum2 < MIN_BALL_SUM)
		return false;

	/*Get the maximal and cal the ratio*/
	int max = std::max(sum1, sum2);
	int min = std::min(sum1, sum2);

	ratio = (float)(min)/(float)max;

	/*Manage exceptions with small balls*/
	if(max==3)
		return min >= 2;

	/*Return true if the difference < MIN_THRESHOLD or the ratio is high*/
	return (ratio >= MIN_VALID_RATIO) || (min + MIN_THRESHOLD >= max);
}

void
BallDetector::getExpectedRadius(BallSample &ball)
{
	float average;
	HPoint2D points2D[6];
	HPoint3D* points3D[6];

	/*Calculate 3D point*/
	_Kinematics->get3DPositionZ(ball.p3D, ball.p2D, BALL_RADIUS);

	HPoint3D p1_3D = {ball.p3D.X + BALL_RADIUS, ball.p3D.Y + BALL_RADIUS, ball.p3D.Z, 1.0f};
	HPoint3D p2_3D = {ball.p3D.X + BALL_RADIUS, ball.p3D.Y - BALL_RADIUS, ball.p3D.Z, 1.0f};
	HPoint3D p3_3D = {ball.p3D.X - BALL_RADIUS, ball.p3D.Y + BALL_RADIUS, ball.p3D.Z, 1.0f};
	HPoint3D p4_3D = {ball.p3D.X - BALL_RADIUS, ball.p3D.Y - BALL_RADIUS, ball.p3D.Z, 1.0f};
	HPoint3D p5_3D = {ball.p3D.X, ball.p3D.Y, ball.p3D.Z + BALL_RADIUS, 1.0f};
	HPoint3D p6_3D = {ball.p3D.X, ball.p3D.Y, ball.p3D.Z - BALL_RADIUS, 1.0f};
	points3D[0] = &p1_3D;
	points3D[1] = &p2_3D;
	points3D[2] = &p3_3D;
	points3D[3] = &p4_3D;
	points3D[4] = &p5_3D;
	points3D[5] = &p6_3D;

	cvStartWriteSeq ( CV_32SC2, sizeof(CvSeq), sizeof(CvPoint), this->storage, &writer);

	/*Get the expected points in the image*/
	for (int i = 0; i < 6; i++) {
		_Kinematics->get2DPosition(*points3D[i], points2D[i]);
		CvPoint p = {points2D[i].x, points2D[i].y};
		CV_WRITE_SEQ_ELEM(p, writer);
	}

	/*Calculate width and height in image*/
	CvSeq* seq = cvEndWriteSeq( &writer );
	CvRect boundingBox = cvBoundingRect(seq);

	average = (boundingBox.width + boundingBox.height)/2;
	ball.radius_expected = average/2;
}

bool
BallDetector::checkRadius(BallSample &ball)
{
	float max, min;
	float ratio;

	if(ball.radius <= 0.0 || ball.radius_expected <= 0.0)
		return false;

	/*Get the maximal and cal the ratio*/
	if(ball.radius > ball.radius_expected) {
		max = ball.radius;
		min = ball.radius_expected;
	} else {
		max = ball.radius_expected;
		min = ball.radius;
	}

	ratio = min/max;

	/*Return true if the difference < MIN_THRESHOLD_RADIUS or ratio is high*/
	return (ratio >= MIN_VALID_RATIO_RADIUS) || (min + (float)MIN_THRESHOLD_RADIUS >= max);
}

void
BallDetector::clearDetected()
{
	if(!this->balls.empty())
	{
		this->balls.clear();
	}	
}

void
BallDetector::drawValidRegions(IplImage* src)
{
}

void
BallDetector::drawDetected(IplImage* src)
{
	CvPoint p;
	int aux;

	/*Draw balls*/
	for(list<BallSample>::iterator ball = this->balls.begin(); ball != this->balls.end(); ball++) {
		p.x  = (*ball).p2D.x;
		p.y  = (*ball).p2D.y;

		/*Expected radius*/
		aux = (int)((*ball).radius_expected*2.0 + 1.0)/2; /*Add 0.5 if the float is x.5*/
		cvCircle(src, p, (int) aux, CV_RGB(95,0,255), 1);		

		/*Calculated radius*/
		aux = (int)((*ball).radius*2.0 + 1.0)/2; /*Add 0.5 if the float is x.5*/
		cvCircle(src, p, (int) aux, CV_RGB(95,255,255), 1);		
	}
}

bool
BallDetector::isSeen()
{
	return this->balls.size()>0;//seen();
}



void
BallDetector::predict()
{
	ballmodel->predict();
}

void
BallDetector::updateFromObservation()
{
	list<BallSample>::iterator ball = this->balls.begin();

	lastBallTimestamp = getCurrentTime();

	list<AbstractSample> features;
	for (unsigned i = 0; i < this->balls.size(); i++ ) {
		BallSample z = (*ball);
		AbstractSample *as = dynamic_cast<BallSample*>(&z);
		features.push_back( *as );
		ball++;
	}

	ballmodel->updateFromObservation( features );
}

void
BallDetector::updateFromOdometry()
{
	// Calculate movement from last step
	float movX, movY, movA, currentBodyX, currentBodyY, currentBodyTh, dx, dy;

	body->getGlobalMovement(currentBodyX, currentBodyY, currentBodyTh);
	dx = currentBodyX - lastBodyX;
	dy = currentBodyY - lastBodyY;

	body->getRelativeMovement(lastBodyX, lastBodyY, lastBodyTh, movX, movY,  movA);

	// Update current values of movement
	lastBodyX = currentBodyX;
	lastBodyY = currentBodyY;
	lastBodyTh = currentBodyTh;
}


bica::ShapeList
BallDetector::getGrDebugAbs()
{
	shapeListAbs.clear();

	// Ground truth
	if (_gtReceived) {

		double ballX, ballY;
		getGTBallAbs( ballX, ballY );
		bica::Point3DPtr p(new bica::Point3D( (float)ballX, (float)ballY, 0.0f ));
		bica::CirclePtr c(new bica::Circle( bica::BLACK, true, "r", 125, "GTBall", p, 80.0f, 0.0f ));

		shapeListAbs.push_back(c);
	}

	return shapeListAbs;
}

bica::EllipsePtr ellipseFromFilter(const kalman::algorithm::MatrixCMPair& filter) {
	bica::Point3DPtr p(new bica::Point3D( filter.first.e(0,0), filter.first.e(1,0), 0.0f ));
	bica::EllipsePtr estEllipse(new bica::Ellipse);
	estEllipse->center = p;
	estEllipse->width =  std::max( 80., 4 * sqrt(filter.second.e(0,0)) );
	estEllipse->length = std::max( 80., 4 * sqrt(filter.second.e(1,1)) );
	estEllipse->angle =  toDegrees( atan2(filter.first.e(1,0), filter.first.e(0,0)) );
	estEllipse->z = 0.0f;
	estEllipse->color = bica::ORANGE;
	estEllipse->filled = true;
	estEllipse->opacity = 125;
	estEllipse->accKey = "o";
	estEllipse->label = "KalmanBall";
	return estEllipse;
}

bica::ShapeList
BallDetector::getGrDebugRel()
{
	shapeListRel.clear();

	// Center of the nets
	ShapeList auxList = ballmodel->getGrDebugRel();
	//Insert the GoalsModel's shape list into my shape list
	shapeListRel.insert(shapeListRel.end(), auxList.begin(), auxList.end());

	// Kalman ball
	shapeListRel.push_back( ellipseFromFilter(filter) );

	// Ground truth
	if (_gtReceived) {

		double ballX, ballY;
		getGTBallRel( ballX, ballY );
		bica::Point3DPtr p(new bica::Point3D( (float)ballX, (float)ballY, 0.0f ));
		bica::CirclePtr c(new bica::Circle( bica::BLACK, true, "r", 125, "GTBall", p, 80.0f, 0.0f ));

		if (isInsideOwnArea())
			c->color = bica::RED;
		else if (isInsideOppArea())
			c->color = bica::YELLOW;

		shapeListRel.push_back(c);
	}

	return shapeListRel;
}

VisualMemoryObjPtr
BallDetector::getVisualMemoryObject(const Ice::Current& c)
{
	VisualMemoryObjPtr ball = new VisualMemoryObj();
	ball->x = ballmodel->estimate.getPositionInRelativeCoordinates().x;
	ball->y = ballmodel->estimate.getPositionInRelativeCoordinates().y;

	ball->dx = 4 * sqrt(cvmGet(ballmodel->jpdaf->objects[0]->error_cov_post, 0, 0));
	ball->dy = 4 * sqrt(cvmGet(ballmodel->jpdaf->objects[0]->error_cov_post, 1, 1));

	ball->quality = ballmodel->estimate.getQuality();
	ball->time = ballmodel->elapsedTimeSinceLastObs;
	ball->reliability = ObjectState::reliability2string(ballmodel->estimate.getReliability());

	return ball;
}

void
BallDetector::predictionUpdate(const Ice::Current& c)
{
	measurement = false;
	perception->step();
	body->step();
	kinematics->step();
	this->detect(true);
	predict();
	updateFromOdometry();
}

void
BallDetector::measurementUpdate(const Ice::Current& c)
{
	measurement = true;
	perception->step();
	body->step();
	kinematics->step();
	this->detect(true);

	ballmodel->predict();
	ballmodel->updateFromOdometry();

	/*Save time*/
	if(this->balls.size() > 0)
	{
		updateFromObservation();
		this->lastSeen = this->getTime();
	}
}

void 
BallDetector::setGTBallAbs( double x, double y, const Ice::Current& c ) {
	setGTBallAbs(x, y);
}

void
BallDetector::setGTBallAbs( double x, double y )
{
	_gtReceived = true;
	this->_gtBallX = x;
	this->_gtBallY = y;
}

void
BallDetector::getGTBallAbs( double &x, double &y )
{
	x = _gtBallX;
	y = _gtBallY;
};

void
BallDetector::getGTBallRel( double &x, double &y )
{
	double gtRobotX, gtRobotY, gtRobotTh;

	//Convert to relative coordinates
	_GTLocalization->getPosition( gtRobotX, gtRobotY, gtRobotTh );

	Pose2D robotPose( gtRobotTh, gtRobotX, gtRobotY);
	Vector2<double> ballAbs(_gtBallX, _gtBallY);
	Vector2<double> ballRel = Pose2D::fieldCoord2Relative( robotPose, ballAbs );

	x = ballRel.x;
	y = ballRel.y;
};

bool
BallDetector::isInsideOwnArea()
{
	if ( (goalDetector->goals->p1NetCenterEstimate.getReliability() < ObjectState::HIGH_RELIABLE) ||
			(ballmodel->estimate.getReliability() < ObjectState::HIGH_RELIABLE) ) return false;

	Vector2<double> netCenter = goalDetector->goals->p1NetCenterEstimate.getPositionInRelativeCoordinates();
	double netAngle = goalDetector->goals->p1NetCenterEstimate.getAngle();
	Pose2D netPose( netAngle , netCenter.x, netCenter.y);
	Vector2<double> ball = ballmodel->estimate.getPositionInRelativeCoordinates();

	Vector2<double> ballRel = Pose2D::fieldCoord2Relative( netPose, ball );
	ballRel.x = -ballRel.x;
	ballRel.y = -ballRel.y;

	return ((ballRel.y < 1100) && (ballRel.y > -1100)) && ((ballRel.x < 600) && (ballRel.x >= 0));
}

bool
BallDetector::isInsideOppArea()
{
	if ( (goalDetector->goals->p0NetCenterEstimate.getReliability() < ObjectState::HIGH_RELIABLE) ||
			(ballmodel->estimate.getReliability() < ObjectState::HIGH_RELIABLE) ) return false;

	Vector2<double> netCenter = goalDetector->goals->p0NetCenterEstimate.getPositionInRelativeCoordinates();
	double netAngle = goalDetector->goals->p0NetCenterEstimate.getAngle();
	Pose2D netPose( netAngle , netCenter.x, netCenter.y);
	Vector2<double> ball = ballmodel->estimate.getPositionInRelativeCoordinates();

	Vector2<double> ballRel = Pose2D::fieldCoord2Relative( netPose, ball );
	ballRel.x = -ballRel.x;
	ballRel.y = -ballRel.y;

	return ((ballRel.y < 1100) && (ballRel.y > -1100)) && ((ballRel.x < 600) && (ballRel.x >= 0));
}
