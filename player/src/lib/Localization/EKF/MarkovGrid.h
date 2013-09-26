#ifndef MARKOVGRID_H
#define MARKOVGRID_H

#include "EKF_1D.h"
#include "Pose2D.h"
#include "Common.h"
#include <list>

#include "debug.h"

#define CELL_SIZE 800.0

typedef struct
{
	double pos_x, pos_y;
	float prob;
	EKF_1D *theta;
}tgridcell;

typedef struct
{
	int i,j;
	float prob;
	float uxy;
	float ut;
	float x,y,t;
	float covered;
}tcellcand;

using namespace std;

class MarkovGrid
{
public:

	MarkovGrid();
	~MarkovGrid();

	void predict(float movx, float movy, float movt);
	void correct(HPoint2D &perceived_pos, HPoint2D &official_pos, float angStdev, float disStdev, bool ambiguous=false);
	void correct(HPoint3D &pos, MatrixCM R);

	//bool Correct(HPoint3D &lm, HPoint3D &obs, MatrixCM R);
	//bool Correct(HPoint3D &obs, MatrixCM R);

	float getProbIJ(int i, int j);
	float getProbXY(float x, int y);
	float getThetaIJ(int i, int j);
	float getThetaXY(float x, int y);

	void restart();
	void restart(list<Pose2D> &initPoses);
	void addPosition(float x, float y, float theta);

	void init();
	void resetUniform();
	void resetClear();
	void normalize();

	float evaluate(float x, float y, float t);

	void getGridPos(list<tcellcand> &cands);
	tgridcell *getCell(float x, float y);

	float getCellSize() { return CELL_SIZE;};

	void addDebugInfo(bica::ShapeList &lista);

	int getRows() {return gridrows;};
	int getColumns() {return gridcolumns;};

private:
	void FromGridtoCoords(int i, int j, float &x, float &y);
	void ToGridfromCoords(float x, float y, int &i, int &j);

	float getLandmarkProb(float x, float y, HPoint2D &obs, HPoint2D &lm, float dist_std_dev);
	float getProb(float x, float std_dev);


	tgridcell **grid;
	int gridrows, gridcolumns;
	float quality;

	float nangles[9];
};

#endif
