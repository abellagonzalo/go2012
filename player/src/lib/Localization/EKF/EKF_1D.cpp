#include "EKF_1D.h"

EKF_1D::EKF_1D() : EKF()
{
	/*s->resize(1, 1);
	P->identity(1);

	P->sete(0, 0, M_PI * M_PI);

	t(0.0);*/

	pf = M_PI * M_PI;
	sf = 0.0;
}

EKF_1D::~EKF_1D()
{
}

void
EKF_1D::restart()
{
	EKF::restart();
/*
	P->sete(0, 0, M_PI * M_PI);

	t(0.0);*/
	pf = M_PI * M_PI;
	sf = 0.0;

}

void
EKF_1D::init(MatrixCM sinit, MatrixCM Pinit, double pos_x, double pos_y)
{
	this->pos_x = pos_x;
	this->pos_y = pos_y;

	*s = sinit;
	*P = Pinit;

	sf = sinit.e(0,0);
	pf = Pinit.e(0,0);
}

void
EKF_1D::predict(float movx, float movy, float movt)
{
	float lpf;

	float ounc = M_PI*sqrt(movx*movx+movy*movy)/200.0;
	float unc = movt * 0.2 + ounc;

	lpf = pf;

	pf = pf + (unc*unc);

	if(pf>(M_PI*M_PI)) pf = M_PI*M_PI;
	t(normalizePi(t() + movt));
}

void
EKF_1D::correct(float obs, MatrixCM R)
{
	float q;

	MatrixCM v(1, 1);
	MatrixCM H(1, 1);
	MatrixCM S;
	MatrixCM K;
	MatrixCM I;
	MatrixCM o;

	o.identity(1);
	I.identity(1);
	K.identity(1);
	S.identity(1);
	H.identity(1);

	v.sete(0, 0, obs-t());

	S = H * (*P) * transpose(H) + R;
	K = (*P) * transpose(H) * inverse(S);

	*s = (*s) + K * v;

	*P = (I - K * H) * (*P);
	checkLimits();

}


void
EKF_1D::correct(HPoint3D &lm, HPoint3D &obs, MatrixCM R)
{

	float thetam = atan2(obs.Y, obs.X);
	float thetav = normalizePi(atan2(lm.Y - pos_y, lm.X - pos_x) - t());

	float k = pf/(pf+R.e(0,0));
	sf = sf + k * normalizePi(thetav - thetam);

	pf = (1.0-k) * pf;
	checkLimits();
}

void
EKF_1D::checkLimits()
{
	if (t() > M_PI)
		t(t() - 2 * M_PI);
	if (t() < -M_PI)
		t(t() + 2 * M_PI);
}
