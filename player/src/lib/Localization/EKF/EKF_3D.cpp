#include "EKF_3D.h"

EKF_3D::EKF_3D() : EKF()
{
	s->resize(3, 1);
	P->identity(3);

	P->sete(0, 0, 2000.0 * 2000.0);
	P->sete(1, 1, 2000.0 * 2000.0);
	P->sete(2, 2, 1.0 * 1.0);

	x(0.0);
	y(0.0);
	t(0.0);

	for(int i=0; i<obsbuffersize;i++)
		obscounter[i] = 0;
	idxcounter = 0;
}

EKF_3D::~EKF_3D()
{
	//EKF::~EKF();

}

void
EKF_3D::restart()
{
	EKF::restart();

	P->sete(0, 0, 3000.0 * 3000.0);
	P->sete(1, 1, 2000.0 * 2000.0);
	P->sete(2, 2, 2.0 * 2.0);

	x(0.0);
	y(0.0);
	t(0.0);

	for(int i=0; i<obsbuffersize;i++)
		obscounter[i] = 0.0;
}

void
EKF_3D::restart(MatrixCM sinit, MatrixCM Pinit)
{
	EKF::restart(sinit, Pinit);

	for(int i=0; i<obsbuffersize;i++)
		obscounter[i] = 0.0;
}

void
EKF_3D::init(MatrixCM sinit, MatrixCM Pinit)
{
	*s = sinit;
	*P = Pinit;
}

void
EKF_3D::predict(float movx, float movy, float movt)
{

	float newx, newy, newt;

	newx = x();
	newy = y();
	newt = t();

	MatrixCM A;
	A.identity(3);
	A.sete(0, 2, -movy * cos(t()) - movx * sin(t()));
	A.sete(1, 2, movx * cos(t()) - movy * sin(t()));

	MatrixCM W;
	W.identity(3);
	W.sete(0, 0, cos(t()));
	W.sete(0, 1, -sin(t()));
	W.sete(1, 0, sin(t()));
	W.sete(1, 1, cos(t()));

	//Valores provisionales antes de calcularlos
	MatrixCM Q;
	Q.identity(3);
	Q.sete(0, 0, (movx * 0.3) * (movx * 0.3));
	Q.sete(1, 1, (movy * 0.3) * (movy * 0.3));
	Q.sete(2, 2, (movt * 0.6) * (movt * 0.6));

	*P = A * (*P) * transpose(A) + W * Q * transpose(W);


	x(x() + movx * cos(t()) - movy * sin(t()));
	y(y() + movx * sin(t()) + movy * cos(t()));
	//Checkear si movt es correcto, ya que parece que no es necesario normalizar...
	t(normalizePi(t() + movt));

}

bool //Todo
EKF_3D::correct(HPoint3D &obs, MatrixCM R)
{
	float q;
	double ov;

	MatrixCM v(3, 1);
	MatrixCM H(3, 3);
	MatrixCM S;
	MatrixCM K;
	MatrixCM I;
	MatrixCM o;

	o.identity(1);
	I.identity(3);
	K.identity(3);
	S.identity(3);
	H.identity(3);

	v.sete(0, 0, obs.X-x());
	v.sete(1, 0, obs.Y-y());
	v.sete(2, 0, obs.Z-t());

	S = H * (*P) * transpose(H) + R;
	K = (*P) * transpose(H) * inverse(S);

	o = transpose(v) * inverse(S) * v;

	ov = exp(-0.5 * (double)o.e(0,0));

	//cerr<<"ov = "<<ov<<endl;

	if(ov>0.0)
	{
		//cerr<<"EKF_3D ("<<x()<<","<<y()<<","<<t()*180.0/M_PI<<") +  ";

		//cerr <<"["<<v.e(0,0)<<","<<v.e(1,0)<<","<<v.e(2,0)*180.0/M_PI<<"] -> ";
		*s = (*s) + K * v;

		//cerr<<"________________"<<endl<<"R = ";
		//R.print();
		//cerr<<"P = ";

		//cerr<<"("<<x()<<","<<y()<<","<<t()*180.0/M_PI<<")"<<endl;
		*P = (I - K * H) * (*P);
		//P->print();
		//cerr<<"_______________"<<endl;;

		checkLimits();

		obscounter[idxcounter] += ov;
	}

	return ov;
}


bool
EKF_3D::correct(HPoint3D &lm, HPoint3D &obs, MatrixCM R)
{
	float q;
	double ov;

	MatrixCM v(2, 1);
	MatrixCM H(2, 3);
	MatrixCM S;
	MatrixCM K;
	MatrixCM I;
	MatrixCM o;

	o.identity(1);
	I.identity(3);
	K.identity(3);
	S.identity(2);

	float rhom = sqrt(obs.X * obs.X + obs.Y * obs.Y);
	float thetam = atan2(obs.Y, obs.X);

	float rhov = sqrt(powf(lm.X - x(), 2.0) + powf(lm.Y - y(), 2.0));
	float thetav = normalizePi(atan2(lm.Y - y(), lm.X - x()) - t());

	v.sete(0, 0, rhom - rhov);
	v.sete(1, 0, thetam - thetav);


	q = powf(lm.X - x(), 2.0) + powf(lm.Y - y(), 2.0);
	if (q != 0) {
		H.sete(0, 0, -(lm.X - x()) / sqrt(q));
		H.sete(0, 1, -(lm.Y - y()) / sqrt(q));
		H.sete(0, 2, 0.0);
		H.sete(1, 0, (lm.Y - y()) / q);
		H.sete(1, 1, -(lm.X - x()) / q);
		H.sete(1, 2, -1.0);
	} else {
		H.sete(0, 0, 0.0);
		H.sete(0, 1, 0.0);
		H.sete(0, 2, 0.0);
		H.sete(1, 0, 0.0);
		H.sete(1, 1, 0.0);
		H.sete(1, 2, -1.0);
	}

	S = H * (*P) * transpose(H) + R;
	K = (*P) * transpose(H) * inverse(S);

	o = transpose(v) * inverse(S) * v;

	ov = exp(-0.5 * (double)o.e(0,0));


	//cerr<<"("<<x()<<","<<y()<<") = "<<ov<<endl;
	if(ov>0.001)
	{

		*s = (*s) + K * v;
		*P = (I - K * H) * (*P);

		checkLimits();

		obscounter[idxcounter] += ov;
	}

	return ov;
}

void
EKF_3D::checkLimits()
{
	if (t() > M_PI)
		t(t() - 2 * M_PI);
	if (t() < -M_PI)
		t(t() + 2 * M_PI);
	if (x() > 3000.0)
		x(3000.0);
	if (x() < -3000.0)
		x(-3000.0);
	if (y() > 2000.0)
		y(2000.0);
	if (y() < -2000.0)
		y(-2000.0);
}

string
EKF_3D::getDbgInfo()
{
	std::stringstream out;

	float Q = (float)getObsCounter()/((float)(obsbuffersize-1)*3.0);

	//cerr<<"Q = "<<Q<<endl;
	if(Q>1.0) Q = 1.0;

	out<<s->e(0, 0) << ":" << s->e(1, 0) << ":" << s->e(2, 0) << ":"
			<< P->e(0, 0) << ":" << P->e(0, 1) << ":"
			<< P->e(0, 2) << ":" << P->e(1, 0) << ":" << P->e(1, 1) << ":"
			<< P->e(1, 2) << ":" << P->e(2, 0) << ":" << P->e(2, 1) << ":" << P->e(2, 2)<< ":"<<getObsCounter()<<":";
	string s = out.str();

	return s;
}

float
EKF_3D::getObsCounter()
{
	float sum=0.0;
	int i;
	int cont;

	/*cerr<<"[";
	for(i=0; i<efksmax; i++)
	{
		cerr<<obscounter[i]<<", ";
	}
	cerr<<"]"<<endl;*/
	cont = obsbuffersize + 1;
	i = idxcounter+1;




	while(i!=idxcounter)
	{
		sum = sum + obscounter[i] * (cont--);
		i = (i+1) % (obsbuffersize);
	}

	return sum;
}

void
EKF_3D::changeCycle()
{
	idxcounter = (idxcounter+1) % obsbuffersize;
	obscounter[idxcounter] = 0.0;
}

