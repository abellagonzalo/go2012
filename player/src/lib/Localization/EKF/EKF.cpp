#include "EKF.h"

EKF::EKF()
{
	s = new MatrixCM();
	P = new MatrixCM();

}

EKF::~EKF()
{
	delete s;
	delete P;
}

void
EKF::restart()
{
	s->clear();
	P->identity(P->nrows());
}

void
EKF::restart(MatrixCM sinit, MatrixCM Pinit)
{
	*P = Pinit;
	*s = sinit;
}

void
EKF::init(MatrixCM sinit, MatrixCM Pinit)
{
	*s = sinit;
	*P = Pinit;
}


void
EKF::getState(MatrixCM &state)
{
	state = *s;
}


