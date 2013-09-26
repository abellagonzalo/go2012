#ifndef EKF_3D_H
#define EKF_3D_H

#include "EKF.h"
#include "progeo.h"
#include "Common.h"
#include <string.h>
#include <math.h>
#include "almath/tools/almath.h"
#include <sstream>
#include <iostream>
#include "Pose2D.h"

using namespace std;

class EKF_3D;

typedef struct
{
	Pose2D estimada;
	Pose2D instantanea;
}tdbg;

typedef struct
{
	EKF_3D *filter;
	int id;
}tEKF;

class EKF_3D : public EKF
{
public:

	EKF_3D();
	~EKF_3D();

	void init(MatrixCM sinit, MatrixCM Pinit);

	void predict(float movx, float movy, float movt);
	bool correct(HPoint3D &lm, HPoint3D &obs, MatrixCM R);
	bool correct(HPoint3D &obs, MatrixCM R);

	void restart();

	void restart(MatrixCM sinit, MatrixCM Pinit);

	string getDbgInfo();

	double x() {return s->e(0,0);};
	double y() {return s->e(1,0);};
	double t() {return s->e(2,0);};

	double Px() {return sqrt(P->e(0,0));};
	double Py() {return sqrt(P->e(1,1));};
	double Pt() {return sqrt(P->e(2,2));};

	float getObsCounter();
	void changeCycle();



	static const int obsbuffersize = 10;
private:

	void x(double v) {return s->sete(0,0,v);};
	void y(double v) {return s->sete(1,0, v);};
	void t(double v) {return s->sete(2,0, v);};

	void checkLimits();

	float obscounter[obsbuffersize];
	int idxcounter;
};

#endif
