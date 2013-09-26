#ifndef EKF_1D_H
#define EKF_1D_H

#include "EKF.h"
#include "progeo.h"
#include "Common.h"

#include <string.h>
#include <math.h>
#include "almath/tools/almath.h"
#include <sstream>
#include <iostream>

using namespace std;

class EKF_1D : public EKF
{
public:

	EKF_1D();
	~EKF_1D();

	void init(MatrixCM sinit, MatrixCM Pinit, double pos_x, double pos_y);

	void predict(float movx, float movy, float movt);
	void correct(HPoint3D &lm, HPoint3D &obs, MatrixCM R);
	void correct(float obs, MatrixCM R);

	void restart();

	double t() {return sf;/*s->e(0,0);*/};
	double ut() {return pf;/*P->e(0,0);*/};

	void t(double newt) {sf = newt;/*s->e(0,0);*/};
	void ut(double newu) {pf = newu;/*P->e(0,0);*/};

private:

	double pos_x, pos_y;

	void checkLimits();

	float sf,pf;

};

#endif
