#include "WorldModel.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>

using namespace std;

WorldModel::WorldModel()
{
	points = new list<wmPoint*>;
	lines = new list<wmLine*>;
	circles = new list<wmCircle*>;
	corners = new list<wmCorner*>;
}

void 
WorldModel::init()
{
	loadPoints(WORLD_MODEL_CONFIG_FILE);
	loadLines(WORLD_MODEL_CONFIG_FILE);
	loadCircles(WORLD_MODEL_CONFIG_FILE);
	loadCorners(WORLD_MODEL_CONFIG_FILE);

	this->sworld_model = this->getWorldModel();

	cout << "World Model Loaded" << endl;

	/*this->print();*/
}

WorldModel::~WorldModel()
{
	list<wmPoint*>::iterator itp;
	for(itp = points->begin(); itp != points->end(); itp++)
		delete *itp;
	delete points;

	list<wmLine*>::iterator itl;
	for(itl = lines->begin(); itl != lines->end(); itl++)
		delete *itl;
	delete lines;

	list<wmCircle*>::iterator itc;
	for(itc = circles->begin(); itc != circles->end(); itc++)
		delete *itc;
	delete circles;

	list<wmCorner*>::iterator itco;
	for(itco = corners->begin(); itco != corners->end(); itco++)
		delete *itco;
	delete corners;

}


void
WorldModel::loadPoints(string file)
{
	ifstream infile;
	string line, token;
	
	points->clear();

	infile.open(file.c_str(), ifstream::in);
	
	if(infile.fail())
	{
		cerr << begin_red << "[WorldModel] Error to load WorldModel file" << end_color << endl;
		return;
	}
	
	getline(infile, line);
	
	while (infile.good())
	{
		getline(infile, line);
		if(line.c_str()[0] == '#') continue;
		
		istringstream iss(line);
		getline(iss, token, ' ');
		if(token.compare("point") != 0) continue;
		
		wmPoint *point = new wmPoint;
		
		//ID
		getline(iss, token, ' ');
		point->id = token;

		//x,y,z,h
		getline(iss, token, ' ');
		point->p.X = (float)atof(token.c_str());
		getline(iss, token, ' ');
		point->p.Y = (float)atof(token.c_str());
		getline(iss, token, ' ');
		point->p.Z = (float)atof(token.c_str());
		getline(iss, token, ' ');
		point->p.H = (float)atof(token.c_str());
		
		points->push_back(point);
	}
	infile.close();
}

void
WorldModel::loadLines(string file)
{
	ifstream infile;
	string line, token;
	
	lines->clear();

	infile.open (file.c_str(), ifstream::in);
	getline(infile, line);
	
	while (infile.good())
	{
		getline(infile, line);
		if(line.c_str()[0] == '#') continue;
		
		istringstream iss(line);
		getline(iss, token, ' ');
		if(token.compare("line") != 0) continue;
		
		wmLine *line = new wmLine;
		
		//ID
		getline(iss, token, ' ');
		line->id = token;
		
		//points
		getline(iss, token, ' ');
		line->p1 = token;
		getline(iss, token, ' ');
		line->p2 = token;
		getline(iss, token, ' ');
		line->p3 = token;
		getline(iss, token, ' ');
		line->p4 = token;
		getline(iss, token, ' ');
		line->p5 = token;
		getline(iss, token, ' ');
		line->p6 = token;
		getline(iss, token, ' ');
		line->p7 = token;
		getline(iss, token, ' ');
		line->p8 = token;	
		getline(iss, token, ' ');
		line->color = token;	
		
		HPoint3D p1, p2, p3, p4;

		p1 = getPoint(line->p1)->p;
		p2 = getPoint(line->p2)->p;
		p3 = getPoint(line->p3)->p;
		p4 = getPoint(line->p4)->p;
		//cerr<<"("<<p1.X<<","<<p1.Y<<")("<<p2.X<<","<<p2.Y<<")("<<p3.X<<","<<p3.Y<<")("<<p4.X<<","<<p4.Y<<")"<<endl;

		line->angle = atan2(p1.Y-p2.Y, p1.X-p2.X);
		line->lenght = (sqrt((p1.Y-p2.Y)*(p1.Y-p2.Y)+(p1.X-p2.X)*(p1.X-p2.X))+sqrt((p3.Y-p4.Y)*(p3.Y-p4.Y)+(p3.X-p4.X)*(p3.X-p4.X)))/2.0;


		double vdx, vdy;
		vdx = p1.X - p2.X;
		vdy = p1.Y - p2.Y;

		double c;

			c = -vdy*p1.X + vdx*p1.Y;

			if(c!=0.0)
			{
				line->A = vdy / c;
				line->B = -vdx / c;
				line->C = 1.0;
			}else
			{
				line->A = vdy;
				line->B = -vdx;
				line->C = 0.0;
			}

		/*if(line->A!=0.0)
			cerr<<"eq for ("<<p1.X<<","<<p1.Y<<") - ("<<p2.X<<","<<p2.Y<<")\t"<<line->A/line->A<<"x+"<<line->B/line->A<<"y+"<<line->C/line->A<<"=0.0"<<endl;
		else
			cerr<<"eq for ("<<p1.X<<","<<p1.Y<<") - ("<<p2.X<<","<<p2.Y<<")\t"<<line->A/line->B<<"x+"<<line->B/line->B<<"y+"<<line->C/line->B<<"=0.0"<<endl;
	*/

		/*if((p2.X-p1.X)!=0.0)
			m = (p2.Y-p1.Y)/(p2.X-p1.X);
		else
			m = 1.0;

			line->A = m;
			line->B = -1.0;
			line->C = p1.Y - p1.X * m;
*/
		//		A = (y2-y1)/(x2-x1);
		//		B = -1.0;
		//		C = y1 - x1 * K;

		lines->push_back(line);
	}
	infile.close();
}



void
WorldModel::loadCorners(string file)
{
	ifstream infile;
	string line, token;

	corners->clear();

	infile.open (file.c_str(), ifstream::in);
	getline(infile, line);

	while (infile.good())
	{
		getline(infile, line);
		if(line.c_str()[0] == '#') continue;

		istringstream iss(line);
		getline(iss, token, ' ');
		if(token.compare("corner") != 0) continue;

		wmCorner *corner = new wmCorner;

		//ID
		getline(iss, token, ' ');
		corner->id = token;

		//points
		getline(iss, token, ' ');
		corner->p1 = token;
		getline(iss, token, ' ');
		corner->p2 = token;

		getline(iss, token, ' ');

		if(token.compare("T") == 0)
			corner->type = LineDetector::INTERSECTION_TYPE_T;
		if(token.compare("L") == 0)
			corner->type = LineDetector::INTERSECTION_TYPE_L;
		if(token.compare("X") == 0)
			corner->type = LineDetector::INTERSECTION_TYPE_X;

		corners->push_back(corner);
	}
	infile.close();
}



void
WorldModel::loadCircles(string file)
{
	ifstream infile;
	string line, token;
	
	circles->clear();

	infile.open (file.c_str(), ifstream::in);
	getline(infile, line);
	
	while (infile.good())
	{
		getline(infile, line);
		if(line.c_str()[0] == '#') continue;
		
		istringstream iss(line);
		getline(iss, token, ' ');
		if(token.compare("circle") != 0) continue;
		
		wmCircle *circle = new wmCircle;
		
		//ID
		getline(iss, token, ' ');
		circle->id = token;
		
		//points
		getline(iss, token, ' ');
		circle->p1 = token;
		getline(iss, token, ' ');
		circle->p2 = token;

		//radius
		getline(iss, token, ' ');
		circle->r1 = atof(token.c_str());
		getline(iss, token, ' ');
		circle->r2 = atof(token.c_str());
		getline(iss, token, ' ');
		circle->color = token;	

		circles->push_back(circle);
	}
	infile.close();
}

wmPoint*
WorldModel::getPoint(string id)
{
	list<wmPoint*>::iterator it = points->begin();

	while(it != points->end())
	{
		if((*it)->id.compare(id) == 0)
			return *it;
		it++;
	}
	cerr<<"No existe un punto llamado "<<id<<endl; 
	return NULL;
}

wmLine*
WorldModel::getLine(string id)
{
	list<wmLine*>::iterator it = lines->begin();

	while(it != lines->end())
	{
		if((*it)->id.compare(id) == 0)
			return *it;
		it++;
	}
	
	cerr<<"No existe una linea llamada "<<id<<endl; 
	return NULL;
}


wmCorner*
WorldModel::getCorner(string id)
{
	list<wmCorner*>::iterator it = corners->begin();

	while(it != corners->end())
	{
		if((*it)->id.compare(id) == 0)
			return *it;
		it++;
	}

	cerr<<"No existe un corner llamado "<<id<<endl;
	return NULL;
}

list<wmCorner*>*
WorldModel::getCornerType(int type)
{
	list<wmCorner*>* result;
	result = new list<wmCorner*>;



	list<wmCorner*>::iterator it = corners->begin();
	while(it != corners->end())
	{
		if((*it)->type == type)
			result->push_back(*it);
		it++;
	}
	return result;

}


wmCircle*
WorldModel::getCircle(string id)
{
	list<wmCircle*>::iterator it = circles->begin();

	while(it != circles->end())
	{
		if((*it)->id.compare(id) == 0)
			return *it;
		it++;
	}
	
	cerr<<"No existe un círculo llamado "<<id<<endl; 
	return NULL;

}

void 
WorldModel::print()
{
	wmPoint *p1, *p2, *p3, *p4, *p5, *p6, *p7, *p8;

	list<wmPoint*>::iterator it = this->points->begin();
	while(it != points->end())
	{
		cout<<"point "<<(*it)->id<<": ("
			<<(*it)->p.X<<", "
			<<(*it)->p.Y<<", "
			<<(*it)->p.Z<<", "
			<<(*it)->p.H<<")"<<endl;

		it++;
	}
	
	list<wmLine*>::iterator it2 = this->lines->begin();
	while(it2 != lines->end())
	{
		p1 = getPoint((*it2)->p1);
		p2 = getPoint((*it2)->p2);
		p3 = getPoint((*it2)->p3);
		p4 = getPoint((*it2)->p4);
		p5 = getPoint((*it2)->p5);
		p6 = getPoint((*it2)->p6);
		p7 = getPoint((*it2)->p7);
		p8 = getPoint((*it2)->p8);
		cout<<"line "<<(*it2)->id<<": "
			<<"("<<p1->p.X<<", "<<p1->p.Y<<", "<<p1->p.Z<<", "<<p1->p.H<<"), "
			<<"("<<p2->p.X<<", "<<p2->p.Y<<", "<<p2->p.Z<<", "<<p2->p.H<<"), "
			<<"("<<p3->p.X<<", "<<p3->p.Y<<", "<<p3->p.Z<<", "<<p3->p.H<<"), "
			<<"("<<p4->p.X<<", "<<p4->p.Y<<", "<<p4->p.Z<<", "<<p4->p.H<<"), "
			<<"("<<p5->p.X<<", "<<p5->p.Y<<", "<<p5->p.Z<<", "<<p5->p.H<<"), "
			<<"("<<p6->p.X<<", "<<p6->p.Y<<", "<<p6->p.Z<<", "<<p6->p.H<<"), "
			<<"("<<p7->p.X<<", "<<p7->p.Y<<", "<<p7->p.Z<<", "<<p7->p.H<<"), "
			<<"("<<p8->p.X<<", "<<p8->p.Y<<", "<<p8->p.Z<<", "<<p8->p.H<<"), "
			<<"Color: "<<(*it2)->color<<endl;
		it2++;
	}
				
	list<wmCircle*>::iterator it3 = this->circles->begin();
	while(it3 != circles->end())
	{
		p1 = getPoint((*it3)->p1);
		p2 = getPoint((*it3)->p2);
		cout<<"circle "<<(*it3)->id<<": "
			<<"("<<p1->p.X<<", "<<p1->p.Y<<", "<<p1->p.Z<<", "<<p1->p.H<<"), "
			<<"("<<p2->p.X<<", "<<p2->p.Y<<", "<<p2->p.Z<<", "<<p2->p.H<<"), "
			<<(*it3)->r1<<", "
			<<(*it3)->r2<<", "
			<<"Color: "<<(*it3)->color<<endl;
		it3++;
	}
}

string
WorldModel::getWorldModel(const Ice::Current& c)
{
	return this->sworld_model;
}

string 
WorldModel::getWorldModel()
{

	wmPoint *p1, *p2, *p3, *p4, *p5, *p6, *p7, *p8;
	stringstream out;

	list<wmLine*>::iterator it2 = this->lines->begin();
	while(it2 != lines->end())
	{
		p1 = getPoint((*it2)->p1);
		p2 = getPoint((*it2)->p2);
		p3 = getPoint((*it2)->p3);
		p4 = getPoint((*it2)->p4);
		p5 = getPoint((*it2)->p5);
		p6 = getPoint((*it2)->p6);
		p7 = getPoint((*it2)->p7);
		p8 = getPoint((*it2)->p8);
		out	<<"#L"
			<<":"<<p1->p.X<<":"<<p1->p.Y<<":"<<p1->p.Z<<":"<<p1->p.H
			<<":"<<p2->p.X<<":"<<p2->p.Y<<":"<<p2->p.Z<<":"<<p2->p.H
			<<":"<<p3->p.X<<":"<<p3->p.Y<<":"<<p3->p.Z<<":"<<p3->p.H
			<<":"<<p4->p.X<<":"<<p4->p.Y<<":"<<p4->p.Z<<":"<<p4->p.H
			<<":"<<p5->p.X<<":"<<p5->p.Y<<":"<<p5->p.Z<<":"<<p5->p.H
			<<":"<<p6->p.X<<":"<<p6->p.Y<<":"<<p6->p.Z<<":"<<p6->p.H
			<<":"<<p7->p.X<<":"<<p7->p.Y<<":"<<p7->p.Z<<":"<<p7->p.H
			<<":"<<p8->p.X<<":"<<p8->p.Y<<":"<<p8->p.Z<<":"<<p8->p.H
			<<":"<<(*it2)->color;
		it2++;
	}

	list<wmCircle*>::iterator it3 = this->circles->begin();
	while(it3 != circles->end())
	{
		p1 = getPoint((*it3)->p1);
		p2 = getPoint((*it3)->p2);
		out<<"#C"
			<<":"<<p1->p.X<<":"<<p1->p.Y<<":"<<p1->p.Z<<":"<<p1->p.H
			<<":"<<p2->p.X<<":"<<p2->p.Y<<":"<<p2->p.Z<<":"<<p2->p.H
			<<":"<<(*it3)->r1
			<<":"<<(*it3)->r2
			<<":"<<(*it3)->color;
		it3++;
	}

	list<wmCorner*>::iterator it4 = this->corners->begin();
	while(it4 != corners->end())
	{
		p1 = getPoint((*it4)->p1);
		p2 = getPoint((*it4)->p2);
		out<<"#Corners"
			<<":"<<p1->p.X<<":"<<p1->p.Y<<":"<<p1->p.Z<<":"<<p1->p.H
			<<":"<<p2->p.X<<":"<<p2->p.Y<<":"<<p2->p.Z<<":"<<p2->p.H
			<<":"<<(*it4)->type;
		it4++;
	}

	string s = out.str();

	return s;
}

int
WorldModel::getCalibrationLines(wmLine3D * lines3d, int max)
{
	int count;
	wmPoint *p1, *p2, *p3, *p4, *p5, *p6, *p7, *p8;
	int numcirclelines = 12;

	/*Save lines*/
	count = 0;
	list<wmLine*>::iterator it2 = this->lines->begin();
	while(it2 != lines->end() && count<max)
	{
		p1 = getPoint((*it2)->p1);
		p2 = getPoint((*it2)->p2);
		p3 = getPoint((*it2)->p3);
		p4 = getPoint((*it2)->p4);
		p5 = getPoint((*it2)->p5);
		p6 = getPoint((*it2)->p6);
		p7 = getPoint((*it2)->p7);
		p8 = getPoint((*it2)->p8);

		lines3d[count].p1.X=p1->p.X;lines3d[count].p1.Y=p1->p.Y;lines3d[count].p1.Z=p1->p.Z;lines3d[count].p1.H=p1->p.H;
		lines3d[count].p2.X=p2->p.X;lines3d[count].p2.Y=p2->p.Y;lines3d[count].p2.Z=p2->p.Z;lines3d[count].p2.H=p2->p.H;
		lines3d[count].p3.X=p3->p.X;lines3d[count].p3.Y=p3->p.Y;lines3d[count].p3.Z=p3->p.Z;lines3d[count].p3.H=p3->p.H;
		lines3d[count].p4.X=p4->p.X;lines3d[count].p4.Y=p4->p.Y;lines3d[count].p4.Z=p4->p.Z;lines3d[count].p4.H=p4->p.H;
		it2++;
		count++;
	}

	/*Convert circles to lines*/
	list<wmCircle*>::iterator it3 = this->circles->begin();
	while(it3 != circles->end() && count<max)
	{
		p1 = getPoint((*it3)->p1);
		p2 = getPoint((*it3)->p2);
		if((*it3)->r2 == 0.0) {
			/*Goal post*/
			lines3d[count].p1.X=p1->p.X;lines3d[count].p1.Y=p1->p.Y;lines3d[count].p1.Z=p1->p.Z;lines3d[count].p1.H=p1->p.H;
			lines3d[count].p2.X=p2->p.X;lines3d[count].p2.Y=p2->p.Y;lines3d[count].p2.Z=p2->p.Z;lines3d[count].p2.H=p2->p.H;
			lines3d[count].p3.X=p1->p.X;lines3d[count].p3.Y=p1->p.Y;lines3d[count].p3.Z=p1->p.Z;lines3d[count].p3.H=p1->p.H;
			lines3d[count].p4.X=p2->p.X;lines3d[count].p4.Y=p2->p.Y;lines3d[count].p4.Z=p2->p.Z;lines3d[count].p4.H=p2->p.H;
			count++;	
		} else {
			/*Central circle*/
			if(count + numcirclelines <= max) {
				this->circle2Lines(p1, (*it3)->r1, (*it3)->r2, numcirclelines, lines3d, count);
				count += numcirclelines;
			}		
		}
		it3++;
	}

	return count;
}


void
WorldModel::circle2Lines(wmPoint *p, float r1, float r2, int num_segments, wmLine3D * lines3d, int init)
{
	int i;
	float x1, y1, x2, y2, theta;
	int count = init;
	int endpos;

	for(i=0; i<num_segments; i++) {
		/*Get the current angle*/
		theta = 2.0 * M_PI * ((float)i) / ((float)num_segments);

		x1 = r1 * cos(theta);
		y1 = r1 * sin(theta);
		x2 = r2 * cos(theta);
		y2 = r2 * sin(theta);

		/*Start point*/
		lines3d[count].p1.X=p->p.X+x1;lines3d[count].p1.Y=p->p.Y+y1;lines3d[count].p1.Z=p->p.Z;lines3d[count].p1.H=p->p.H;
		lines3d[count].p3.X=p->p.X+x2;lines3d[count].p3.Y=p->p.Y+y2;lines3d[count].p3.Z=p->p.Z;lines3d[count].p3.H=p->p.H;

		/*End point*/
		if(i==0)
			endpos = count+num_segments-1;
		else
			endpos = count-1;

		lines3d[endpos].p2.X=p->p.X+x1;lines3d[endpos].p2.Y=p->p.Y+y1;lines3d[endpos].p2.Z=p->p.Z;lines3d[endpos].p2.H=p->p.H;
		lines3d[endpos].p4.X=p->p.X+x2;lines3d[endpos].p4.Y=p->p.Y+y2;lines3d[endpos].p4.Z=p->p.Z;lines3d[endpos].p4.H=p->p.H;
	
		count++;			
	}
}
