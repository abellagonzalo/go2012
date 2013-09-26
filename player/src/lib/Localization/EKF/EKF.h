#ifndef EKF_H
#define EKF_H

#include "Matrix.h"

using namespace std;

class EKF
{
public:

	EKF();
	~EKF();

	void init(MatrixCM sinit, MatrixCM Pinit);

	virtual void restart() = 0;
	void restart(MatrixCM sinit, MatrixCM Pinit);

	void getState(MatrixCM &state);

	MatrixCM *get_s() {return s;};
	MatrixCM *get_P() {return P;};

protected:

	MatrixCM *s;
	MatrixCM *P;
};

#endif
