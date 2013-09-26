#include "ObservationModel.h"

ObservationModel::ObservationModel()
{
	_LineDetector = LineDetector::getInstance();
	_GoalDetector = GoalDetector::getInstance();
	_ImageHandler = ImageHandler::getInstance();
	_Kinematics = Kinematics::getInstance();

	this->current_reliability=0.0;

	/*Debug*/
	imgPoints = new ImageData();
	IceUtil::Time t = IceUtil::Time::now();
	imgPoints->timeStamp.seconds = (long)t.toSeconds();
	imgPoints->timeStamp.useconds = (long)t.toMicroSeconds() - imgPoints->timeStamp.seconds*1000000;
	imgPoints->pixelData.resize(ImageInput::IMG_WIDTH * ImageInput::IMG_HEIGHT * ImageInput::IMG_CHANNELS);
	this->imgSrc = cvCreateImage(cvSize(ImageInput::IMG_WIDTH, ImageInput::IMG_HEIGHT), ImageInput::BYTES_PER_CHANNEL, ImageInput::IMG_CHANNELS);
}

ObservationModel::~ObservationModel()
{
	cvReleaseImage(&imgSrc);
}

float
ObservationModel::getImageReliability()
{
	return this->current_reliability;
}

bool
ObservationModel::getImageParameters(vector<TImageObject> &objects, bool &hasGoals)
{
	TImageObject object;
	vector<TImageObject> points;
	bool has_complete_goal, has_partial_goal;
	bool has_circle_center, has_intersections, has_lines;
	double lines_rel;
	int npoints;

	has_complete_goal = has_partial_goal = false;
	has_circle_center = has_intersections = has_lines = false;

	objects.clear();

	/*Goals*/
	GoalSample * gs = _GoalDetector->getGoal();
	if(gs->found) {
		hasGoals = true;
		object.color = gs->color;
		object.isgoal = true;

		/*Complete goal*/
		if(gs->type == GOAL_COMPLETE) {
			/*Save both posts*/
			object.p3D = gs->post1->p3D;
			object.type = gs->post1->type;
			objects.push_back(object);

			object.p3D = gs->post2->p3D;
			object.type = gs->post2->type;
			objects.push_back(object);
			has_complete_goal = true;
		} else {
			/*Save one post*/
			object.p3D = gs->post1->p3D;
			object.type = gs->post1->type;
			objects.push_back(object);
			has_partial_goal = true;
		}
	}

	/*Circle*/
	CircleSample * circle = _LineDetector->getCircle();
	if(circle->found) {

		object.color = ImageInput::CWHITE;
		object.isgoal = false;
		object.p3D = circle->p3D;
		object.type = LineDetector::CIRCLE_CENTER;
		objects.push_back(object);
		has_circle_center = true;

		/*Intersection between circle and midline*/
		if(circle->midLine != NULL) {
			/*Cover all intersections*/
			list<IntersectionSample> * intersections = _LineDetector->getIntersections();
			for(list<IntersectionSample>::iterator it=intersections->begin();it!=intersections->end();it++) {
				if((*it).type != LineDetector::INTERSECTION_TYPE_X)
					continue;

				/*Save X intersections*/
				object.p3D = (*it).p3D;
				object.type = (*it).type;
				objects.push_back(object);
				has_intersections = true;
			}		
		}
	}

	/*Lines*/
	list<LineSample> * lines = _LineDetector->getLines();
	for(list<LineSample>::iterator line = lines->begin(); line != lines->end(); line++)
		this->getPointsFromLine(points, (*line).ps2D.x, (*line).ps2D.y, (*line).pe2D.x, (*line).pe2D.y, ImageInput::CWHITE, true);

	/*Compare similar points*/
	npoints = 0;
	for(vector<TImageObject>::iterator it=points.begin(); it!=points.end(); it++) {
		if((*it).valid) {
			for(vector<TImageObject>::iterator it2=it+1; it2!=points.end(); it2++) {
				if((*it2).valid) {
					/*If the points are together, delete the second one*/
					if(calcDistance2D((*it).p2D.x, (*it).p2D.y, (*it2).p2D.x, (*it2).p2D.y)<=8.0)
						(*it2).valid=0;
				}
			}
			npoints++;
		}
	}

	/*Select max points*/
	if(points.size() > 0) {
		has_lines = true;
		this->selectRandom(points, MAX_SELECTED_PLINES, objects, npoints);
	}

	/*Calculate reliability*/
	this->current_reliability = 0.0;
	if(has_complete_goal)
		this->current_reliability += 0.8;
	else if(has_partial_goal)
		this->current_reliability += 0.5;

	if(has_circle_center)
		this->current_reliability += 0.4;
	if(has_intersections)
		this->current_reliability += 0.2;

	if(has_lines) {
		this->current_reliability += 0.2;
		lines_rel = points.size();
		if(lines_rel > MAX_SELECTED_PLINES)
			lines_rel = MAX_SELECTED_PLINES;
		this->current_reliability += 0.2 * ((double)lines_rel/(double)MAX_SELECTED_PLINES);
	}	

	return this->current_reliability > 0.0;
}

void
ObservationModel::getPointsFromLine(vector<TImageObject> &points, double xini, double yini, double xfin, double yfin, int color, bool extremes)
{
	TImageObject object;
	double coef = 0.0;
	double maxcoef = 1.0;

	if(!extremes) {
		coef = 0.25;
		maxcoef = 0.75;
	}

	object.color = color;
	object.isgoal = false;
	object.type = LineDetector::LINE_SEGMENT;
	object.valid = true;

	while(coef <= maxcoef) {

		getPointFromVector(object.p2D.x, object.p2D.y, xini, yini, xfin, yfin, coef);
		points.push_back(object);

		coef+=0.25;
	}
}

int
ObservationModel::getNextPosition(vector<TImageObject> &points, int from, int size)
{
	int i;
	int count=0;

	i=from;

	do {
		if(i>=size)
			i=0;

		if(points[i].valid)
			return i;

		i++;
		count++;
	} while(count<size);

	return -1;
}

void
ObservationModel::selectRandom(vector<TImageObject> &points, int maxpoints, vector<TImageObject> &objects, int size)
{
	int pos;
	int numleft;

	if(size > maxpoints) {
		/*Select random points*/
		numleft = maxpoints;
		while(numleft) {
			pos = rand()%size;
			pos = getNextPosition(points, pos, size);
			if(pos>=0) {
				/*Save selected point*/
				objects.push_back(points[pos]);
				points[pos].valid = 0;
				numleft--;
			} else
				break;
		}
	} else {
		/*Save all*/
		for(int i=0;i<size;i++)
			objects.push_back(points[i]);
	}
}

/*#############################################################################################################################
															Debug
##############################################################################################################################*/

bica::ImageDataPtr
ObservationModel::getImgPoints()
{
	/*CvPoint p;
	int color;
	int totp;
	int i;

	totp = this->totalDebugPoints;*/

	_ImageHandler->getImageRGB(this->imgSrc->imageData, false, 0);

	/*Selected points*/
	/*for(i=0;i<totp;i++) {
		p.x = this->debugPoints[i*3];
		p.y = this->debugPoints[i*3+1];
		color = this->debugPoints[i*3+2];
		if(color == ImageInput::CWHITE)
			this->drawRectangle(this->imgSrc, p, CV_RGB(0,0,255),2);
		else if(color == ImageInput::CBLUE || color == ImageInput::CYELLOW)
			this->drawRectangle(this->imgSrc, p, CV_RGB(255,0,0),2);
		else if(color == 25) //Yellow
			this->drawRectangle(this->imgSrc, p, CV_RGB(255,255,0),2);
		else if(color == 26) //Dark yellow
			this->drawRectangle(this->imgSrc, p, CV_RGB(200,200,0),2);
	}*/

	memmove( &(imgPoints->pixelData[0]), (char *) this->imgSrc->imageData, ImageInput::IMG_WIDTH * ImageInput::IMG_HEIGHT * ImageInput::IMG_CHANNELS * sizeof(char));

	return this->imgPoints;
}

void
ObservationModel::drawRectangle(IplImage* src, CvPoint p, CvScalar clr, int val)
{
	CvPoint p1, p2;

	p1.x = p.x-val;
	p1.y = p.y-val;

	p2.x = p.x+val;
	p2.y = p.y+val;

	cvRectangle(src, p1, p2, clr, 1, 8, 0);
}
