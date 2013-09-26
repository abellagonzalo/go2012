#include "LocalizationHealth.h"

LocalizationHealth::LocalizationHealth()
{
	_Kinematics = Kinematics::getInstance();
	_MovementModel = MovementModel::getInstance();
	_GameController = GameController::getInstance();

	this->max_distance = 160.0;
}

LocalizationHealth::~LocalizationHealth()
{
}

void
LocalizationHealth::init()
{
	this->loadDistances();
}

/*#############################################################################################################################
															Health calculation
##############################################################################################################################*/

double
LocalizationHealth::getHealth(float x, float y, float theta, TKinematics robotConfig, vector<TImageObject> objects)
{
	double prob, tmp_prob, tmp_prob2;
	double geometry_prob, lines_prob;
	bool used_geometry = false;
	bool used_lines = false;
	TPinHoleCamera camera;
	double distance, dist_lines = 0.0;
	int num_lines = 0;
	HPoint2D in;
	HPoint3D out;

	geometry_prob = 1.0;
	lines_prob = 1.0;

	/*Configure camera*/
	_Kinematics->configureCamera(robotConfig, &camera, x, y, theta);

	/*Get geometry prob*/
	for(vector<TImageObject>::iterator it = objects.begin(); it!=objects.end(); it++) {
		tmp_prob = 1.0;

		if((*it).isgoal) {
			/*Post of a goal*/
			/*TODO BLUEGOAL: Check both goals*/
			//tmp_prob = this->getHealthPost(x, y, theta, (*it).p3D.X, (*it).p3D.Y, (*it).color, (*it).type, (int) _GameController->getMyColor());
			tmp_prob = this->getHealthPost(x, y, theta, (*it).p3D.X, (*it).p3D.Y, (*it).color, (*it).type, TEAM_BLUE);
			tmp_prob2 = this->getHealthPost(x, y, theta, (*it).p3D.X, (*it).p3D.Y, (*it).color, (*it).type, TEAM_RED);
			if(tmp_prob2 > tmp_prob)
				tmp_prob = tmp_prob2;

			used_geometry = true;
		} else if((*it).type == LineDetector::CIRCLE_CENTER) {
			/*Circle center*/
			tmp_prob = this->getHealthCircle(x, y, theta, (*it).p3D.X, (*it).p3D.Y);
			used_geometry = true;
		} else if((*it).type == LineDetector::INTERSECTION_TYPE_X) {
			tmp_prob = this->getHealthMidLine(x, y, theta, (*it).p3D.X, (*it).p3D.Y);
			used_geometry = true;
		}

		geometry_prob = geometry_prob*tmp_prob;
	}

	/*Get lines prob*/
	for(vector<TImageObject>::iterator it = objects.begin(); it!=objects.end(); it++) {
		if(!(*it).isgoal && (*it).type == LineDetector::LINE_SEGMENT) {
			in.x = (*it).p2D.x;
			in.y = (*it).p2D.y;
			in.h = 1.0;

			/*Get 3D position and distance*/
			_Kinematics->get3DPosition(&camera, out, in);
			distance = this->getDistanceLine(&camera, in, out);

			/*Distance too big*/
			if(distance >= (double)DISTANCE_INFINITE || distance > max_distance) {
				lines_prob = 0.1;
				break;
			}

			dist_lines+=distance;
			num_lines++;
			used_lines = true;
		}
	}

	if(num_lines > 0 && lines_prob == 1.0) {
		lines_prob = 1.0-(dist_lines/(double)num_lines)/100.0;
		if(lines_prob<0)
			lines_prob=0;
	}

	if(!used_geometry && !used_lines)
		return 0.5;

	if(!used_geometry)
		return lines_prob;
	if(!used_lines)
		return geometry_prob;

	prob = geometry_prob*0.7 + lines_prob*0.3;

	return prob;
}

double
LocalizationHealth::getHealthPost(float x, float y, float theta, float post_x, float post_y, int post_color, int post_type, int team_type)
{
	double coordinates, prob, prob2, prob_dist, prob_angle;
	HPoint2D real_post;
	bool use_both;

	/*Check player color*/
	if((team_type == TEAM_BLUE && post_color == ImageInput::CYELLOW) || (team_type == TEAM_RED && post_color == ImageInput::CBLUE))
		coordinates = 1.0;
	else
		coordinates = -1.0;

	/*Check type*/
	if(post_type == GOAL_LEFT_POST) {
		real_post.x = 3000.0*coordinates;
		real_post.y = 700.0*coordinates;
		use_both = false;
	} else if(post_type == GOAL_RIGHT_POST) {
		real_post.x = 3000.0*coordinates;
		real_post.y = -700.0*coordinates;
		use_both = false;		
	} else {
		/*Unknown post*/
		use_both = true;
	}

	if(use_both) {
		/*Recursive call, check both goals and save best prob*/
		prob = this->getHealthPost(x, y, theta, post_x, post_y, post_color, GOAL_LEFT_POST, team_type);
		prob2 = this->getHealthPost(x, y, theta, post_x, post_y, post_color, GOAL_RIGHT_POST, team_type);
		if(prob2 > prob)
			prob = prob2;
	} else {
		prob_dist = this->calc_distance_prob(x, y, post_x, post_y, real_post.x, real_post.y);
		prob_angle = this->calc_angle_prob(x, y, theta, post_x, post_y, real_post.x, real_post.y);
		prob = prob_dist*0.7 + prob_angle*0.3;
	}
	
	return prob;
}

double
LocalizationHealth::getHealthCircle(float x, float y, float theta, float circle_x, float circle_y)
{
	double prob, prob_dist, prob_angle;
	HPoint2D real_circle;

	real_circle.x = 0.0;
	real_circle.y = 0.0;	

	prob_dist = this->calc_distance_prob(x, y, circle_x, circle_y, real_circle.x, real_circle.y);
	prob_angle = this->calc_angle_prob(x, y, theta, circle_x, circle_y, real_circle.x, real_circle.y);
	prob = prob_dist*0.6 + prob_angle*0.4;

	return prob;
}

double
LocalizationHealth::getHealthMidLine(float x, float y, float theta, float midline_x, float midline_y)
{
	double prob, prob2, prob_dist, prob_dist2, prob_angle, prob_angle2;
	HPoint2D real_midline1, real_midline2;

	real_midline1.x = 0.0;
	real_midline1.y = 600.0;	
	real_midline2.x = 0.0;
	real_midline2.y = -600.0;	

	prob_dist = this->calc_distance_prob(x, y, midline_x, midline_y, real_midline1.x, real_midline1.y);
	prob_angle = this->calc_angle_prob(x, y, theta, midline_x, midline_y, real_midline1.x, real_midline1.y);
	prob = prob_dist*0.6 + prob_angle*0.4;

	prob_dist2 = this->calc_distance_prob(x, y, midline_x, midline_y, real_midline2.x, real_midline2.y);
	prob_angle2 = this->calc_angle_prob(x, y, theta, midline_x, midline_y, real_midline2.x, real_midline2.y);
	prob2 = prob_dist2*0.6 + prob_angle2*0.4;

	if(prob2 > prob)
		prob = prob2;

	return prob;
}

double
LocalizationHealth::calc_distance_prob(double robot_x, double robot_y, double obj_x, double obj_y, double real_obj_x, double real_obj_y)
{
	double rel_dist, real_dist;
	double prob;
	double distance = 3000.0;

	/*Get relative and real distances*/
	rel_dist = (double) calcDistance2D((float)obj_x, (float)obj_y, 0.0, 0.0);
	real_dist = (double) calcDistance2D((float)robot_x, (float)robot_y, (float)real_obj_x, (float)real_obj_y);

	prob = 1.0 - abs(rel_dist-real_dist)/distance;
	if(prob < 0.1)
		prob = 0.1;	

	return prob;	
}

double
LocalizationHealth::calc_angle_prob(double robot_x, double robot_y, double robot_t, double obj_x, double obj_y, double real_obj_x, double real_obj_y)
{
	double rel_angle, real_angle;
	double theta, prob;
	double angle = LOC_PI;

	/*Get relative and real angles*/
	rel_angle = this->calc_angle(0.0, 0.0, obj_x, obj_y);
	real_angle = this->calc_angle(robot_x, robot_y, real_obj_x, real_obj_y);

	/*Normalize real_angle with robot_t*/
	this->normalize_angle(robot_t);
	theta = real_angle - robot_t;
	this->normalize_angle(theta);

	/*Get difference*/
	theta = theta - rel_angle;
	this->normalize_angle(theta);

	/*Calc prob*/
	prob = 1.0 - abs(theta)/angle;
	if(prob < 0.1)
		prob = 0.1;	

	return prob;
}

double
LocalizationHealth::calc_angle(double x1, double y1, double x2, double y2)
{
	double diffx, diffy;

	diffx = x2 - x1;
	diffy = y2 - y1;

	if(diffy == 0.0) {
		if(diffx >= 0.0)
			return 0.0;
		else
			return LOC_PI; //PI
	}

	if(diffx == 0.0) {
		if(diffy >= 0.0)
			return LOC_PI_2;	//PI/2
		else
			return -LOC_PI_2; //-PI/2
	}

	return atan2(diffy, diffx);
}

void
LocalizationHealth::normalize_angle(double &angle)
{
	while(angle > LOC_PI)
		angle -= 2*LOC_PI;
	while(angle < -LOC_PI)
		angle += 2*LOC_PI;
}


double
LocalizationHealth::getDistanceLine(TPinHoleCamera * camera, HPoint2D p2d, HPoint3D p3d)
{

	int index1, index2;
	HPoint2D p2dtmp;
	HPoint3D p3dtmp;

	index1 = (int)(p3d.X+(float)MAXX+(float)XERROR_LINES)/JUMP_LINES;
	index2 = (int)(p3d.Y+(float)MAXY+(float)YERROR_LINES)/JUMP_LINES;

	if(index1<0)
		index1=0;
	if(index2<0)
		index2=0;
	if(index1>=ROWS_DISTANCES_LINES)
		index1=ROWS_DISTANCES_LINES-1;
	if(index2>=COLS_DISTANCES_LINES)
		index2=COLS_DISTANCES_LINES-1;

	p3dtmp = distancesLines[index1][index2];

	_Kinematics->get2DPosition(camera, p3dtmp, p2dtmp);

	/*Don't see the point*/
	if(p2dtmp.h < 0) 
		return (double)DISTANCE_INFINITE;

	return distancePoints(p2d.x, p2d.y, p2dtmp.x, p2dtmp.y);
}

double
LocalizationHealth::distancePoints(float xini, float yini, float xfin, float yfin)
{
	return sqrt((xini-xfin)*(xini-xfin) + (yini-yfin)*(yini-yfin));
}

/*#############################################################################################################################
																				Best Health
##############################################################################################################################*/

void
LocalizationHealth::getBestPositions(vector<TImageObject> objects, vector<HPoint2D> &positions)
{
	TImageObject left, right, unknown;

	positions.clear();

	left.type = GOAL_LEFT_POST;
	left.isgoal = false;
	right.type = GOAL_RIGHT_POST;
	right.isgoal = false;
	unknown.type = GOAL_UNKNOWN_POST;
	unknown.isgoal = false;

	/*Get posts*/
	for(vector<TImageObject>::iterator it = objects.begin(); it!=objects.end(); it++) {
		if((*it).isgoal) {
			if((*it).type == GOAL_LEFT_POST)
				left = *it;
			else if((*it).type == GOAL_RIGHT_POST)
				right = *it;
			else if((*it).type == GOAL_UNKNOWN_POST)
				unknown = *it;
		} 
	}
	
	/*TODO BLUEGOAL: Check both goals*/
	if(unknown.isgoal) {
		//this->getPositionsPost(unknown, positions, unknown.type, (int) _GameController->getMyColor());
		this->getPositionsPost(unknown, positions, unknown.type, TEAM_BLUE);
		this->getPositionsPost(unknown, positions, unknown.type, TEAM_RED);
	}
	if(left.isgoal) {
		//this->getPositionsPost(left, positions, left.type, (int) _GameController->getMyColor());
		this->getPositionsPost(left, positions, left.type, TEAM_BLUE);
		this->getPositionsPost(left, positions, left.type, TEAM_RED);
	}
	if(right.isgoal) {
		//this->getPositionsPost(right, positions, right.type, (int) _GameController->getMyColor());	
		this->getPositionsPost(right, positions, right.type, TEAM_BLUE);	
		this->getPositionsPost(right, positions, right.type, TEAM_RED);	
	}
}

void
LocalizationHealth::getPositionsPost(TImageObject post, vector<HPoint2D> &positions, int post_type, int team_type)
{
	double coordinates;
	HPoint2D real_post, pos;
	double distance, pos_x, pos_y, pos_theta, angle;
	bool use_both;
	double i, factor;

	/*Check player color*/
	if((team_type == TEAM_BLUE && post.color == ImageInput::CYELLOW) || (team_type == TEAM_RED && post.color == ImageInput::CBLUE))
		coordinates = 1.0;
	else
		coordinates = -1.0;

	/*Check type*/
	if(post_type == GOAL_LEFT_POST) {
		real_post.x = 3000.0*coordinates;
		real_post.y = 700.0*coordinates;
		use_both = false;
	} else if(post_type == GOAL_RIGHT_POST) {
		real_post.x = 3000.0*coordinates;
		real_post.y = -700.0*coordinates;
		use_both = false;		
	} else {
		/*Unknown post*/
		use_both = true;
	}

	if(use_both) {
		/*Recursive call, check both post and save best positions*/
		this->getPositionsPost(post, positions, GOAL_LEFT_POST, team_type);
		this->getPositionsPost(post, positions, GOAL_RIGHT_POST, team_type);
	} else {
		/*Relative distance and angle*/
		distance = (double) calcDistance2D(post.p3D.X, post.p3D.Y, 0.0, 0.0);
		angle = this->calc_angle(0.0, 0.0, post.p3D.X, post.p3D.Y);

		/*Save circle around the post*/
		factor = 4000.0/distance;
		i = -180.0;
		while(i<180.0) {
			pos_theta = i*LOC_PI/180.0;
			if(this->getPositionXY(real_post, distance, pos_theta, pos_x, pos_y)) {
				pos_theta = (pos_theta + LOC_PI) - angle;
				this->normalize_angle(pos_theta);

				/*Add position*/
				pos.x = pos_x;
				pos.y = pos_y;
				pos.h = pos_theta;
				positions.push_back(pos);
			}
			i += factor;
		}
	}
}

bool
LocalizationHealth::getPositionXY(HPoint2D center, double radius, double angle, double &X, double &Y)
{
	double pos_x, pos_y;
	float old_x, old_y, old_angle = 0.0;

	/*Solve x^2 + y^2 = r^2*/
	pos_x = center.x + radius*cos(angle);
	pos_y = center.y + radius*sin(angle);

	/*Check field position*/
	old_x = pos_x;
	old_y = pos_y;
	_MovementModel->checkValidPosition(old_x, old_y, old_angle);
	if(abs(old_x - pos_x) > 0.1 || abs(old_y - pos_y) > 0.1)
		return false;

	X = pos_x;
	Y = pos_y;

	return true;
}

/*#############################################################################################################################
															Distances
##############################################################################################################################*/

void
LocalizationHealth::loadDistances()
{

	char word1[255];
	char word2[255];
	char word3[255];
	char word4[255];
	FILE * file;
	int count;
	int isrow, iscol;

	/*Lines*/
	file = fopen("/home/nao/bica/conf/geneticLocalization/distancesLines.txt", "r");

	if(file==NULL) {
		cout << "The configuration file of location is NULL" << endl;
	}

	count=0;
	while (fscanf (file, "%s %s %s %s", word1, word2, word3, word4) != EOF) {
		isrow=count/COLS_DISTANCES_LINES;
		iscol=count%COLS_DISTANCES_LINES;
		distancesLines[isrow][iscol].X = atof(word3);
		distancesLines[isrow][iscol].Y = atof(word4);
		distancesLines[isrow][iscol].Z = 0.0;
		distancesLines[isrow][iscol].H = 1.0;
		count++;

		if(count==ROWS_DISTANCES_LINES*COLS_DISTANCES_LINES)
			break;
	}
	fclose (file);
}
