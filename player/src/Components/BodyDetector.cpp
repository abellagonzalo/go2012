#include "BodyDetector.h"

BodyDetector::BodyDetector()
{
	this->_Kinematics = Kinematics::getInstance();

	this->escale = 1000.0;
}

BodyDetector::~BodyDetector()
{
}

void
BodyDetector::init(const string newName, AL::ALPtr<AL::ALBroker> parentBroker)
{
	Component::init(newName, parentBroker);
	try {
		this->pmotion = parentBroker->getMotionProxy();
	} catch( ALError& e)	{
		cerr<<"[BodyDetector:init()] " << e.toString() << endl;
	}

	this->setFreqTime(MEDIUM_RATE);

}

void
BodyDetector::stop()
{
}

void
BodyDetector::forceStep(void)
{

	if (!isTime2Run())
		return;
	startDebugInfo();
	this->calcFoots();

	this->createSegments();

	/*Calc zones with robot's body*/
	this->calcBodyLine();

	/*vector<float> HeadRT = this->pmotion->getTransform("Head", 2, USE_SENSOR);
	cout << "Head" << endl;
	this->printMatrix(HeadRT);

	vector<float> LArmRT = this->pmotion->getTransform("LArm", 2, USE_SENSOR);
	cout << "LArm" << endl;
	this->printMatrix(LArmRT);

	vector<float> RArmRT = this->pmotion->getTransform("RArm", 2, USE_SENSOR);
	cout << "RArm" << endl;
	this->printMatrix(RArmRT);

	vector<float> LLegRT = this->pmotion->getTransform("LLeg", 2, USE_SENSOR);
	cout << "LLeg" << endl;
	this->printMatrix(LLegRT);

	vector<float> RLegRT = this->pmotion->getTransform("RLeg", 2, USE_SENSOR);
	cout << "RLeg" << endl;
	this->printMatrix(RLegRT);

	vector<float> TorsoRT = this->pmotion->getTransform("Torso", 2, USE_SENSOR);
	cout << "Torso" << endl;
	this->printMatrix(TorsoRT);*/

//Name of the item. Could be: Head, LArm, RArm, LLeg, RLeg, Torso, CameraTop, CameraBottom, MicroFront, MicroRear, MicroLeft, MicroRight, LFsrFR, LFsrFL, LFsrRR, LFsrRR, RFsrFR, RFsrFL, RFsrRR, RFsrRR, Accelerometer, Gyrometer.
	endDebugInfo();
}

void
BodyDetector::step(void)
{
}

int
BodyDetector::getMaxRow(int index)
{
	if(index >= ImageInput::IMG_STEPS_V || index < 0)
		return 0;

	return borders[index].y;
}

void
BodyDetector::calcBodyLine()
{

	int marginx = ImageInput::IMG_MARGIN_STEP;
	int col, counter;
	HPoint2D cols, cole;
	HPoint3D colline;
	HPoint2D intersection;

	counter = 0;

	/*Cover columns*/
	for(col=marginx;col<ImageInput::IMG_WIDTH;col=col+ImageInput::IMG_STEP) {

		/*Create vector for column*/
		cols.x = col;
		cols.y = 0;
		cole.x = col;
		cole.y = ImageInput::IMG_HEIGHT;
		calcVector2D(cols, cole, colline);

		borders[counter].x = (float) col;
		borders[counter].y = (float) ImageInput::IMG_HEIGHT;

		/*Cover all lines and try to find intersections*/
		for(vector<BodySegment>::iterator segment = this->segments.begin(); segment != this->segments.end(); segment++) {

			/*Calculate intersection*/
			calcIntersection2D(colline, (*segment).line, intersection);

			/*Check borders*/
			if(intersection.h == 0.0)
				continue;
			if(intersection.x < (*segment).minx || intersection.x > (*segment).maxx)
				continue;
			if(intersection.y < (*segment).miny || intersection.y > (*segment).maxy)
				continue;
			
			/*If is upper in image, save it*/
			if(intersection.y < borders[counter].y)
				borders[counter].y = (float) intersection.y;
		}

		counter++;
	}
}

void
BodyDetector::calcFoots()
{
	HPoint3D init;
	
	vector<float> LLegRT = this->pmotion->getTransform("LLeg", 2, USE_SENSOR);

	init.X = LLegRT[3]*this->escale;
	init.Y = LLegRT[7]*this->escale;
	init.Z = LLegRT[11]*this->escale;
	init.H = 1.0;

	LFoot[0] = LFoot[1] = LFoot[2] = LFoot[3] = init;

	LFoot[0].X += FOOTS_X_FRONT;
	LFoot[0].Y += FOOTS_Y_OUT;
	LFoot[1].X += FOOTS_X_FRONT;
	LFoot[1].Y -= FOOTS_Y_IN;
	LFoot[2].X -= FOOTS_X_BACK;
	LFoot[2].Y += FOOTS_Y_OUT;
	LFoot[3].X -= FOOTS_X_BACK;
	LFoot[3].Y -= FOOTS_Y_IN;

	/*Calc position of left foot in image*/
	this->_Kinematics->get2DPosition(LFoot[0], LFoot2D[0]);
	this->_Kinematics->get2DPosition(LFoot[1], LFoot2D[1]);
	this->_Kinematics->get2DPosition(LFoot[2], LFoot2D[2]);
	this->_Kinematics->get2DPosition(LFoot[3], LFoot2D[3]);

	vector<float> RLegRT = this->pmotion->getTransform("RLeg", 2, USE_SENSOR);

	init.X = RLegRT[3]*this->escale;
	init.Y = RLegRT[7]*this->escale;
	init.Z = RLegRT[11]*this->escale;
	init.H = 1.0;

	RFoot[0] = RFoot[1] = RFoot[2] = RFoot[3] = init;

	RFoot[0].X += FOOTS_X_FRONT;
	RFoot[0].Y += FOOTS_Y_IN;
	RFoot[1].X += FOOTS_X_FRONT;
	RFoot[1].Y -= FOOTS_Y_OUT;
	RFoot[2].X -= FOOTS_X_BACK;
	RFoot[2].Y += FOOTS_Y_IN;
	RFoot[3].X -= FOOTS_X_BACK;
	RFoot[3].Y -= FOOTS_Y_OUT;

	/*Calc position of right foot in image*/
	this->_Kinematics->get2DPosition(RFoot[0], RFoot2D[0]);
	this->_Kinematics->get2DPosition(RFoot[1], RFoot2D[1]);
	this->_Kinematics->get2DPosition(RFoot[2], RFoot2D[2]);
	this->_Kinematics->get2DPosition(RFoot[3], RFoot2D[3]);
}

void
BodyDetector::createSegments()
{
	/*Clear old segments*/
	this->segments.clear();

	/*Left foot*/
	if(LFoot2D[0].h > 0 && LFoot2D[1].h > 0)
		this->createNewSegment(LFoot2D[0], LFoot2D[1]);
	if(LFoot2D[0].h > 0 && LFoot2D[2].h > 0)
		this->createNewSegment(LFoot2D[0], LFoot2D[2]);
	if(LFoot2D[1].h > 0 && LFoot2D[3].h > 0)
		this->createNewSegment(LFoot2D[1], LFoot2D[3]);

	/*Right foot*/
	if(RFoot2D[0].h > 0 && RFoot2D[1].h > 0)
		this->createNewSegment(RFoot2D[0], RFoot2D[1]);
	if(RFoot2D[0].h > 0 && RFoot2D[2].h > 0)
		this->createNewSegment(RFoot2D[0], RFoot2D[2]);
	if(RFoot2D[1].h > 0 && RFoot2D[3].h > 0)
		this->createNewSegment(RFoot2D[1], RFoot2D[3]);
}

void
BodyDetector::createNewSegment(HPoint2D p1, HPoint2D p2)
{
	BodySegment segment;

	/*Set column border*/
	segment.miny = 0;
	segment.maxy = ImageInput::IMG_HEIGHT;

	/*Set body border*/	
	if(p1.x < p2.x)
		segment.minx = p1.x;
	else
		segment.minx = p2.x;

	if(p1.x > p2.x)
		segment.maxx = p1.x;
	else
		segment.maxx = p2.x;

	/*Calculate vector*/
	calcVector2D(p1, p2, segment.line);	

	/*Add segment*/
	this->segments.push_back(segment);
}

void
BodyDetector::printMatrix(vector<float> matrix)
{
	cout << "--------------------" << endl;
	cout << "| " << matrix[0] << " " << matrix[1] << " " << matrix[2] << " " << matrix[3] << " |" << endl;
	cout << "| " << matrix[4] << " " << matrix[5] << " " << matrix[6] << " " << matrix[7] << " |" << endl;
	cout << "| " << matrix[8] << " " << matrix[9] << " " << matrix[10] << " " << matrix[11] << " |" << endl;
	cout << "--------------------" << endl;
}



void
BodyDetector::drawBody(IplImage* src)
{
	CvPoint p;
	int i;

	/*Left foot*/
	this->drawLine(src, LFoot2D[0], LFoot2D[1]);
	this->drawLine(src, LFoot2D[0], LFoot2D[2]);
	this->drawLine(src, LFoot2D[1], LFoot2D[3]);

	/*Right foot*/
	this->drawLine(src, RFoot2D[0], RFoot2D[1]);
	this->drawLine(src, RFoot2D[0], RFoot2D[2]);
	this->drawLine(src, RFoot2D[1], RFoot2D[3]);

	for(i=0;i<ImageInput::IMG_STEPS_V;i++) {
		p.x = this->borders[i].x;
		p.y = this->borders[i].y;
		if(p.y < ImageInput::IMG_HEIGHT)
			cvCircle(src, p, 2, CV_RGB(0,0,0), 2);	
	}
}

void
BodyDetector::drawLine(IplImage* src, HPoint2D p1, HPoint2D p2)
{

	CvPoint p, q;

	if(p1.h > 0 && p2.h > 0) {
		p.x = (int)p1.x;
		p.y = (int)p1.y;
		q.x = (int)p2.x;
		q.y = (int)p2.y;

		cvLine(src, p, q, CV_RGB(255,0,0), 1, CV_AA, 0);
	} 
}
