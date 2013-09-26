#ifndef WM_H
#define WM_H

#include "Singleton.h"
#include "progeo.h"
#include "Common.h"
#include "almath/tools/almath.h"
#include "LineDetector.h"
#include <list>
#include <map>
#include <string>

#include <IceE/IceE.h>
#include <localizationI.h>

using namespace std;
using namespace bica;

#define WORLD_MODEL_CONFIG_FILE "/home/nao/bica/conf/wm.cfg"

typedef struct
{
	string id;
	HPoint3D p;	
}wmPoint;

typedef struct
{
	string id;
	string p1;
	string p2;
	string p3;
	string p4;
	string p5;
	string p6;
	string p7;
	string p8;	
	string color;
	float angle; //to speed-up the search for matching lines
	float lenght;
	float A;
	float B;
	float C;
}wmLine;


typedef struct
{
	string id;
	string p1;
	string p2;
	int type;
}wmCorner;


typedef struct
{
	string id;
	string p1;
	string p2;
	float r1;
	float r2;
	string color;
}wmCircle;

typedef struct
{
	HPoint3D p1;
	HPoint3D p2;
	HPoint3D p3;
	HPoint3D p4;
	HPoint3D p5;
	HPoint3D p6;
	HPoint3D p7;
	HPoint3D p8;	
	string color;
}wmLine3D;

class WorldModel: public Singleton<WorldModel>, public WorldModelProvider
{
public:
	WorldModel();
	~WorldModel();

	void init();

	wmPoint *getPoint(string id);
	wmLine *getLine(string id);
	wmCircle *getCircle(string id);
	wmCorner *getCorner(string id);
	list<wmCorner*> *getCornerType(int type); //devuelve los corners X, T o L

	virtual string getWorldModel(const Ice::Current& c);
	string getWorldModel();
	int getCalibrationLines(wmLine3D * lines3d, int max);

	list<wmLine*> *getLines() {return lines;};

	void print();
private:

	void loadPoints(string file);
	void loadLines(string file);
	void loadCircles(string file);
	void loadCorners(string file);

	void circle2Lines(wmPoint *p, float r1, float r2, int num_segments, wmLine3D * lines3d, int init);

	list<wmPoint*> 	*points;
	list<wmLine*> 	*lines;
	list<wmCircle*> *circles;
	list<wmCorner*> *corners;
	string sworld_model;
};

#endif
