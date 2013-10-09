/*
 * Name: GoalDetector.h
 * @Author: Eduardo Perdices (eperdices@gsyc.es)
 * 
 * Description: Class to detect goals from the GoalSegments
 *
 * Created on: 31/05/2010
 *
 * Copyright (C) Universidad Rey Juan Carlos
 * All Rights Reserved.
 *
 */

#include "GoalDetector.h"

const float GoalDetector::MIN_POST_DENSITY = 0.8;
const float GoalDetector::MIN_ACCEPTED_WIDTH_DENSITY = 0.6;
const float GoalDetector::POST_HEIGHT = 800.0;
const float GoalDetector::POST_RADIUS = 50.0;
const float GoalDetector::MAX_POST_DISTANCE = 7500.0;

GoalDetector::GoalDetector() : Detector(), GoalAttentionClient()
{
	_Horizon = Horizon::getInstance();
	_Segmentation = Segmentation::getInstance();
	_GTLocalization = GTLocalization::getInstance();

	this->color = ImageInput::CBLUE;
	this->color2 = ImageInput::CYELLOW;

	// Goals model
	goals = new GoalsModel();

	lastBodyX = lastBodyY = lastBodyTh = 0.0f;
	lastBallTimestamp = 0;
	_gtReceived = false;
}

GoalDetector::~GoalDetector()
{
}

void
GoalDetector::init(const string newName, AL::ALPtr<AL::ALBroker> parentBroker) {
	Component::init(newName, parentBroker);
	perception = Perception::getInstance();
	body = Body::getInstance();
	kinematics =  Kinematics::getInstance();

	this->setFreqTime(SHORT_RATE);

	//For attention
	initClient();

	goalVMlog = fopen((string("/tmp/vmGoal.log").c_str()), "w");
	vmIters = 0;
	vmMeanCycleTime = 0.0;
}

void
GoalDetector::step()
{
	perception->step();
	kinematics->step();
	body->step();

	if (!isTime2Run())
		return;

	startDebugInfo();
	this->detect(true);

	//goals->predict();

	ostringstream s;
	long beginVMcycleTime = getCurrentTime();
	goals->updateFromOdometry();

	list<AbstractSample> features;
	GoalPostSample z;
	GoalPostSample z1, z2;
	AbstractSample *as;
	AbstractSample *as1, *as2, *as3, *as4;

	switch(this->goal.type){
	case GOAL_LEFT_POST:
	case GOAL_RIGHT_POST:
		/*case GOAL_COMPLETE:
		//cout << "Observación no ambigua ";
		goals->updateFromObservation( goal );
		break;*/
	case GOAL_UNKNOWN_POST:
		//cout << "Observación ambigua de un solo poste desconocido\n";
		z = (*this->goal.post1);
		as = dynamic_cast<GoalPostSample*>(&z);
		features.push_back( *as );
		goals->updateFromObservation( goal.color, features );
		break;
	case GOAL_COMPLETE:

		//cout << "Observación ambigua completa\n";
		z1 = (*this->goal.post1);
		as1 = dynamic_cast<GoalPostSample*>(&z1);
		features.push_back( *as1 );

		z2 = (*this->goal.post2);
		as2 = dynamic_cast<GoalPostSample*>(&z2);
		features.push_back( *as2 );

		// Generate virtual observations for the opposite goal
		//cout << "GoalDetector::step() Generating two virtual opposite posts\n";
		HPoint3D vp0, vp1;
		goals->generateVirtualNet(z1.p3D, z2.p3D, vp0, vp1);
		GoalPostSample z3, z4;
		z3.p3D.X = vp0.X;
		z3.p3D.Y = vp0.Y;
		z4.p3D.X = vp1.X;
		z4.p3D.Y = vp1.Y;
		as3 = dynamic_cast<GoalPostSample*>(&z3);
		features.push_back( *as3 );
		as4 = dynamic_cast<GoalPostSample*>(&z4);
		features.push_back( *as4 );

		goals->updateFromObservation( goal.color, features );
		break;
	}

	long endVMcycleTime = getCurrentTime() - beginVMcycleTime;
	vmIters++;
	vmMeanCycleTime = ((vmMeanCycleTime * (vmIters - 1)) + endVMcycleTime) / (vmIters);
	s << "[" << "Goal Visual Memory" << "] Mean cycle time of " << vmMeanCycleTime / 1000.0 << " ms.\n";
	if(goalVMlog != NULL)
	{
		fprintf(goalVMlog, "%s", s.str().c_str());
		fflush(goalVMlog);
	}

	/*Save time*/
	if(seen())
	{
		HPoint2D in;

		in.x = ((getObjX() + 1.0)/2.0) * ImageInput::IMG_WIDTH;
		in.y = ((getObjY() + 1.0)/2.0) * ImageInput::IMG_HEIGHT;
		in.h = 1.0;

		/*switch(this->goal.type){
		case GOAL_LEFT_POST:
		case GOAL_RIGHT_POST:
		case GOAL_UNKNOWN_POST:
			GoalPostSample *auxp;
			if(this->goal.post1->valid)
				auxp = this->goal.post1;
			else
				auxp = this->goal.post1;

			in.x = (auxp->maxx+auxp->minx)/2;
			in.y = auxp->maxy;//(auxp->maxy+auxp->miny)/2;
			break;
		case GOAL_COMPLETE:
			in.x = (this->goal.post1->maxx+this->goal.post1->minx+this->goal.post2->maxx+this->goal.post2->minx)/4;
			in.y = (this->goal.post1->maxy+this->goal.post2->maxy)/2;//(this->goal.post1->maxy+this->goal.post1->miny+this->goal.post2->maxy+this->goal.post2->miny)/4;//(this->goal.post1->maxy+this->goal.post2->maxy)/2;
			break;
		default:
			cerr<<"Goal really not found!!"<<endl;
			//in.x = (this->goal.post1->maxx+this->goal.post1->minx+this->goal.post2->maxx+this->goal.post2->minx)/4;
			//in.y = (this->goal.post1->maxy+this->goal.post1->miny+this->goal.post2->maxy+this->goal.post2->miny)/4;//(this->goal.post1->maxy+this->goal.post2->maxy)/2;
			valid = false;
		};*/

		if(this->goal.type != GOAL_NOT_FOUND)
		{
			kinematics->get3DPositionZ(lastposp3d, in, 0.0);

			/*cerr<<"Goal found"<<endl;
			cerr<<"\tcolor: "<<this->goal.color<<endl;
			cerr<<"\ttype: "<<this->goal.type<<endl;
			cerr<<"\tGoal ("<<in.x<<", "<<in.y<<") at ("<<lastposp3d.X<<", "<<lastposp3d.Y<<")"<<endl;*/
			this->lastSeen = this->getTime();
		}
	}

	// Log
	beginLog();

	double GTownNetX, GTownNetY;
	getGTOwnNetCenter( GTownNetX, GTownNetY );
	double GToppNetX, GToppNetY;
	getGTOppNetCenter( GToppNetX, GToppNetY );
	double GTownNetLeftX, GTownNetLeftY;
	getGTOwnNetLeftPost( GTownNetLeftX, GTownNetLeftY );
	double GTownNetRightX, GTownNetRightY;
	getGTOwnNetRightPost( GTownNetRightX, GTownNetRightY );
	double GToppNetLeftX, GToppNetLeftY;
	getGTOppNetLeftPost( GToppNetLeftX, GToppNetLeftY );
	double GToppNetRightX, GToppNetRightY;
	getGTOppNetRightPost( GToppNetRightX, GToppNetRightY );

	//if ( _gtReceived ) {
	// Ground truth opponent net
	addToLog ( "GToppNetX", ComponentSnapshot::LOG_DOUBLE, GToppNetX );
	addToLog ( "GToppNetY", ComponentSnapshot::LOG_DOUBLE, GToppNetY );
	addToLog ( "GToppNetLeftX", ComponentSnapshot::LOG_DOUBLE, GToppNetLeftX );
	addToLog ( "GToppNetLeftY", ComponentSnapshot::LOG_DOUBLE, GToppNetLeftY );
	addToLog ( "GToppNetRightX", ComponentSnapshot::LOG_DOUBLE, GToppNetRightX );
	addToLog ( "GToppNetRightY", ComponentSnapshot::LOG_DOUBLE, GToppNetRightY );

	// Ground truth own net
	addToLog ( "GTownNetX", ComponentSnapshot::LOG_DOUBLE, GTownNetX );
	addToLog ( "GTownNetY", ComponentSnapshot::LOG_DOUBLE, GTownNetY );
	addToLog ( "GTownNetLeftX", ComponentSnapshot::LOG_DOUBLE, GTownNetLeftX );
	addToLog ( "GTownNetLeftY", ComponentSnapshot::LOG_DOUBLE, GTownNetLeftY );
	addToLog ( "GTownNetRightX", ComponentSnapshot::LOG_DOUBLE, GTownNetRightX );
	addToLog ( "GTownNetRightY", ComponentSnapshot::LOG_DOUBLE, GTownNetRightY );
	//}

	// P0 net
	addToLog ( "P0NetCenterX", ComponentSnapshot::LOG_DOUBLE, goals->p0NetCenterEstimate.getPositionInRelativeCoordinates().x );
	addToLog ( "p0NetCenterY", ComponentSnapshot::LOG_DOUBLE, goals->p0NetCenterEstimate.getPositionInRelativeCoordinates().y );
	double p0NetQuality = (double)(goals->p0NetCenterEstimate.getQuality());
	addToLog ( "p0NetQuality", ComponentSnapshot::LOG_DOUBLE, p0NetQuality );

	addToLog ( "p0NetLeftX", ComponentSnapshot::LOG_DOUBLE, goals->p0LeftEstimate.getPositionInRelativeCoordinates().x );
	addToLog ( "p0NetLeftY", ComponentSnapshot::LOG_DOUBLE, goals->p0LeftEstimate.getPositionInRelativeCoordinates().y );
	double p0LeftQuality = (double)(goals->p0LeftEstimate.getQuality());
	addToLog ( "p0LeftQuality", ComponentSnapshot::LOG_DOUBLE, p0LeftQuality );
	addToLog ( "p0NetRightX", ComponentSnapshot::LOG_DOUBLE, goals->p0RightEstimate.getPositionInRelativeCoordinates().x );
	addToLog ( "p0NetRightY", ComponentSnapshot::LOG_DOUBLE, goals->p0RightEstimate.getPositionInRelativeCoordinates().y );
	double p0RightQuality = goals->p0RightEstimate.getQuality();
	addToLog ( "p0RightQuality", ComponentSnapshot::LOG_DOUBLE, p0RightQuality );
	addToLog ( "p0LeftXcov", ComponentSnapshot::LOG_DOUBLE, cvmGet(goals->yellowJPDAF->objects[goals->p0l]->error_cov_post, 0, 0) );
	addToLog ( "p0LeftYcov", ComponentSnapshot::LOG_DOUBLE, cvmGet(goals->yellowJPDAF->objects[goals->p0l]->error_cov_post, 1, 1) );
	addToLog ( "p0RightXcov", ComponentSnapshot::LOG_DOUBLE, cvmGet(goals->yellowJPDAF->objects[goals->p0r]->error_cov_post, 0, 0) );
	addToLog ( "p0RightYcov", ComponentSnapshot::LOG_DOUBLE, cvmGet(goals->yellowJPDAF->objects[goals->p0r]->error_cov_post, 1, 1) );


	// p1 net
	addToLog ( "P1NetCenterX", ComponentSnapshot::LOG_DOUBLE, goals->p1NetCenterEstimate.getPositionInRelativeCoordinates().x );
	addToLog ( "p1NetCenterY", ComponentSnapshot::LOG_DOUBLE, goals->p1NetCenterEstimate.getPositionInRelativeCoordinates().y );
	double p1NetQuality = (double)(goals->p1NetCenterEstimate.getQuality());
	addToLog ( "p1NetQuality", ComponentSnapshot::LOG_DOUBLE, p1NetQuality );

	addToLog ( "p1NetLeftX", ComponentSnapshot::LOG_DOUBLE, goals->p1LeftEstimate.getPositionInRelativeCoordinates().x );
	addToLog ( "p1NetLeftY", ComponentSnapshot::LOG_DOUBLE, goals->p1LeftEstimate.getPositionInRelativeCoordinates().y );
	double p1LeftQuality = (double)(goals->p1LeftEstimate.getQuality());
	addToLog ( "p1LeftQuality", ComponentSnapshot::LOG_DOUBLE, p1LeftQuality );
	addToLog ( "p1NetRightX", ComponentSnapshot::LOG_DOUBLE, goals->p1RightEstimate.getPositionInRelativeCoordinates().x );
	addToLog ( "p1NetRightY", ComponentSnapshot::LOG_DOUBLE, goals->p1RightEstimate.getPositionInRelativeCoordinates().y );
	double p1RightQuality = goals->p1RightEstimate.getQuality();
	addToLog ( "p1RightQuality", ComponentSnapshot::LOG_DOUBLE, p1RightQuality );
	addToLog ( "p1LeftXcov", ComponentSnapshot::LOG_DOUBLE, cvmGet(goals->yellowJPDAF->objects[goals->p1l]->error_cov_post, 0, 0) );
	addToLog ( "p1LeftYcov", ComponentSnapshot::LOG_DOUBLE, cvmGet(goals->yellowJPDAF->objects[goals->p1l]->error_cov_post, 1, 1) );
	addToLog ( "p1RightXcov", ComponentSnapshot::LOG_DOUBLE, cvmGet(goals->yellowJPDAF->objects[goals->p1r]->error_cov_post, 0, 0) );
	addToLog ( "p1RightYcov", ComponentSnapshot::LOG_DOUBLE, cvmGet(goals->yellowJPDAF->objects[goals->p1r]->error_cov_post, 1, 1) );

	endLog( getName() );

	updateAP();
	endDebugInfo();
}

void
GoalDetector::detect(bool debug)
{
	int typeb, typey;
	GoalPostSample * posts = NULL;

	/*Clear saved elements*/
	this->clearDetected();

	/*TODO BLUEGOAL: Skip blue goal*/
	typeb = GOAL_NOT_FOUND; //typeb = this->searchGoal(this->color, this->postsBlue);
	typey = this->searchGoal(this->color2, this->postsYellow);

	/*Check if we don't see any goals*/
	if(typeb == GOAL_NOT_FOUND && typey == GOAL_NOT_FOUND) {
		this->goal.found = false;
		this->goal.type = GOAL_NOT_FOUND;
		return;
	}

	/*Check if we see both goals, it can't happen*/
	if(typeb == GOAL_COMPLETE && typey == GOAL_COMPLETE) {
		this->goal.found = false;
		this->goal.type = GOAL_NOT_FOUND;
		return;
	}

	/*If we detect one complete goal, select it (even if we detect a single post of the other goal)*/
	if(typeb == GOAL_COMPLETE)
		posts = this->postsBlue;
	if(typey == GOAL_COMPLETE)
		posts = this->postsYellow;	

	if(posts == NULL) {
		/*If we see one unknown post of both goals, discart both*/
		if(typeb == GOAL_UNKNOWN_POST && typey == GOAL_UNKNOWN_POST) {
			this->goal.found = false;
			this->goal.type = GOAL_NOT_FOUND;
			return;
		}
		/*If we see one known post of both goals, discart both*/	
		if(	(typeb == GOAL_LEFT_POST || typeb == GOAL_RIGHT_POST) && 
				(typey == GOAL_LEFT_POST || typey == GOAL_RIGHT_POST)) {
			this->goal.found = false;
			this->goal.type = GOAL_NOT_FOUND;
			return;
		}	

		/*Select a goal with a single post*/
		if(typeb == GOAL_LEFT_POST || typeb == GOAL_RIGHT_POST)
			posts = this->postsBlue;
		else if(typey == GOAL_LEFT_POST || typey == GOAL_RIGHT_POST)
			posts = this->postsYellow;
		else if(typeb == GOAL_UNKNOWN_POST)
			posts = this->postsBlue;
		else if(typey == GOAL_UNKNOWN_POST)
			posts = this->postsYellow;
	}

	/*Save the goal selected*/
	if(posts != NULL) {
		/*If it's a complete goal*/
		if(posts[0].valid && posts[1].valid)
			this->saveGoal(&(posts[0]), &(posts[1]));
		else {
			/*If it's a single goal*/
			if(posts[0].valid)
				this->saveGoal(&(posts[0]),NULL);
			if(posts[1].valid)
				this->saveGoal(&(posts[1]),NULL);
		}
	}
}

int
GoalDetector::searchGoal(int color, GoalPostSample * posts)
{
	int i, k, col;
	int accumulator[ImageInput::IMG_WIDTH];
	list<Segment> * lsegments;
	GoalPostSample postSample;
	GoalPostSample postSample2;

	memset(accumulator, 0, sizeof(int)*ImageInput::IMG_WIDTH);	

	/*Hough transform to find lines, we guess the lines are vertical, so we just need to find the columns*/
	for(i=0;i<ImageInput::IMG_STEPS_H;i++) {
		/*Get the segments of the row*/
		lsegments = _RegionBuilder->getGoalHSegments(i);
		for(list<Segment>::iterator seg = lsegments->begin(); seg != lsegments->end(); seg++) {
			/*Save goal segments not too big*/
			if((*seg).color == color && (*seg).length < MAX_SEGMENT_SIZE_GOAL) {
				for(k=(*seg).x;k<=(*seg).xend;k++)
					accumulator[k]++;
			}
		}
	}

	/*Get the best column*/
	postSample.numAcc = 0;
	for(col=0;col<ImageInput::IMG_WIDTH;col++) {
		if(accumulator[col] > postSample.numAcc && accumulator[col] >= MIN_GOAL_ACCS) {
			postSample.column = col;
			postSample.numAcc = accumulator[col];
		}			
	}

	if(postSample.numAcc > 0) {
		/*Save the post and validate*/
		postSample.miny = ImageInput::IMG_HEIGHT - 1;
		postSample.maxy = 0;
		postSample.color = color;
		postSample.valid = this->checkGoalPost(postSample);
		posts[0] = postSample;

		/*Get the second best column, far away from the first one*/
		postSample2.numAcc = 0;
		for(col=0;col<ImageInput::IMG_WIDTH;col++) {
			if(accumulator[col] > postSample2.numAcc && accumulator[col] >= MIN_GOAL_ACCS) {
				/*Distance to the best post > threshold*/
				if(abs(col-postSample.column) > MIN_POSTS_DISTANCE) {
					postSample2.column = col;
					postSample2.numAcc = accumulator[col];
				}
			}			
		}

		if(postSample2.numAcc > 0) {
			/*Save the second column and validate*/
			postSample2.miny = ImageInput::IMG_HEIGHT - 1;
			postSample2.maxy = 0;
			postSample2.color = color;
			postSample2.valid = this->checkGoalPost(postSample2);	
			posts[1] = postSample2;
		}
	}

	/*If we can see both posts*/
	if(posts[0].valid && posts[1].valid) {

		if(posts[0].column < posts[1].column) {
			posts[0].type = GOAL_LEFT_POST;
			posts[1].type = GOAL_RIGHT_POST;
		} else {
			posts[0].type = GOAL_RIGHT_POST;
			posts[1].type = GOAL_LEFT_POST;
		}

		return GOAL_COMPLETE;
	}

	/*If we can see only one post*/
	if(posts[0].valid) {
		posts[0].type = this->hasCrossbar(posts[0]);
		return posts[0].type;
	}

	if(posts[1].valid) {
		posts[1].type = this->hasCrossbar(posts[1]);
		return posts[1].type;
	}

	return GOAL_NOT_FOUND;
}

bool
GoalDetector::checkGoalPost(GoalPostSample &post)
{
	int i;
	bool foundLast = false;
	bool hasRowSegment = false;
	int numNotFound = 0;
	float density;
	Point2D pextreme;
	int expWidth, maxWidth, minWidth;
	int expHeight, realHeight, maxHeight, minHeight;
	int numAccepted;
	list<Segment*> postSegments;
	list<Segment> * lsegments;

	/*Find the segments and the extremes of the post, from bottom to top to avoid false positives*/
	for(i=ImageInput::IMG_STEPS_H-1;i>=0;i--) {
		hasRowSegment = false;

		/*Get the segments of the row*/
		lsegments = _RegionBuilder->getGoalHSegments(i);
		for(list<Segment>::iterator seg = lsegments->begin(); seg != lsegments->end(); seg++) {

			/*If segment is part of the post save it*/
			if((*seg).x <= post.column && (*seg).xend >= post.column) {

				if(!foundLast) {
					/*Save the last row of the post*/
					post.maxy = (*seg).y;
					foundLast = true;
				} 

				/*Save the first row of the post (at this moment, we go from bottom to top)*/
				post.miny = (*seg).y;
				postSegments.push_back(&(*seg));
				hasRowSegment = true;		
			}
		}

		/*If we have already found a segment but we don't find a segment in X rows, end search*/
		if(foundLast) {
			if(!hasRowSegment) {
				numNotFound++;
				if(numNotFound >= MAX_ROWS_WITHOUT_POST) {
					break;
				}
			}
		}
	}

	/*Extremes not together*/
	if(post.maxy == post.miny)
		return false;

	/*Check the density of the segments*/
	density = (float)(post.maxy - post.miny)/(float)ImageInput::IMG_STEP;
	density = (float)(postSegments.size()-1.0)/density;
	if(density < MIN_POST_DENSITY)
		return false;

	/*Calc the extremes of the posts with more precision*/
	_ImageHandler->scanPostExtremeInImage(post.column, post.maxy, post.maxx, post.maxy, post.color, true);
	_ImageHandler->scanPostExtremeInImage(post.column, post.miny, post.minx, post.miny, post.color, false);

	/*Check if we can detect the real extremes of the posts*/
	if(post.maxy <= ImageInput::IMG_HEIGHT - 1 - TOP_POST_FOUND_MARGIN)
		post.maxFound = true;
	else
		post.maxFound = false;

	if(post.miny >= TOP_POST_FOUND_MARGIN)
		post.minFound = true;
	else
		post.minFound = false;

	/*If we can't see at least one of the extremes, it's not valid*/
	if(!post.maxFound && !post.minFound)
		return false;

	/*The bottom extreme has to be under the field border*/
	if(post.maxFound) {
		pextreme.x = post.maxx;
		pextreme.y = post.maxy;

		if(_Horizon->distance2horizon(pextreme) > NET_MIN_DIST_BASE_TO_HORIZON)
			return false;
	}

	/*Check the distance to the post*/
	this->calcPostPosition(post);
	if(calcDistance2D((int)post.p3D.X, (int)post.p3D.Y, 0, 0) > MAX_POST_DISTANCE)
		return false;

	/*Get the expected width of the post*/
	expWidth = this->getExpectedPostWidth(post);
	if(expWidth < 0)
		return false;

	/*Number of segments with expected width*/
	numAccepted = 0;
	maxWidth = expWidth * 1.66 + 2;
	minWidth = expWidth * 0.6 - 2;
	for(list<Segment*>::iterator seg = postSegments.begin(); seg != postSegments.end(); seg++) {
		if((*seg)->length >= minWidth && (*seg)->length <= maxWidth) {
			numAccepted++;
		}			
	}
	if(postSegments.size() > 0)
		density = (float)numAccepted/(float)postSegments.size();
	else
		density = 0;
	if(density < MIN_ACCEPTED_WIDTH_DENSITY)
		return false;

	/*Get the expected height of the post*/
	if(post.maxFound && post.minFound) {
		expHeight = this->getExpectedPostHeight(post);
		if(expHeight < 0)
			return false;

		maxHeight = expHeight * 1.33;
		minHeight = expHeight * 0.75;
		realHeight = calcDistance2D(post.minx, post.miny, post.maxx, post.maxy);
		if(realHeight < minHeight || realHeight > maxHeight)
			return false;
	}

	/*Check if the post has green under it*/
	if(!this->hasPostGreen(post, expWidth))
		return false;

	return true;
}

void
GoalDetector::calcPostPosition(GoalPostSample &post)
{
	HPoint2D post2D;
	float zpoint;

	if(post.maxFound) {
		post2D.x = post.maxx;
		post2D.y = post.maxy;
		post2D.h = 1.0;
		zpoint = 0.0;		
	} else if(post.minFound) {
		post2D.x = post.minx;
		post2D.y = post.miny;
		post2D.h = 1.0;
		zpoint = POST_HEIGHT;		
	} else
		return;

	/*Obtain the position in 3D of the extreme*/
	_Kinematics->get3DPositionZ(post.p3D, post2D, zpoint);
}

int
GoalDetector::getExpectedPostWidth(GoalPostSample &post)
{
	HPoint2D right2D, left2D;
	HPoint3D right3D, left3D;

	/*Set the width of the post in 3D*/
	right3D = post.p3D;
	right3D.Y -= POST_RADIUS;
	left3D = post.p3D;
	left3D.Y += POST_RADIUS;

	_Kinematics->get2DPosition(right3D, right2D);
	_Kinematics->get2DPosition(left3D, left2D);

	/*Obtain the expected width of the post in the image*/
	return (int) fabs(right2D.x - left2D.x); 
}

int
GoalDetector::getExpectedPostHeight(GoalPostSample &post)
{
	HPoint2D up2D;
	HPoint3D up3D;

	/*Set the height of the post in 3D*/
	up3D = post.p3D;
	up3D.Z = POST_HEIGHT;

	_Kinematics->get2DPosition(up3D, up2D);

	/*Obtain the expected width of the post in the image*/
	return (int) calcDistance2D((int)up2D.x, (int)up2D.y, post.maxx, post.maxy);
}

int
GoalDetector::hasPostGreen(GoalPostSample &post, int expWidth)
{
	list<Segment> * segments;
	int col, index;
	bool found;
	int marginx = ImageInput::IMG_STEP/2;
	int greenPos = ImageInput::IMG_HEIGHT - 1;
	int dist;

	/*Check 3 columns, right, center and left*/
	for(col=post.maxx-expWidth;col<=post.maxx+expWidth;col+=expWidth) {

		if(col<0 || col>= ImageInput::IMG_WIDTH)
			continue;

		index = (col+marginx)/ImageInput::IMG_STEP - 1;
		if(index < 0) index = 0;
		if(index >= ImageInput::IMG_STEPS_V) index = ImageInput::IMG_STEPS_V-1;


		found = false;
		segments = _Segmentation->getSegments(index);

		/*Search the first segment green*/
		list<Segment>::iterator seg = segments->begin();
		while( seg != segments->end() && !found) {
			if((*seg).color == ImageInput::CGREEN) {
				/*If the green is after the post (with a margin), save the position*/
				if((*seg).y >= post.maxy - MAX_GREEN_POST_DIST) {
					if((*seg).y < greenPos)
						greenPos = (*seg).y;
					found = true;
				}
			}
			seg++;
		}		
	}

	/*Calc the distance*/
	dist = greenPos - post.maxy;

	return dist < MAX_GREEN_POST_DIST;
}

int
GoalDetector::hasCrossbar(GoalPostSample &post)
{
	int i, left = 0, right = 0;
	int distImg, maxy, miny;
	list<Segment> * lsegments;

	if(!post.minFound)
		return GOAL_UNKNOWN_POST;

	/*TODO: Ver si se puede deducir aunque no se vea el extremo superior claramente*/

	/*Check the distance to the closest border of the image. Avoid checking more columns on one side than on the other*/
	distImg = ImageInput::IMG_WIDTH/2;
	if(post.column < distImg)
		distImg = post.column;
	else
		distImg = ImageInput::IMG_WIDTH - post.column - 1;

	/*If the distance is not going to be enough, exit without checking*/
	if(distImg < CROSSBAR_WIDTH_JUMPS*ImageInput::IMG_STEP)
		return GOAL_UNKNOWN_POST;

	miny = post.miny - CROSSBAR_MARGIN_Y;
	maxy = post.miny + CROSSBAR_MARGIN_Y;
	if(miny < 0) miny = 0;
	if(maxy >= ImageInput::IMG_HEIGHT) maxy = ImageInput::IMG_HEIGHT - 1;

	/*Check the vertical segments*/
	for(i=0;i<ImageInput::IMG_STEPS_V;i++) {

		/*Get the segments of the column*/
		lsegments = _RegionBuilder->getGoalVSegments(i);
		for(list<Segment>::iterator seg = lsegments->begin(); seg != lsegments->end(); seg++) {

			/*Find segments not too big near the X column*/
			if(	(*seg).color == color && (*seg).length < MAX_CROSSBAR_SEGMENT_SIZE && abs((*seg).x - post.minx) <= distImg) {

				/*Check the height of the segment*/
				if(((*seg).y >= miny && (*seg).y <= maxy) || ((*seg).yend >= miny && (*seg).yend <= maxy)) {

					/*Check the side*/
					if((*seg).x > post.minx)
						right++;
					else
						left++;				

				}
			}
		}
	}

	/*If the difference is big between the left and the right, save the side*/
	if(abs(right - left) > CROSSBAR_WIDTH_JUMPS) {
		if(right > left)
			return GOAL_LEFT_POST;
		else
			return GOAL_RIGHT_POST;
	}

	/*TODO: Comprobar pequeños trozos de larguero cuando no llegue a CROSSBAR_WIDTH_JUMPS*/

	return GOAL_UNKNOWN_POST;
}

void
GoalDetector::clearDetected()
{

	this->postsBlue[0].valid = false;
	this->postsBlue[1].valid = false;

	this->postsYellow[0].valid = false;
	this->postsYellow[1].valid = false;

	this->goal.found = false;
	this->goal.type = GOAL_NOT_FOUND;
}

void
GoalDetector::saveGoal(GoalPostSample * post1, GoalPostSample * post2)
{
	if(post1 == NULL && post2 == NULL) {
		this->goal.found = false;
		this->goal.type = GOAL_NOT_FOUND;
		return;
	}

	/*Complete goal*/
	if(post1 != NULL && post2 != NULL) {
		this->goal.post1 = post1;
		this->goal.post2 = post2;
		this->goal.p3D.X = (post1->p3D.X + post2->p3D.X)/2.0;
		this->goal.p3D.Y = (post1->p3D.Y + post2->p3D.Y)/2.0;
		this->goal.p3D.Z = 0.0;
		this->goal.p3D.H = 1.0;
		this->goal.type = GOAL_COMPLETE;
		this->goal.color = post1->color;
		this->goal.found = true;
		return;
	}	

	/*Just one post*/
	if(post1 != NULL) {
		this->goal.post1 = post1;
		this->goal.post2 = NULL;
		this->goal.p3D = post1->p3D;
		this->goal.type = post1->type;
		this->goal.color = post1->color;
		this->goal.found = true;
		return;
	}
}

void
GoalDetector::drawValidRegions(IplImage* src)
{
}

void
GoalDetector::drawDetected(IplImage* src)
{
	CvPoint p, q;
	int col;

	if(this->goal.found) {



	}

	/*Draw blue goal posts*/
	for(col=0;col<2;col++) {
		if(!this->postsBlue[col].valid)
			continue;

		p.x  = this->postsBlue[col].minx;
		p.y  = this->postsBlue[col].miny;
		q.x  = this->postsBlue[col].maxx;
		q.y  = this->postsBlue[col].maxy;
		if(this->postsBlue[col].color == this->color) {
			cvLine(src, p, q, CV_RGB(0,255,255), 1, CV_AA, 0);
		}
		if(this->postsBlue[col].color == this->color2) {
			cvLine(src, p, q, CV_RGB(255,255,0), 1, CV_AA, 0);
		}
	}

	/*Draw yellow goal posts*/
	for(col=0;col<2;col++) {
		if(!this->postsYellow[col].valid)
			continue;

		p.x  = this->postsYellow[col].minx;
		p.y  = this->postsYellow[col].miny;
		q.x  = this->postsYellow[col].maxx;
		q.y  = this->postsYellow[col].maxy;
		if(this->postsYellow[col].color == this->color) {
			cvLine(src, p, q, CV_RGB(0,255,255), 1, CV_AA, 0);
		}
		if(this->postsYellow[col].color == this->color2) {
			cvLine(src, p, q, CV_RGB(255,255,0), 1, CV_AA, 0);
		}
	}
}

bool
GoalDetector::isSeen()
{
	return this->goal.found && (this->goal.type != GOAL_NOT_FOUND);//seen();
}



float
GoalDetector::getObjX()
{

	if(isSeen())
	{
		float x;
		GoalPostSample *auxp;
		if(this->goal.post1->valid)
			auxp = this->goal.post1;
		else
			auxp = this->goal.post2;

		switch(this->goal.type){
		case GOAL_LEFT_POST:
			x = (auxp->maxx+auxp->minx)/2 + ImageInput::IMG_WIDTH / 3;
			break;
		case GOAL_RIGHT_POST:
			x = (auxp->maxx+auxp->minx)/2 - ImageInput::IMG_WIDTH / 3;
			break;
		case GOAL_UNKNOWN_POST:
			x = (auxp->maxx+auxp->minx)/2;

			break;
		case GOAL_COMPLETE:
			x = (this->goal.post1->maxx+this->goal.post1->minx+this->goal.post2->maxx+this->goal.post2->minx)/4;
			break;
		default:
			cerr<<"Goal really not found!!"<<endl;
			x = 0.0;
		};

		return 2.0*((x - ImageInput::IMG_WIDTH/2.0)/(ImageInput::IMG_WIDTH));
	}else
		return 1000.0; //Para que no se bloquee
}

float
GoalDetector::getObjY()
{
	if(isSeen())
	{
		float y;

		switch(this->goal.type){
		case GOAL_LEFT_POST:
		case GOAL_RIGHT_POST:
		case GOAL_UNKNOWN_POST:
			GoalPostSample *auxp;
			if(this->goal.post1->valid)
				auxp = this->goal.post1;
			else
				auxp = this->goal.post2;

			//if(maxy<ImageInput::IMG_HEIGHT)

			//y = auxp->maxy-(ImageInput::IMG_HEIGHT-auxp->maxy+20);
			y = auxp->maxy;//(auxp->maxy + auxp->miny)/2;//(this->goal.post1->maxy+this->goal.post2->maxy)/2;

			break;
		case GOAL_COMPLETE:
			y = (this->goal.post1->maxy+this->goal.post2->maxy)/2;// -(ImageInput::IMG_HEIGHT-(this->goal.post1->maxy+this->goal.post2->maxy)/2+20);
			//y = (this->goal.post1->maxy+this->goal.post1->miny+this->goal.post2->maxy+this->goal.post2->miny)/4;//(this->goal.post1->maxy+this->goal.post2->maxy)/2;
			break;
		default:
			cerr<<"Goal really not found!!"<<endl;
			y = 0.0;
		};

		return 2.0*((y - ImageInput::IMG_HEIGHT/2.0)/(ImageInput::IMG_HEIGHT));
	}else
		return 0.0;
}

bica::ShapeList
GoalDetector::getGrDebugRel()
{
	shapeListRel.clear();

	// Center of the nets
	ShapeList auxList = goals->getGrDebugRel();
	//Insert the GoalsModel's shape list into my shape list
	shapeListRel.insert(shapeListRel.end(), auxList.begin(), auxList.end());

	// Ground truth
	if (_gtReceived) {

		double GTownNetX, GTownNetY;
		getGTOwnNetCenter( GTownNetX, GTownNetY );
		bica::Point3DPtr p1(new bica::Point3D( (float)GTownNetX, (float)GTownNetY, 0.0f ));
		bica::CirclePtr c1(new bica::Circle( bica::BLACK, true, "r", 125, "", p1, 50.0f, 0.0f ));
		shapeListRel.push_back(c1);

		double GToppNetX, GToppNetY;
		getGTOppNetCenter( GToppNetX, GToppNetY );
		bica::Point3DPtr p2(new bica::Point3D( (float)GToppNetX, (float)GToppNetY, 0.0f ));
		bica::CirclePtr c2(new bica::Circle( bica::BLACK, true, "r", 125, "", p2, 50.0f, 0.0f ));
		shapeListRel.push_back(c2);

		double GTownNetLeftX, GTownNetLeftY;
		getGTOwnNetLeftPost( GTownNetLeftX, GTownNetLeftY );
		bica::Point3DPtr p3(new bica::Point3D( (float)GTownNetLeftX, (float)GTownNetLeftY, 0.0f ));
		bica::CirclePtr c3(new bica::Circle( bica::BLACK, true, "r", 125, "", p3, 50.0f, 0.0f ));
		shapeListRel.push_back(c3);

		double GTownNetRightX, GTownNetRightY;
		getGTOwnNetRightPost( GTownNetRightX, GTownNetRightY );
		bica::Point3DPtr p4(new bica::Point3D( (float)GTownNetRightX, (float)GTownNetRightY, 0.0f ));
		bica::CirclePtr c4(new bica::Circle( bica::BLACK, true, "r", 125, "", p4, 50.0f, 0.0f ));
		shapeListRel.push_back(c4);

		double GToppNetLeftX, GToppNetLeftY;
		getGTOppNetLeftPost( GToppNetLeftX, GToppNetLeftY );
		bica::Point3DPtr p5(new bica::Point3D( (float)GToppNetLeftX, (float)GToppNetLeftY, 0.0f ));
		bica::CirclePtr c5(new bica::Circle( bica::BLACK, true, "r", 125, "", p5, 50.0f, 0.0f ));
		shapeListRel.push_back(c5);

		double GToppNetRightX, GToppNetRightY;
		getGTOppNetRightPost( GToppNetRightX, GToppNetRightY );
		bica::Point3DPtr p6(new bica::Point3D( (float)GToppNetRightX, (float)GToppNetRightY, 0.0f ));
		bica::CirclePtr c6(new bica::Circle( bica::BLACK, true, "r", 125, "", p6, 50.0f, 0.0f ));
		shapeListRel.push_back(c6);
	}

	return shapeListRel;
}

VisualMemoryObjPtr
GoalDetector::getVisualMemoryObject(const string &obj, const Ice::Current& c)
{
	int i;
	VisualMemoryObjPtr object = new VisualMemoryObj();

	if (obj == "p0LeftPost") {
		object->x = goals->p0LeftEstimate.getPositionInRelativeCoordinates().x;
		object->y = goals->p0LeftEstimate.getPositionInRelativeCoordinates().y;
		/*if (goals->p0LeftEstimate == &(goals->opponentP0Estimate)){
			i = 0;
		} else {
			i = 1;
		}*/
		object->dx = 4 * sqrt(cvmGet(goals->yellowJPDAF->objects[goals->p0l]->error_cov_post, 0, 0));
		object->dy = 4 * sqrt(cvmGet(goals->yellowJPDAF->objects[goals->p0l]->error_cov_post, 1, 1));
		object->quality = goals->p0LeftEstimate.getQuality();
		object->time = goals->p0ElapsedTimeSinceLastObs;
		object->reliability = goals->p0LeftEstimate.getReliabilityString();
	} else if (obj == "p0RightPost") {
		object->x = goals->p0RightEstimate.getPositionInRelativeCoordinates().x;
		object->y = goals->p0RightEstimate.getPositionInRelativeCoordinates().y;
		/*if (goals->opponentRightEstimate == &(goals->opponentP0Estimate)){
			i = 0;
		} else {
			i = 1;
		}*/
		object->dx = 4 * sqrt(cvmGet(goals->yellowJPDAF->objects[goals->p0r]->error_cov_post, 0, 0));
		object->dy = 4 * sqrt(cvmGet(goals->yellowJPDAF->objects[goals->p0r]->error_cov_post, 1, 1));
		object->quality = goals->p0RightEstimate.getQuality();
		object->time = goals->p0ElapsedTimeSinceLastObs;
		object->reliability = goals->p0RightEstimate.getReliabilityString();
	}else if (obj == "p0Net") {
		object->x = goals->p0NetCenterEstimate.getPositionInRelativeCoordinates().x;
		object->y = goals->p0NetCenterEstimate.getPositionInRelativeCoordinates().y;
		object->dx = 0;
		object->dy = 0;
		object->quality = goals->p0NetCenterEstimate.getQuality();
		object->time = goals->p0ElapsedTimeSinceLastObs;
		object->reliability = goals->p0NetCenterEstimate.getReliabilityString();
	} else if (obj == "p1LeftPost") {
		object->x = goals->p1LeftEstimate.getPositionInRelativeCoordinates().x;
		object->y = goals->p1LeftEstimate.getPositionInRelativeCoordinates().y;
		/*if (goals->ownLeftEstimate == &(goals->ownP0Estimate)){
			i = 0;
		} else {
			i = 1;
		}*/
		object->dx = 4 * sqrt(cvmGet(goals->yellowJPDAF->objects[goals->p1l]->error_cov_post, 0, 0));
		object->dy = 4 * sqrt(cvmGet(goals->yellowJPDAF->objects[goals->p1l]->error_cov_post, 1, 1));
		object->quality = goals->p1LeftEstimate.getQuality();
		object->time = goals->p1ElapsedTimeSinceLastObs;
		object->reliability = goals->p1LeftEstimate.getReliabilityString();
	} else if (obj == "p1RightPost") {
		object->x = goals->p1RightEstimate.getPositionInRelativeCoordinates().x;
		object->y = goals->p1RightEstimate.getPositionInRelativeCoordinates().y;
		/*if (goals->ownRightEstimate == &(goals->ownP0Estimate)){
			i = 0;
		} else {
			i = 1;
		}*/
		object->dx = 4 * sqrt(cvmGet(goals->yellowJPDAF->objects[goals->p1r]->error_cov_post, 0, 0));
		object->dy = 4 * sqrt(cvmGet(goals->yellowJPDAF->objects[goals->p1r]->error_cov_post, 1, 1));
		object->quality = goals->p1RightEstimate.getQuality();
		object->time = goals->p1ElapsedTimeSinceLastObs;
		object->reliability = goals->p1RightEstimate.getReliabilityString();
	} else if (obj == "p1Net") {
		object->x = goals->p1NetCenterEstimate.getPositionInRelativeCoordinates().x;
		object->y = goals->p1NetCenterEstimate.getPositionInRelativeCoordinates().y;
		object->dx = 0;
		object->dy = 0;
		object->quality = goals->p1NetCenterEstimate.getQuality();
		object->time = goals->p1ElapsedTimeSinceLastObs;
		object->reliability = goals->p1NetCenterEstimate.getReliabilityString();
	} else {
		cerr << "GoalDetector::getVisualMemoryObject() Incorrect object (" << obj << ")\n";
	}

	return object;
}

void
GoalDetector::predictionUpdate(const Ice::Current& c)
{
	perception->step();
	kinematics->step();
	body->step();

	goals->predict();
}

void
GoalDetector::measurementUpdate(const Ice::Current& c)
{
	perception->step();
	kinematics->step();
	body->step();
	this->detect(true);

	//goals->predict();
	goals->updateFromOdometry();

	list<AbstractSample> features;
	GoalPostSample z, z1, z2;
	AbstractSample *as, *as1, *as2, *as3, *as4;

	//cout << this->goal.type << endl;

	switch(this->goal.type){
	case GOAL_LEFT_POST:
	case GOAL_RIGHT_POST:
		/*case GOAL_COMPLETE:
		cout << "Observación no ambigua ";
		goals->updateFromObservation( goal );
		break;*/
	case GOAL_UNKNOWN_POST:
		//cout << "Observación ambigua de un solo poste desconocido\n";
		z = (*this->goal.post1);
		as = dynamic_cast<GoalPostSample*>(&z);
		//cout << "PostSample (" << z.p3D.X << "," << z.p3D.Y << ") Abstract " <<
		//		as->p3D.X << "," << as->p3D.Y << ")(" << z.p2D.x << "," <<
		//		z.p2D.y << ")\n";
		features.push_back( *as );
		goals->updateFromObservation( goal.color, features );

		break;
	case GOAL_COMPLETE:

		//cout << "Poste1 (" << this->goal.post1->p3D.X << "," <<  this->goal.post1->p3D.Y << ")\n";
		//cout << "Poste2 (" << this->goal.post2->p3D.X << "," <<  this->goal.post2->p3D.Y << ")\n";
		z1 = (*this->goal.post1);
		as1 = dynamic_cast<GoalPostSample*>(&z1);
		//cout << "PostSample1 (" << z1.p3D.X << "," << z1.p3D.Y << ") Abstract " <<
		//		as1->p3D.X << "," << as1->p3D.Y << ")(" << z1.p2D.x << "," <<
		//		z1.p2D.y << ")\n";
		features.push_back( *as1 );

		z2 = (*this->goal.post2);
		as2 = dynamic_cast<GoalPostSample*>(&z2);
		//cout << "PostSample2 (" << z2.p3D.X << "," << z2.p3D.Y << ") Abstract " <<
		//		as2->p3D.X << "," << as2->p3D.Y << ")(" << z2.p2D.x << "," <<
		//		z2.p2D.y << ")\n";
		features.push_back( *as2 );

		// Generate virtual observations for the opposite goal
		//cout << "GoalDetector::step() Generating two virtual opposite posts\n";
		HPoint3D vp0, vp1;
		goals->generateVirtualNet(z1.p3D, z2.p3D, vp0, vp1);
		GoalPostSample z3, z4;
		z3.p3D.X = vp0.X;
		z3.p3D.Y = vp0.Y;
		z4.p3D.X = vp1.X;
		z4.p3D.Y = vp1.Y;
		as3 = dynamic_cast<GoalPostSample*>(&z3);
		features.push_back( *as3 );
		as4 = dynamic_cast<GoalPostSample*>(&z4);
		features.push_back( *as4 );

		goals->updateFromObservation( goal.color, features );
		break;
	}
}

void
GoalDetector::getGTOppNetCenter( double &x, double &y )
{
	double robotX, robotY, robotTh;

	//Convert to relative coordinates
	_GTLocalization->getPosition( robotX, robotY, robotTh );

	//cout << "RobotPos: " << robotX << "," << robotY << "," << robotTh << ")\n";

	Pose2D robotPose( robotTh, robotX, robotY);
	Vector2<double> oppNetAbs(3000, 0);
	Vector2<double> oppNetRel = Pose2D::fieldCoord2Relative( robotPose, oppNetAbs );

	x = oppNetRel.x;
	y = oppNetRel.y;
}

void
GoalDetector::setGTActivated()
{
	_gtReceived = true;
}

void
GoalDetector::getGTOppNetLeftPost( double &x, double &y )
{
	double robotX, robotY, robotTh;

	//Convert to relative coordinates
	_GTLocalization->getPosition( robotX, robotY, robotTh );

	Pose2D robotPose( robotTh, robotX, robotY);
	Vector2<double> oppNetAbs(3000, 700);
	Vector2<double> oppNetRel = Pose2D::fieldCoord2Relative( robotPose, oppNetAbs );

	x = oppNetRel.x;
	y = oppNetRel.y;
}

void
GoalDetector::getGTOppNetRightPost( double &x, double &y )
{
	double robotX, robotY, robotTh;

	//Convert to relative coordinates
	_GTLocalization->getPosition( robotX, robotY, robotTh );

	Pose2D robotPose( robotTh, robotX, robotY);
	Vector2<double> oppNetAbs(3000, -700);
	Vector2<double> oppNetRel = Pose2D::fieldCoord2Relative( robotPose, oppNetAbs );

	x = oppNetRel.x;
	y = oppNetRel.y;
}

void
GoalDetector::getGTOwnNetCenter( double &x, double &y )
{
	double robotX, robotY, robotTh;

	//Convert to relative coordinates
	_GTLocalization->getPosition( robotX, robotY, robotTh );

	Pose2D robotPose( robotTh, robotX, robotY);
	Vector2<double> oppNetAbs(-3000, 0);
	Vector2<double> oppNetRel = Pose2D::fieldCoord2Relative( robotPose, oppNetAbs );

	x = oppNetRel.x;
	y = oppNetRel.y;
}

void
GoalDetector::getGTOwnNetLeftPost( double &x, double &y )
{
	double robotX, robotY, robotTh;

	//Convert to relative coordinates
	_GTLocalization->getPosition( robotX, robotY, robotTh );

	Pose2D robotPose( robotTh, robotX, robotY);
	Vector2<double> oppNetAbs(-3000, -700);
	Vector2<double> oppNetRel = Pose2D::fieldCoord2Relative( robotPose, oppNetAbs );

	x = oppNetRel.x;
	y = oppNetRel.y;
}

void
GoalDetector::getGTOwnNetRightPost( double &x, double &y )
{
	double robotX, robotY, robotTh;

	//Convert to relative coordinates
	_GTLocalization->getPosition( robotX, robotY, robotTh );

	Pose2D robotPose( robotTh, robotX, robotY);
	Vector2<double> oppNetAbs(-3000, 700);
	Vector2<double> oppNetRel = Pose2D::fieldCoord2Relative( robotPose, oppNetAbs );

	x = oppNetRel.x;
	y = oppNetRel.y;
}
