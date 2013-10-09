/*
 * Name: W3DDetector.h
 * @Author: Eduardo Perdices (eperdices@gsyc.es)
 * @Author: Carlos Agüero (caguero@gsyc.es)
 * @Author: Francisco Martín Rico (fmartin@gsyc.es)
 * 
 * Description: Wave3D
 *
 * Created on: 18/07/2011
 *
 * Copyright (C) Universidad Rey Juan Carlos
 * All Rights Reserved.
 *
 */

#include "W3DDetector.h"
#include "Kinematics.h"


const float W3DDetector::MIN_VALID_RATIO = 0.8;
const float W3DDetector::BALL_RADIUS = 32.5;
const float W3DDetector::MIN_VALID_RATIO_RADIUS = 0.7;
const long W3DDetector::TIME_SLICE = 15000;
const float W3DDetector::DEFAULT_SP_DENSITY = 30.0;

W3DDetector::W3DDetector() : Detector(), W3DAttentionClient()
								{

	this->storage = cvCreateMemStorage();


	this->color = ImageInput::CORANGE;

	// Ball model
	ballmodel = new BallModel(1, 3);

	lastBodyX = lastBodyY = lastBodyTh = 0.0f;
	lastBallTimestamp = 0;
	measurement = false;

	intPoints.clear();
	debugPoints.clear();
	pthread_mutex_init(&mutex, NULL);

	lastImageTs = 0;

}

W3DDetector::~W3DDetector()
{
}

void
W3DDetector::init(const string newName, AL::ALPtr<AL::ALBroker> parentBroker) {
	Component::init(newName, parentBroker);
	perception = Perception::getInstance();
	body = Body::getInstance();
	kinematics =  Kinematics::getInstance();

	this->setFreqTime(SHORT_RATE);

	//For attention
	initClient();

	resetScan();

	ballDetected = false;

//	ostringstream s;
//	s<<"HB\t"<<getCurrentTime()<<"\t1.0"<<endl;
//	s<<"HB\t"<<getCurrentTime()<<"\t0.0"<<endl;
//	writeLog(s.str());

}

void
W3DDetector::step()
{
	struct timeval s1, s2;
	long start, end, acum;
	ostringstream s;
	//startDebugInfo();
	acum = 0;

	gettimeofday(&s1, NULL);
	start = s1.tv_sec * 1000000 + s1.tv_usec;

	long ts = getCurrentTime();

	perception->shortStep();
	if(lastImageTs == perception->getImageTs())
		return;


	lastImageTs = perception->getImageTs();

	gettimeofday(&s2, NULL);
	end = s2.tv_sec * 1000000 + s2.tv_usec;
	acum = acum + (end-start);

	//s<<"HB\t"<<ts<<"\t0.0"<<endl;
	//s<<"HB\t"<<ts<<"\t1.0"<<endl;


	//s<<"w 1: "<<(end-start)<<endl;
	//writeLog(s.str());

	body->step();
	kinematics->step();


	gettimeofday(&s1, NULL);
	start = s1.tv_sec * 1000000 + s1.tv_usec;

	this->detect(true);
	s<<"TW\t"<<getCurrentTime()<<"\t"<<(getCurrentTime()-ts)-perception->getWaitingTime()<<endl;
	writeLog(s.str());

	gettimeofday(&s2, NULL);
	end = s2.tv_sec * 1000000 + s2.tv_usec;
	acum = acum + (end-start);

	//s<<"w 2: "<<(end-start)<<endl;
	//writeLog(s.str());





	ballmodel->predict();
	ballmodel->updateFromOdometry();
	if(this->balls.size() > 0)
	{
			updateFromObservation();
			this->lastSeen = this->getTime();
	}

	ballDetected = 	(this->balls.size() > 0);

	int det = 0;
	if((this->balls.size() > 0))
	{
		det = 1;
	}

	Vector2<double> ball = ballmodel->estimate.getPositionInRelativeCoordinates();

	HPoint2D simg;
	simg.x = ImageInput::IMG_WIDTH/2;
	simg.y = ImageInput::IMG_HEIGHT/2;
	simg.h = 1.0;
	HPoint3D s3D;
	kinematics->get3DPosition(s3D, simg);

	s<<"AC\t"<<getCurrentTime()<<"\t"<<ball[0]<<"\t"<<ball[1]<<"\t"<<"\t"<<s3D.X/s3D.H<<"\t"<<s3D.Y/s3D.H<<"\t"<<det<<endl;
	writeLog(s.str());
	//	else
	//		s<<"W3D\t"<<end<<"\t100000000.0\t100000000.0\t"<<"\t"<<s3D.X/s3D.H<<"\t"<<s3D.Y/s3D.H<<"\t"<<(LoadSim::getInstance()->getLoad())<<"\t"<<perception->getImageAge()<<"\t"<<acum<<endl;
	//
	//	writeLog(s.str());


	//s<<"w A: "<<(acum-perception->getWaitingTime())<<endl;
	//writeLog(s.str());
	//endDebugInfo();
	//s<<"HB\t"<<getCurrentTime()<<"\t1.0"<<endl;
	//s<<"HB\t"<<getCurrentTime()<<"\t0.0"<<endl;
	//writeLog(s.str());
	//	//if (!isTime2Run())
//	//	return;
//
//
//
//	struct timeval s1, s2;
//	long start, end, acum;
//	acum = 0;
//	long wt=0;
//
//
//
//	gettimeofday(&s1, NULL);
//	start = s1.tv_sec * 1000000 + s1.tv_usec;
//	perception->shortStep();
//	gettimeofday(&s2, NULL);
//	end = s2.tv_sec * 1000000 + s2.tv_usec;
//	//acum = acum + (end-start);
//
//	//wt = wt + perception->getWaitingTime();
//
//	body->step();
//	kinematics->step();
//	_LoadSim->step();
//
//
//
//	Vector2<double> ball = ballmodel->estimate.getPositionInRelativeCoordinates();
//
//	gettimeofday(&s1, NULL);
//	start = s1.tv_sec * 1000000 + s1.tv_usec;
//	this->detect(true);
//	wt = wt + waitdetect;
//	gettimeofday(&s2, NULL);
//	end = s2.tv_sec * 1000000 + s2.tv_usec;
//	acum = acum + (end-start)-waitdetect;
//
//
//
//	/*
//	 * Exp1
//
//	if(ballDetected)
//		cerr<<"W3D\t"<<end<<"\t"<<ball[0]<<"\t"<<ball[1]<<"\t"<<acum<<endl;
//	else
//		cerr<<"W3D\t"<<end<<"\t100000000.0\t100000000.0\t"<<acum<<endl;
//	 */
//
//
//	HPoint2D simg;
//	simg.x = ImageInput::IMG_WIDTH/2;
//	simg.y = ImageInput::IMG_HEIGHT/2;
//	simg.h = 1.0;
//	HPoint3D s3D;
//	kinematics->get3DPosition(s3D, simg);
//
//
//	ostringstream s;
//
//	if(ballDetected)
//		s<<"W3D\t"<<end<<"\t"<<ball[0]<<"\t"<<ball[1]<<"\t"<<"\t"<<s3D.X/s3D.H<<"\t"<<s3D.Y/s3D.H<<"\t"<<(LoadSim::getInstance()->getLoad())<<"\t"<<perception->getImageAge()<<"\t"<<acum<<endl;
//	else
//		s<<"W3D\t"<<end<<"\t100000000.0\t100000000.0\t"<<"\t"<<s3D.X/s3D.H<<"\t"<<s3D.Y/s3D.H<<"\t"<<(LoadSim::getInstance()->getLoad())<<"\t"<<perception->getImageAge()<<"\t"<<acum<<endl;
//
//	writeLog(s.str());
//
//	ballmodel->predict();
//	ballmodel->updateFromOdometry();
//
//	/*Save time*/
//	if(this->balls.size() > 0)
//	{
//		updateFromObservation();
//
//		this->lastSeen = this->getTime();
//		//cerr<<"["<<this->lastSeen<<"]";
//	}
//
//	//endDebugInfo();
//	//ballmodel->timeWhenLastSeen[0] = (unsigned)((getCurrentTime() - lastBallTimestamp) / 1000000);	// secs
//	/*if(this->isSeen())
//		cerr<<"+";
//	else
//		cerr<<"-";*/
//
//	//BallModel* balls = this->getObj();
//	//Vector2<double> pos = balls->estimate.getPositionInRelativeCoordinates();
//
//	//cerr<<"Ball at: "<<pos.x<<", "<<pos.y<<endl;
}

void
W3DDetector::calculateCameraLimits()
{
	HPoint2D si, sd, ii, id;

	si.x = 0.0; si.y = 0.0; si.h = 1.0;
	sd.x = ImageInput::IMG_WIDTH; sd.y = 0.0; sd.h = 1.0;
	ii.x = 0.0; ii.y = ImageInput::IMG_HEIGHT; ii.h = 1.0;
	id.x = ImageInput::IMG_WIDTH; id.y = ImageInput::IMG_HEIGHT; id.h = 1.0;

	kinematics->get3DPosition(camerap1, sd);
	kinematics->get3DPosition(camerap2, si);
	kinematics->get3DPosition(camerap3, ii);
	kinematics->get3DPosition(camerap4, id);

	float angle1, angle2, angle3, angle4;

	angle1 = atan2(camerap1.Y, camerap1.X);
	angle2 = atan2(camerap2.Y, camerap2.X);
	angle3 = atan2(camerap3.Y, camerap3.X);
	angle4 = atan2(camerap4.Y, camerap4.X);

	if((angle1*angle4)<0.0)
	{
		float theta, rho;

		theta = atan2(camerap4.Y, camerap4.X);
		rho = 8000.0;

		camerap1.X = rho * cos(theta);
		camerap1.Y = rho * sin(theta);
	}

	if((angle2*angle3)<0.0)
	{
		float theta, rho;

		theta = atan2(camerap3.Y, camerap3.X);
		rho = 8000.0;

		camerap2.X = rho * cos(theta);
		camerap2.Y = rho * sin(theta);
	}

}

bool
W3DDetector::InSegment(HPoint3D p1, HPoint3D p2, HPoint3D pt)
{
	float dis1, dis2;
	float l;

	l = sqrt((p1.X-p2.X)*(p1.X-p2.X)+(p1.Y-p2.Y)*(p1.Y-p2.Y));

	dis1 = sqrt((p1.X - pt.X)*(p1.X - pt.X)+(p1.Y - pt.Y)*(p1.Y - pt.Y));
	dis2 = sqrt((p2.X - pt.X)*(p2.X - pt.X)+(p2.Y - pt.Y)*(p2.Y - pt.Y));
	return ((dis1<=l) && (dis2<=l));

}

void
W3DDetector::getIntersection(HPoint3D center, float radius, HPoint3D p1, HPoint3D p2, HPoint3D &res, HPoint3D &res2, int &intersec)
{
	double det;
	double a,b,c,m,l,s;
	HPoint3D s1, s2;
	float A,B,C;

	A = -(p2.Y - p1.Y);
	B = p2.X - p1.X;
	C = -(((p2.X-p1.X)*p1.Y) - ((p2.Y-p1.Y)*p1.X));

	if(fabs(B)>fabs(A))
	{
		m = -A/B;
		s = -C/B;

		l = s - center.Y;

		a = 1.0 + (m*m);
		b = (2.0 * m * l) - (2.0 * center.X);
		c = (l*l) - (radius*radius) + (center.X*center.X);

		det = (b*b)-(4.0*a*c);

		if(det < 0)
		{
			intersec = 0;
			return;
		}

		if(det == 0)
		{
			s1.X = -b/(2.0*a);
			s1.Y = ((m*s1.X) + s);

			if(InSegment(p1, p2, s1))
			{
				intersec = 1;
				res.X = s1.X;
				res.Y = s1.Y;
				res.Z = 0.0;
				res.H = 1.0;
				return;
			}else
			{
				intersec = 0;
				return;
			}
		}else
		{
			intersec = 0;

			float xs1, xs2, rdet, a2;

			rdet = sqrt(det);
			a2 = 2.0*a;

			xs1 = (-b+rdet)/(a2);
			xs2 = (-b-rdet)/(a2);
			s1.X = xs1;
			s1.Y = ((m*xs1) + s);
			s2.X = xs2;
			s2.Y = ((m*xs2) + s);

			if(InSegment(p1, p2, s1) && InSegment(p1, p2, s2))
			{
				intersec = 2;

				res.X = s1.X;
				res.Y = s1.Y;
				res.Z = 0.0;
				res.H = 1.0;
				res2.X = s2.X;
				res2.Y = s2.Y;
				res2.Z = 0.0;
				res2.H = 1.0;
				return;
			}else
				if(InSegment(p1, p2, s1))
				{
					intersec = 1;
					res.X = s1.X;
					res.Y = s1.Y;
					res.Z = 0.0;
					res.H = 1.0;
					return;
				}else if(InSegment(p1, p2, s2))
				{
					intersec = 1;
					res.X = s2.X;
					res.Y = s2.Y;
					res.Z = 0.0;
					res.H = 1.0;
					return;

				}
		}
	}else
	{
		m = -B/A;
		s = -C/A;

		l = s - center.X;

		a = 1.0 + (m*m);
		b = (2.0 * m * l) - (2.0 * center.Y);
		c = (l*l) - (radius*radius) + (center.Y*center.Y);

		det = (b*b)-(4.0*a*c);

		if(det < 0)
		{
			intersec = 0;
			return;
		}

		if(det == 0)
		{

			s1.Y = -b/(2.0*a);
			s1.X = ((m*s1.Y) + s);

			if(InSegment(p1, p2, s1))
			{
				intersec = 1;
				res.X = s1.X;
				res.Y = s1.Y;
				res.Z = 0.0;
				res.H = 1.0;
				return;
			}else
			{
				intersec = 0;
				return;
			}
		}else
		{
			float ys1, ys2, rdet, a2;

			rdet = sqrt(det);
			a2 = 2.0*a;

			ys1 = (-b+rdet)/(a2);
			ys2 = (-b-rdet)/(a2);

			s1.Y = ys1;
			s1.X = ((m*ys1) + s);

			s2.Y = ys2;
			s2.X = ((m*ys2) + s);

			if(InSegment(p1, p2, s1) && InSegment(p1, p2, s2))
			{
				intersec = 2;

				res.X = s1.X;
				res.Y = s1.Y;
				res.Z = 0.0;
				res.H = 1.0;
				res2.X = s2.X;
				res2.Y = s2.Y;
				res2.Z = 0.0;
				res2.H = 1.0;
				return;
			}else
				if(InSegment(p1, p2, s1))
				{
					intersec = 1;
					res.X = s1.X;
					res.Y = s1.Y;
					res.Z = 0.0;
					res.H = 1.0;
					return;
				}else if(InSegment(p1, p2, s2))
				{
					intersec = 1;
					res.X = s2.X;
					res.Y = s2.Y;
					res.Z = 0.0;
					res.H = 1.0;
					return;

				}
		}
	}

	intersec = 0;

}

float
W3DDetector::getMinDistance2Vertex(HPoint3D pt)
{
	float dis1, dis2, dis3, dis4;
	float min = 100000.0;

	dis1 = sqrt((camerap1.X - pt.X)*(camerap1.X - pt.X)+(camerap1.Y - pt.Y)*(camerap1.Y - pt.Y));
	dis2 = sqrt((camerap2.X - pt.X)*(camerap2.X - pt.X)+(camerap2.Y - pt.Y)*(camerap2.Y - pt.Y));
	dis3 = sqrt((camerap3.X - pt.X)*(camerap3.X - pt.X)+(camerap3.Y - pt.Y)*(camerap3.Y - pt.Y));
	dis4 = sqrt((camerap4.X - pt.X)*(camerap4.X - pt.X)+(camerap4.Y - pt.Y)*(camerap4.Y - pt.Y));

	if(dis1<min) min = dis1;
	if(dis2<min) min = dis2;
	if(dis3<min) min = dis3;
	if(dis4<min) min = dis4;

	return min;
}

float
W3DDetector::getMinDistance2Line(HPoint3D pt)
{
	float dis1, dis2, dis3, dis4;
	float min = 100000.0;

	dis1 = getDistance2Line(pt, camerap1, camerap2);//sqrt((camerap1.X - pt.X)*(camerap1.X - pt.X)+(camerap1.Y - pt.Y)*(camerap1.Y - pt.Y));
	dis2 = getDistance2Line(pt, camerap2, camerap3);//sqrt((camerap2.X - pt.X)*(camerap2.X - pt.X)+(camerap2.Y - pt.Y)*(camerap2.Y - pt.Y));
	dis3 = getDistance2Line(pt, camerap3, camerap4);//sqrt((camerap3.X - pt.X)*(camerap3.X - pt.X)+(camerap3.Y - pt.Y)*(camerap3.Y - pt.Y));
	dis4 = getDistance2Line(pt, camerap4, camerap1);//sqrt((camerap4.X - pt.X)*(camerap4.X - pt.X)+(camerap4.Y - pt.Y)*(camerap4.Y - pt.Y));

	if(dis1<min) min = dis1;
	if(dis2<min) min = dis2;
	if(dis3<min) min = dis3;
	if(dis4<min) min = dis4;

	return min;
}

float
W3DDetector::getDistance2Line(HPoint3D pt, HPoint3D p1, HPoint3D p2)
{
	float A,B,C;

	A = -(p2.Y - p1.Y);
	B = p2.X - p1.X;
	C = -(((p2.X-p1.X)*p1.Y) - ((p2.Y-p1.Y)*p1.X));

	return (fabs(A*pt.X+B*pt.Y+C)/sqrt(A*A+B*B));
}

float
W3DDetector::getMaxDistance2Line(HPoint3D pt)
{
	float dis1, dis2, dis3, dis4;
	float max = -1.0;

	dis1 = getDistance2Line(pt, camerap1, camerap2);//sqrt((camerap1.X - pt.X)*(camerap1.X - pt.X)+(camerap1.Y - pt.Y)*(camerap1.Y - pt.Y));
	dis2 = getDistance2Line(pt, camerap2, camerap3);//sqrt((camerap2.X - pt.X)*(camerap2.X - pt.X)+(camerap2.Y - pt.Y)*(camerap2.Y - pt.Y));
	dis3 = getDistance2Line(pt, camerap3, camerap4);//sqrt((camerap3.X - pt.X)*(camerap3.X - pt.X)+(camerap3.Y - pt.Y)*(camerap3.Y - pt.Y));
	dis4 = getDistance2Line(pt, camerap4, camerap1);//sqrt((camerap4.X - pt.X)*(camerap4.X - pt.X)+(camerap4.Y - pt.Y)*(camerap4.Y - pt.Y));

	if(dis1>max) max = dis1;
	if(dis2>max) max = dis2;
	if(dis3>max) max = dis3;
	if(dis4>max) max = dis4;

	return max;
}

float
W3DDetector::getMaxDistance2Vertex(HPoint3D pt)
{
	float dis1, dis2, dis3, dis4;
	float max = -1.0;

	dis1 = sqrt((camerap1.X - pt.X)*(camerap1.X - pt.X)+(camerap1.Y - pt.Y)*(camerap1.Y - pt.Y));
	dis2 = sqrt((camerap2.X - pt.X)*(camerap2.X - pt.X)+(camerap2.Y - pt.Y)*(camerap2.Y - pt.Y));
	dis3 = sqrt((camerap3.X - pt.X)*(camerap3.X - pt.X)+(camerap3.Y - pt.Y)*(camerap3.Y - pt.Y));
	dis4 = sqrt((camerap4.X - pt.X)*(camerap4.X - pt.X)+(camerap4.Y - pt.Y)*(camerap4.Y - pt.Y));

	if(dis1>max) max = dis1;
	if(dis2>max) max = dis2;
	if(dis3>max) max = dis3;
	if(dis4>max) max = dis4;

	return max;
}
void
W3DDetector::resetDbgPoints()
{
	//pthread_mutex_lock(&mutex);

	vector<HPoint3D*>::iterator it;
	for (it = debugPoints.begin(); it != debugPoints.end(); ++it) {
		delete *it;
	}

	debugPoints.clear();
	//pthread_mutex_unlock(&mutex);

}

void
W3DDetector::addDbgPoint(HPoint3D p)
{
	//pthread_mutex_lock(&mutex);

	HPoint3D *aux = new HPoint3D;

	*aux = p;

	debugPoints.push_back(aux);

	//pthread_mutex_unlock(&mutex);

}

void
W3DDetector::resetIntPoints()
{
	//pthread_mutex_lock(&mutex);

	vector<HPoint3D*>::iterator it;
	for (it = intPoints.begin(); it != intPoints.end(); ++it) {
		delete *it;
	}

	intPoints.clear();
	//pthread_mutex_unlock(&mutex);

}

void
W3DDetector::addIntPoint(HPoint3D p)
{
	//pthread_mutex_lock(&mutex);

	HPoint3D *aux = new HPoint3D;

	*aux = p;

	intPoints.push_back(aux);

	//pthread_mutex_unlock(&mutex);

}

void
W3DDetector::resetScan()
{


	resetIntPoints();

	//cerr<<"RESET"<<endl;
	Vector2<double> ball = ballmodel->estimate.getPositionInRelativeCoordinates();
	center.X = ball[0];
	center.Y = ball[1];
	center.Z = 2.5;
	center.H = 1.0;

	ir = 0.0;
	theta = 0.0;

}

void
W3DDetector::calculateNewIntPoints()
{
	int in;
	HPoint3D i1, i2;

	resetIntPoints();

	getIntersection(center, ir, camerap1, camerap2, i1, i2, in);

	if(in == 1)
	{
		addIntPoint(i1);
	}
	if(in == 2)
	{
		addIntPoint(i1);
		addIntPoint(i2);
	}


	getIntersection(center, ir, camerap2, camerap3, i1, i2, in);
	if(in == 1)
	{
		addIntPoint(i1);
	}
	if(in == 2)
	{
		addIntPoint(i1);
		addIntPoint(i2);
	}
	getIntersection(center, ir, camerap3, camerap4, i1, i2, in);
	if(in == 1)
	{
		addIntPoint(i1);
	}
	if(in == 2)
	{
		addIntPoint(i1);
		addIntPoint(i2);
	}
	getIntersection(center, ir, camerap4, camerap1, i1, i2, in);
	if(in == 1)
	{
		addIntPoint(i1);
	}
	if(in == 2)
	{
		addIntPoint(i1);
		addIntPoint(i2);
	}
}




void
W3DDetector::detect(bool debug)
{
	waitdetect = 0;
	bool fin;
	struct timeval start_tv, current_tv;
	long start_m, current_m;
	float max;
	float vertx[4], verty[4];

	int pcheck=0;
	ostringstream s;
	fin = false;

	gettimeofday(&start_tv, NULL);
	start_m = start_tv.tv_sec * 1000000 + start_tv.tv_usec;

	this->clearDetected();

	calculateCameraLimits();

	vertx[0] = camerap1.X/camerap1.H;
	verty[0] = camerap1.Y/camerap1.H;
	vertx[1] = camerap2.X/camerap2.H;
	verty[1] = camerap2.Y/camerap2.H;
	vertx[2] = camerap3.X/camerap3.H;
	verty[2] = camerap3.Y/camerap3.H;
	vertx[3] = camerap4.X/camerap4.H;
	verty[3] = camerap4.Y/camerap4.H;

	max = 4 * sqrt(cvmGet(ballmodel->jpdaf->objects[0]->error_cov_post, 0, 0));

	//cerr<<"A max: "<<max<<endl;

	if (max > getMaxDistance2Vertex(center))
		max = getMaxDistance2Vertex(center);

	//cerr<<"B max: "<<max<<endl;

	if(max>2000)
		max = 2000;
	if(max<100)
			max = 100;

	while(!fin)
	{
		while((ir<max) && (!fin))
		{
			s<<"IO\t"<<getCurrentTime()<<"\t"<<getCurrentTime()-perception->getImageTs()<<endl;
			writeLog(s.str());
			if(lastImageTs != perception->getImageTs())
			{

				struct timeval s2;
				long start0, end0;

				gettimeofday(&s2, NULL);
				start0 = s2.tv_sec * 1000000 + s2.tv_usec;

				perception->shortStep();
				lastImageTs = perception->getImageTs();

				waitdetect = waitdetect + perception->getWaitingTime();

				body->step();
				kinematics->step();
				calculateCameraLimits();

				vertx[0] = camerap1.X/camerap1.H;
				verty[0] = camerap1.Y/camerap1.H;
				vertx[1] = camerap2.X/camerap2.H;
				verty[1] = camerap2.Y/camerap2.H;
				vertx[2] = camerap3.X/camerap3.H;
				verty[2] = camerap3.Y/camerap3.H;
				vertx[3] = camerap4.X/camerap4.H;
				verty[3] = camerap4.Y/camerap4.H;
				gettimeofday(&s2, NULL);
				end0 = s2.tv_sec * 1000000 + s2.tv_usec;

				max = 4 * sqrt(cvmGet(ballmodel->jpdaf->objects[0]->error_cov_post, 0, 0));

				if (max > getMaxDistance2Vertex(center))
					max = getMaxDistance2Vertex(center);
				if(max>2000)
					max = 2000;
				if(max<100)
						max = 100;

			}

			gettimeofday(&current_tv, NULL);
			current_m = current_tv.tv_sec * 1000000 + current_tv.tv_usec;

			if((current_m-start_m)>TIME_SLICE)
			{
				return;
			}

			calculateNewIntPoints();

			float steps = (2.0 * M_PI * ir)/DEFAULT_SP_DENSITY;
			if(steps==0) steps = 1.0;

			float incang = (2.0*M_PI)/steps;

			if(intPoints.size()==0)
			{
				HPoint3D scan;
				while((theta<2.0*pi) && (!fin))
				{

					scan.X = center.X+ir * cos(normalizePi(theta));
					scan.Y = center.Y+ir * sin(normalizePi(theta));
					scan.Z = 2.5;
					scan.H = 1.0;

					theta = theta+incang;

					pcheck++;
					//cerr<<"D "<<ir<<endl;
					fin = scan3DPos(scan);
				}
			}else
			{
				vector<HPoint3D*>::iterator it;
				for (it = intPoints.begin(); it != intPoints.end(); ++it)
				{
					float start, end;

					vector<HPoint3D*>::iterator ite;
					ite = (it+1);
					if(ite == intPoints.end()) ite = intPoints.begin();

					start = atan2((*it)->Y-center.Y, (*it)->X-center.X)+incang;
					end = atan2((*ite)->Y-center.Y, (*ite)->X-center.X);

					if(start<0.0) start = start + pi2;
					if(end<0.0) end = end + pi2;
					if(end<start) end = end+pi2;

					HPoint3D scan;
					while(start<end)
					{

						scan.X = center.X+ir * cos(normalizePi(start));
						scan.Y = center.Y+ir * sin(normalizePi(start));
						scan.Z = 2.5;
						scan.H = 1.0;
						pcheck++;

						fin = scan3DPos(scan);

						start = start+incang;
					}
				}
			}

			theta = 0.0;
			ir=ir+DEFAULT_SP_DENSITY;

		}

		resetScan();
	}

	ballDetected = fin;


	//cerr<<"[TI "<<current_m-start_m<<"]"<<endl;

}

bool
W3DDetector::scan3DPos(HPoint3D scan)
{
	struct current;

	Point2D pimg;

	HPoint2D simg;
	kinematics->get2DPosition(scan, simg);
	pimg.x = simg.x;
	pimg.y = simg.y;

	if((simg.x<0.0) || (simg.x>=ImageInput::IMG_WIDTH) || (simg.y<0.0) || (simg.y>=ImageInput::IMG_HEIGHT))
		return false;

	/*if(!perception->getHorizon()->isUnderHorizon(pimg))
	{
		return false;
	}*/

	if(perception->getImageHandler()->getColor(simg.x/simg.h,simg.y/simg.h) == ImageInput::CORANGE)
	{

		//cerr<<"ORANGE"<<endl;
		float cxi, cxd, cyt, cyb;

		for(cxi = 0.0; cxi<100.0; cxi = cxi + 1.0)
		{
			if(perception->getImageHandler()->getColor((simg.x-cxi)/simg.h,simg.y/simg.h) != ImageInput::CORANGE)
				break;
		}

		for(cxd = 0.0; cxd<100.0; cxd = cxd + 1.0)
		{
			if(perception->getImageHandler()->getColor((simg.x+cxd)/simg.h,simg.y/simg.h) != ImageInput::CORANGE)
				break;
		}

		for(cyt = 0.0; cyt<100.0; cyt = cyt + 1.0)
		{
			if(perception->getImageHandler()->getColor(simg.x/simg.h,(simg.y-cyt)/simg.h) != ImageInput::CORANGE)
				break;
		}

		for(cyb = 0.0; cyb<100.0; cyb = cyb + 1.0)
		{
			if(perception->getImageHandler()->getColor(simg.x/simg.h,(simg.y+cyb)/simg.h) != ImageInput::CORANGE)
				break;
		}


		simg.x = ((simg.x - cxi) + (simg.x + cxd))/2.0;
		simg.y = ((simg.y - cyt) + (simg.y + cyb))/2.0;

		//cerr<<"("<<simg.x<<","<<simg.y<<")"<<endl;

		kinematics->get3DPosition(scan, simg);

		BallSampleW3D ballSampleW3D;
		ballSampleW3D.p2D = simg;
		ballSampleW3D.p3D = scan;
		ballSampleW3D.timestamp = getCurrentTime();

		this->balls.push_back(ballSampleW3D);

		return true;
	}
	return false;
}

void
W3DDetector::detect1(bool debug)
{
	/*Clear saved elements*/
	this->clearDetected();


	struct timeval start, current;
	long start_m, current_m;


	/*Validate the regions*/
	//_RegionBuilder->validateRegions(this->color);

	/*Validate balls*/
	//this->validateBalls();

	HPoint3D center;

	Vector2<double> ball = ballmodel->estimate.getPositionInRelativeCoordinates();
	center.X = ball[0];
	center.Y = ball[1];
	center.Z = 2.5;
	center.H = 1.0;


	/*
	 * Vamos a suponer que no es una elipse, sino un círculo
	 *
	estEllipse->center = p;
	estEllipse->width = 4 * sqrt(cvmGet(jpdaf->objects[0]->error_cov_post, 0, 0));
	estEllipse->length = 4 * sqrt(cvmGet(jpdaf->objects[0]->error_cov_post, 1, 1));
	estEllipse->angle =  toDegrees(atan2(cvmGet(jpdaf->objects[0]->state_post, 1, 0),
			cvmGet(jpdaf->objects[0]->state_post, 0, 0)));
	 */

	float radius = 4 * sqrt(cvmGet(ballmodel->jpdaf->objects[0]->error_cov_post, 0, 0));

	HPoint3D scan;
	HPoint2D simg;
	Point2D pimg;

	float vertx[4], verty[4];

	HPoint2D si, sd, ii, id;

	si.x = 0.0; si.y = 0.0; si.h = 1.0;
	sd.x = ImageInput::IMG_WIDTH; sd.y = 0.0; sd.h = 1.0;
	ii.x = 0.0; ii.y = ImageInput::IMG_HEIGHT; ii.h = 1.0;
	id.x = ImageInput::IMG_WIDTH; id.y = ImageInput::IMG_HEIGHT; id.h = 1.0;

	HPoint3D aux3D;
	kinematics->get3DPosition(aux3D, si);
	vertx[0] = aux3D.X/aux3D.H;
	verty[0] = aux3D.Y/aux3D.H;
	kinematics->get3DPosition(aux3D, ii);
	vertx[1] = aux3D.X/aux3D.H;
	verty[1] = aux3D.Y/aux3D.H;
	kinematics->get3DPosition(aux3D, id);
	vertx[2] = aux3D.X/aux3D.H;
	verty[2] = aux3D.Y/aux3D.H;
	kinematics->get3DPosition(aux3D, sd);
	vertx[3] = aux3D.X/aux3D.H;
	verty[3] = aux3D.Y/aux3D.H;

	if(!kinematics->pointInPolygon (4, vertx, verty, center.X/center.H, center.Y/center.H))
	{

		//cerr<<"RESET"<<endl;
		HPoint2D cimg;

		cimg.x = ImageInput::IMG_WIDTH/2.0;
		cimg.x = ImageInput::IMG_HEIGHT/2.0;
		cimg.h = 1.0;
		kinematics->get3DPosition(center, cimg);

		//theta = 0.0;
		//ir = 0.0;
	}


	bool encontrado= false;
	int n=0;

	gettimeofday(&start, NULL);
	start_m = start.tv_sec * 1000000 + start.tv_usec;



	while((ir<radius) && !encontrado)
	{
		n++;
		float steps = (2.0 * M_PI * radius)/20.0;
		if(steps==0) steps = 1.0;

		float incang = (2.0*M_PI)/steps;

		while((theta<2.0*pi) && !encontrado)
		{


			scan.X = center.X+ir * cos(normalizePi(theta));
			scan.Y = center.Y+ir * sin(normalizePi(theta));
			scan.Z = 2.5;
			scan.H = 1.0;

			theta = theta+incang;


			kinematics->get2DPosition(scan, simg);
			pimg.x = simg.x;
			pimg.y = simg.y;

			if((simg.x<0.0) || (simg.x>=ImageInput::IMG_WIDTH) || (simg.y<0.0) || (simg.y>=ImageInput::IMG_HEIGHT))
				continue;

			if(!perception->getHorizon()->isUnderHorizon(pimg))
			{
				//cerr<<"["<<scan.X<<","<<scan.Y<<"]";
				continue;
			}

			if(!kinematics->pointInPolygon (4, vertx, verty, scan.X/scan.H, scan.Y/scan.H))
			{
				continue;
			}




			if(perception->getImageHandler()->getColor(simg.x/simg.h,simg.y/simg.h) == ImageInput::CORANGE)
			{

				float cxi, cxd, cyt, cyb;

				for(cxi = 0.0; cxi<100.0; cxi = cxi + 1.0)
				{
					if(perception->getImageHandler()->getColor((simg.x-cxi)/simg.h,simg.y/simg.h) != ImageInput::CORANGE)
						break;
				}

				for(cxd = 0.0; cxd<100.0; cxd = cxd + 1.0)
				{
					if(perception->getImageHandler()->getColor((simg.x+cxd)/simg.h,simg.y/simg.h) != ImageInput::CORANGE)
						break;
				}

				for(cyt = 0.0; cyt<100.0; cyt = cyt + 1.0)
				{
					if(perception->getImageHandler()->getColor(simg.x/simg.h,(simg.y-cyt)/simg.h) != ImageInput::CORANGE)
						break;
				}

				for(cyb = 0.0; cyb<100.0; cyb = cyb + 1.0)
				{
					if(perception->getImageHandler()->getColor(simg.x/simg.h,(simg.y+cyb)/simg.h) != ImageInput::CORANGE)
						break;
				}


				simg.x = ((simg.x - cxi) + (simg.x + cxd))/2.0;
				simg.y = ((simg.y - cyt) + (simg.y + cyb))/2.0;

				kinematics->get3DPosition(scan, simg);

				//cerr<<"("<<scan.X<<","<<scan.Y<<")"<<endl;

				BallSampleW3D ballSampleW3D;
				ballSampleW3D.p2D = simg;
				ballSampleW3D.p3D = scan;
				ballSampleW3D.timestamp = getCurrentTime();

				this->balls.push_back(ballSampleW3D);

				encontrado = true;
				gettimeofday(&current, NULL);
				current_m = current.tv_sec * 1000000 + current.tv_usec;


				//cerr<<"["<<(current_m-start_m)<<"]"<<endl;

			}

			gettimeofday(&current, NULL);
			current_m = current.tv_sec * 1000000 + current.tv_usec;

			if((current_m-start_m)>TIME_SLICE)
			{


				//cerr<<"["<<(current_m-start_m)<<", "<<n<<", "<<ir<<","<<theta<<"]"<<endl;
				return;
			}

		}

		theta = 0.0;
		ir=ir+10.0;

	}

	ir = 0.0;

	/*gettimeofday(&current, NULL);
	current_m = current.tv_sec * 1000000 + current.tv_usec;

	if((current_m-start_m)>TIME_SLICE)
	{
		cerr<<"["<<(current_m-start_m)<<"]"<<endl;
	}*/


}

void
W3DDetector::validateBalls()
{
	BallSampleW3D ballSampleW3D;
	bool removeBall;
	list<BallRegion*> * bregions = _RegionBuilder->getBallRegions();
	//cout<<"n regiones: "<<bregions->size()<<endl;

	for(list<BallRegion*>::iterator ball = bregions->begin(); ball != bregions->end(); ball++) {

		//std::cout << "validamos en W3DDetector" << std::endl;

		/*Calculate the ball radius if the ball is valid*/
		if(!this->getBallRadius(*ball, ballSampleW3D))
			continue;

		/*Get expected radius*/
		this->getExpectedRadius(ballSampleW3D);

		//std::cout << "miramos expected radius" << std::endl;

		/*Check difference between radius and expected radius*/
		if(!this->checkRadius(ballSampleW3D))
			continue;

		//std::cout << "balon validado" << std::endl;

		this->balls.push_back(ballSampleW3D);
	}

	//std::cout << "tenemos al final: " << this->balls.size() << std::endl;

	/*Check balls in the same position*/
	if(this->balls.size() >= 2) {
		list<BallSampleW3D>::iterator ball1 = this->balls.begin();
		while(ball1 != this->balls.end()) {

			removeBall = false;

			list<BallSampleW3D>::iterator ball2 = ball1;
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
W3DDetector::getBallRadius(BallRegion* ballRegion, BallSampleW3D &ball)
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

	if(sum3 > sum4) {
		if(sum1 > sum2) {
			/*Save the values in the ball*/
			return this->saveValues(ballRegion, diag1, drow, sum1, sum3, ball);
		} else {
			/*Save the values in the ball*/
			return this->saveValues(ballRegion, dcol, diag1, sum2, sum3, ball);
		}
	} else {
		if(sum1 > sum2) {
			/*Save the values in the ball*/
			return this->saveValues(ballRegion, -1*diag2, drow, sum1, sum4, ball);
		} else {
			/*Save the values in the ball*/
			return this->saveValues(ballRegion, dcol, diag2, sum2, sum4, ball);
		}
	}

	return false;
}

bool
W3DDetector::saveValues(BallRegion* ballRegion, int col, int row, int sum1, int sum2, BallSampleW3D &ball)
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
	if(sum1 > sum2)
		ball.radius = ((float)sum1)/2.0;
	else
		ball.radius = ((float)sum2)/2.0;

	return true;	
}

bool
W3DDetector::checkBallRatio(int sum1, int sum2, float &ratio) {

	int max, min;

	/*Both more than 1*/
	if(sum1 <= 1 || sum2 <= 1)
		return false;

	/*One of them more than MIN_BALL_SUM*/
	if(sum1 < MIN_BALL_SUM && sum2 < MIN_BALL_SUM)
		return false;

	/*Get the maximal and cal the ratio*/
	if(sum1 > sum2) {
		max = sum1;
		min = sum2;
	} else {
		max = sum2;
		min = sum1;
	}

	ratio = (float)(min)/(float)max;

	/*Manage exceptions with small balls*/
	if(max==3)
		return min >= 2;

	/*Return true if the difference < MIN_THRESHOLD or the ratio is high*/
	return (ratio >= MIN_VALID_RATIO) || (min + MIN_THRESHOLD >= max);
}

void
W3DDetector::getExpectedRadius(BallSampleW3D &ball)
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
W3DDetector::checkRadius(BallSampleW3D &ball)
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
W3DDetector::clearDetected()
{
	if(!this->balls.empty())
	{
		this->balls.clear();
	}	
}

void
W3DDetector::drawValidRegions(IplImage* src)
{
}

void
W3DDetector::drawDetected(IplImage* src)
{
	CvPoint p;
	int aux;

	/*Draw balls*/
	for(list<BallSampleW3D>::iterator ball = this->balls.begin(); ball != this->balls.end(); ball++) {
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
W3DDetector::isSeen()
{
	return this->balls.size()>0;//seen();
}


void
W3DDetector::predict()
{
	ballmodel->predict();
}

void
W3DDetector::updateFromObservation()
{
	list<BallSampleW3D>::iterator ball = this->balls.begin();

	//ballmodel->updateFromObservation((*ball).p3D);

	lastBallTimestamp = getCurrentTime();

	list<AbstractSample> features;
	for (unsigned i = 0; i < this->balls.size(); i++ ) {
		BallSampleW3D z = (*ball);
		AbstractSample *as = dynamic_cast<BallSampleW3D*>(&z);
		//cerr << "BallSampleW3D (" << z.p3D.X << "," << z.p3D.Y << ") Abstract " <<
		//		as->p3D.X << "," << as->p3D.Y << ")(" << z.p2D.x << "," <<
		//		z.p2D.y << ")\n";
		features.push_back( *as );
		ball++;
	}

	ballmodel->updateFromObservation( features );
	//jpdaf->correct( features );
}

void
W3DDetector::updateFromOdometry()
{
	//predict();

	//jpdaf->correctFromOdometry();

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

	if (movX != 0 || movY != 0 || movA != 0) {	// There has been some movement

		//ballmodel->updateFromOdometry(dx, dy, movA);
	}
}

bica::ShapeList
W3DDetector::getGrDebugRel()
{

	pthread_mutex_lock(&mutex);

	shapeListRel.clear();

	// Center of the nets
	ShapeList auxList = ballmodel->getGrDebugRel();
	//Insert the GoalsModel's shape list into my shape list
	shapeListRel.insert(shapeListRel.end(), auxList.begin(), auxList.end());


	bica::Point3DPtr dp1(new bica::Point3D);
	bica::Point3DPtr dp2(new bica::Point3D);
	bica::Point3DPtr dp3(new bica::Point3D);
	bica::Point3DPtr dp4(new bica::Point3D);

	dp1->x = camerap1.X/camerap1.H;
	dp1->y = camerap1.Y/camerap1.H;
	dp1->z = camerap1.Z/camerap1.H;

	dp2->x = camerap2.X/camerap2.H;
	dp2->y = camerap2.Y/camerap2.H;
	dp2->z = camerap2.Z/camerap2.H;

	dp3->x = camerap3.X/camerap3.H;
	dp3->y = camerap3.Y/camerap3.H;
	dp3->z = camerap3.Z/camerap3.H;

	dp4->x = camerap4.X/camerap4.H;
	dp4->y = camerap4.Y/camerap4.H;
	dp4->z = camerap4.Z/camerap4.H;


	// lines
	bica::LinePtr l1(new bica::Line);
	l1->p1 = dp1;
	l1->p2 = dp2;
	l1->color = bica::YELLOW;
	l1->filled = true;
	l1->opacity = 255;
	l1->accKey = "l";
	//l1->label = "CameraViewL1";
	shapeListRel.push_back(l1);
	// lines
	bica::LinePtr l2(new bica::Line);
	l2->p1 = dp2;
	l2->p2 = dp3;
	l2->color = bica::YELLOW;
	l2->filled = true;
	l2->opacity = 255;
	l2->accKey = "l";
	//l2->label = "CameraViewL2";
	shapeListRel.push_back(l2);
	// lines
	bica::LinePtr l3(new bica::Line);
	l3->p1 = dp3;
	l3->p2 = dp4;
	l3->color = bica::YELLOW;
	l3->filled = true;
	l3->opacity = 255;
	l3->accKey = "l";
	//l3->label = "CameraViewL3";
	shapeListRel.push_back(l3);
	// lines
	bica::LinePtr l4(new bica::Line);
	l4->p1 = dp4;
	l4->p2 = dp1;
	l4->color = bica::YELLOW;
	l4->filled = true;
	l4->opacity = 255;
	l4->accKey = "l";
	//l4->label = "CameraViewL4";
	shapeListRel.push_back(l4);


	vector<HPoint3D*>::iterator it;
	for (it = debugPoints.begin(); it != debugPoints.end(); ++it) {


		bica::CirclePtr c(new bica::Circle);
		bica::Point3DPtr p(new bica::Point3D);

		p->x = (*it)->X/(*it)->H;
		p->y = (*it)->Y/(*it)->H;
		p->z = 100.0;//(*it)->Z/(*it)->H;

		//cerr<<"["<<p->x<<","<<p->y<<","<<p->z<<"]";

		c->center = p;
		c->radius = 50.0;
		c->color = bica::RED;
		c->filled = true;
		c->opacity = 255;
		c->accKey = "c";
		shapeListRel.push_back(c);

	}

	vector<HPoint3D*>::iterator it2;
	for (it2 = intPoints.begin(); it2 != intPoints.end(); ++it2) {


		bica::CirclePtr c(new bica::Circle);
		bica::Point3DPtr p(new bica::Point3D);

		p->x = (*it2)->X/(*it2)->H;
		p->y = (*it2)->Y/(*it2)->H;
		p->z = 100.0;//(*it)->Z/(*it)->H;

		//cerr<<"["<<p->x<<","<<p->y<<","<<p->z<<"]";

		c->center = p;
		c->radius = 50.0;
		c->color = bica::BLUE;
		c->filled = true;
		c->opacity = 255;
		c->accKey = "i";
		shapeListRel.push_back(c);

	}


	pthread_mutex_unlock(&mutex);

	return shapeListRel;

	/*shapeList.clear();

	bica::Point3DPtr p1(new bica::Point3D);
	bica::Point3DPtr p2(new bica::Point3D);
	bica::ArrowPtr arrow(new bica::Arrow);

	p1->x = 0.0f;
	p1->y = 0.0f;
	p1->z = 0.0f;

	// Estimation (JPDAF)
	p2->x = ballmodel->estimate.getPositionInRelativeCoordinates().x;
	p2->y = ballmodel->estimate.getPositionInRelativeCoordinates().y;
	p2->z = 0.0f;

	bica::EllipsePtr estEllipse(new bica::Ellipse);
	estEllipse->center = p2;
	if (measurement) {
		estEllipse->width = 4 * sqrt(cvmGet(ballmodel->jpdaf->objects[0]->error_cov_post, 0, 0));
		estEllipse->length = 4 * sqrt(cvmGet(ballmodel->jpdaf->objects[0]->error_cov_post, 1, 1));
	} else {
		estEllipse->width = 4 * sqrt(cvmGet(ballmodel->jpdaf->objects[0]->error_cov_pre, 0, 0));
		estEllipse->length = 4 * sqrt(cvmGet(ballmodel->jpdaf->objects[0]->error_cov_pre, 1, 1));
	}
	estEllipse->angle =  toDegrees(ballmodel->estimate.getAngle());
	estEllipse->z = 0.0f;
	estEllipse->color = bica::WHITE;
	estEllipse->filled = true;
	estEllipse->opacity = 125;
	estEllipse->accKey = "b";
	estEllipse->label = "Ball";
	shapeList.push_back(estEllipse);

	// Velocity
	bica::Point3DPtr p3(new bica::Point3D);
	arrow->src = p2;
	p3->x = p2->x + ballmodel->dx;
	p3->y = p2->y + ballmodel->dy;
	arrow->dst = p3;
	arrow->width = 1;
	arrow->color = bica::ORANGE;
	arrow->filled = false;
	arrow->opacity = 125;
	arrow->accKey = "b";
	arrow->label = "";
	shapeList.push_back(arrow);

	//JPDAF
	bica::EllipsePtr JPDAFellipse(new bica::Ellipse);
	JPDAFellipse->center = p2;
	if (measurement) {
		JPDAFellipse->width = sqrt(cvmGet(ballmodel->jpdaf->error_cov_post, 0, 0));
		JPDAFellipse->length = sqrt(cvmGet(ballmodel->jpdaf->error_cov_post, 1, 1));
	} else {
		JPDAFellipse->width = sqrt(cvmGet(ballmodel->jpdaf->error_cov_pre, 0, 0));
		JPDAFellipse->length = sqrt(cvmGet(ballmodel->jpdaf->error_cov_pre, 1, 1));
	}
	JPDAFellipse->angle =  toDegrees(ballmodel->JPDAFestimate.getAngle());
	JPDAFellipse->z = 0.0f;
	JPDAFellipse->color = bica::RED;
	JPDAFellipse->filled = true;
	JPDAFellipse->opacity = 125;
	JPDAFellipse->accKey = "c";
	JPDAFellipse->label = "JPDAF";

	shapeList.push_back(JPDAFellipse);


	// Observations
	if (measurement) {
		list<BallSampleW3D>::iterator it;
		int i = 0;

		for ( it = balls.begin(); it != balls.end(); it++) {
			bica::Point3DPtr p(new bica::Point3D);
			bica::EllipsePtr ellipse(new bica::Ellipse);

			p->x = (float)((*it).p3D.X);
			p->y = (float)((*it).p3D.Y);
			p->z = 0.0f;

			ellipse->center = p;
			ellipse->length = 4 * JPDAF::MEASUREMENT_NOISE_COV;
			ellipse->width = 4 * JPDAF::MEASUREMENT_NOISE_COV;
			ellipse->angle =  toDegrees(atan2((*it).p3D.Y, (*it).p3D.X));
			ellipse->z = 0.0f;
			ellipse->color = bica::ORANGE;
			ellipse->filled = true;
			ellipse->opacity = 125;
			ellipse->accKey = "z";
			stringstream ss;//create a stringstream
			ss << "Z" << i + 1;
			ellipse->label = ss.str();
			shapeList.push_back(ellipse);

			bica::LinePtr line(new bica::Line);
			line->p1 = p1;
			line->p2 = p;
			line->width = 1;
			line->color = bica::ORANGE;
			line->filled = false;
			line->opacity = 125;
			line->accKey = "z";
			line->label = "";
			shapeList.push_back(line);

			i++;
		}
	}

	//Get the JPDAF's shape list
	ShapeList auxList = jpdaf->getGrDebugRel();
	//Insert the JPDAF's shape list into my shape list
	shapeList.insert(shapeList.end(), auxList.begin(), auxList.end());

	return shapeList;*/
}

VisualMemoryObjPtr
W3DDetector::getVisualMemoryObject(const Ice::Current& c)
{
	VisualMemoryObjPtr ball = new VisualMemoryObj();
	ball->x = ballmodel->estimate.getPositionInRelativeCoordinates().x;
	ball->y = ballmodel->estimate.getPositionInRelativeCoordinates().y;

	ball->dx = 4 * sqrt(cvmGet(ballmodel->jpdaf->objects[0]->error_cov_post, 0, 0));
	ball->dy = 4 * sqrt(cvmGet(ballmodel->jpdaf->objects[0]->error_cov_post, 1, 1));

	//cout << "Quality: " << ballmodel->estimate.getQuality() << endl;
	ball->quality = ballmodel->estimate.getQuality();
	ball->time = ballmodel->elapsedTimeSinceLastObs;
	ball->reliability = ballmodel->estimate.getReliabilityString();

	return ball;
}

void
W3DDetector::predictionUpdate(const Ice::Current& c)
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
W3DDetector::measurementUpdate(const Ice::Current& c)
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
