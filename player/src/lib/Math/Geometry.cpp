#include "Geometry.h"

float
calcDistance2D(int x1, int y1, int x2, int y2)
{
	return sqrt((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1));
}

float
calcDistance2D(float x1, float y1, float x2, float y2)
{
	return sqrt((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1));
}

float
calcDistance2D(HPoint2D p1, HPoint2D p2)
{
	return sqrt((p2.x-p1.x)*(p2.x-p1.x) + (p2.y-p1.y)*(p2.y-p1.y));
}

float
calcDistance3D(HPoint3D p1, HPoint3D p2)
{
	return sqrt((p2.X-p1.X)*(p2.X-p1.X) + (p2.Y-p1.Y)*(p2.Y-p1.Y) + (p2.Z-p1.Z)*(p2.Z-p1.Z));
}

float
calcDistanceAxis(float x1, float y1, float x2, float y2, float alpha)
{
	float dist;

	dist = (x2 - x1)*cos(alpha) + (y2 - y1)*sin(alpha);

	return fabs(dist);
}

float
calcDistanceAxis(HPoint3D p1, HPoint3D p2, float alpha)
{
	float dist;

	dist = (p2.X - p1.X)*cos(alpha) + (p2.Y - p1.Y)*sin(alpha);

	return fabs(dist);
}

float
calcDistanceAxis(HPoint3D p1, HPoint3D p2, float cosa, float sina)
{
	float dist;

	dist = (p2.X - p1.X)*cosa + (p2.Y - p1.Y)*sina;

	return fabs(dist);
}

float
calcVectorAngle(float x1, float y1, float x2, float y2)
{
	float diffx, diffy;
	float alpha;

	diffx = x2 - x1;
	diffy = y2 - y1;

	if(diffx == 0.0)
		return GEOMETRY_PI_2;

	alpha = atan(diffy/diffx);

	/*Normalize*/
	if(alpha < 0)
		alpha += GEOMETRY_PI;
	if(alpha > GEOMETRY_PI)
		alpha -= GEOMETRY_PI;	

	return alpha;		
}

void
calcVector2D(HPoint2D p1, HPoint2D p2, HPoint3D &v)
{
	/*Get the Ax + By + C = 0 parameters*/
	v.X = p1.y - p2.y; 				//y1*z2 - z1*y2
	v.Y = p2.x - p1.x; 				//z1*x2 - x1*z2
	v.Z = p1.x*p2.y - p1.y*p2.x; 	//x1*y2 - y1*x2
	v.H = 1.0;
}

void
calcVector2D(HPoint3D p1, HPoint3D p2, HPoint3D &v)
{
	/*Get the Ax + By + C = 0 parameters*/
	v.X = p1.Y - p2.Y; 				//y1*z2 - z1*y2
	v.Y = p2.X - p1.X; 				//z1*x2 - x1*z2
	v.Z = p1.X*p2.Y - p1.Y*p2.X; 	//x1*y2 - y1*x2
	v.H = 1.0;
}

void
calcNormalVector2D(HPoint2D p1, HPoint2D p2, HPoint2D p3, HPoint3D &v)
{
	HPoint3D vtmp;
	float nA, nB, nC;

	/*Get the vector p1-p2*/
	calcVector2D(p1, p2, vtmp);

	/*Calc the normal*/
	nA = vtmp.Y;
	nB = -vtmp.X;
	nC = -(p3.x*nA + p3.y*nB); //Solve equation Ax+By+C with central point

	v.X = nA;
	v.Y = nB;
	v.Z = nC;	
	v.H = 1.0;
}

void
calcNormalVector2D(HPoint3D p1, HPoint3D p2, HPoint3D p3, HPoint3D &v)
{
	HPoint3D vtmp;
	float nA, nB, nC;

	/*Get the vector p1-p2*/
	calcVector2D(p1, p2, vtmp);

	/*Calc the normal*/
	nA = vtmp.Y;
	nB = -vtmp.X;
	nC = -(p3.X*nA + p3.Y*nB); //Solve equation Ax+By+C with central point

	v.X = nA;
	v.Y = nB;
	v.Z = nC;	
	v.H = 1.0;
}

void
calcIntersection2D(HPoint3D v1, HPoint3D v2, HPoint2D &p)
{
	float h;

	h = v1.X*v2.Y - v1.Y*v2.X; 		/*x1*y2 - y1*x2*/

	/*Are parallel*/	
	if(h==0)
		p.h = 0.0;

	p.x = (v1.Y*v2.Z - v2.Y*v1.Z)/h; /*y1*z2 - z1*y2*/
	p.y = (v1.Z*v2.X - v1.X*v2.Z)/h; /*z1*x2 - x1*z2*/
	p.h = 1.0;
}

void
calcIntersection2D(HPoint3D v1, HPoint3D v2, HPoint3D &p)
{
	float h;

	h = v1.X*v2.Y - v1.Y*v2.X; 		/*x1*y2 - y1*x2*/

	/*Are parallel*/	
	if(h==0)
		p.H = 0.0;

	p.X = (v1.Y*v2.Z - v2.Y*v1.Z)/h; /*y1*z2 - z1*y2*/
	p.Y = (v1.Z*v2.X - v1.X*v2.Z)/h; /*z1*x2 - x1*z2*/
	p.Z = 0.0;
	p.H = 1.0;
}

bool
areVectorsParallel(float alpha1, float alpha2, float threshold)
{
	float diff;

	diff = alpha2 - alpha1;

	/*Normalize*/
	while(diff < -GEOMETRY_PI_2)
		diff += GEOMETRY_PI;
	while(diff > GEOMETRY_PI_2)
		diff -= GEOMETRY_PI;

	return fabs(diff) < threshold;
}

bool
isPointInsideLine(float px, float py, float ax, float ay, float bx, float by)
{

	float tmp;

	/*Get the u parameter in the equation P = A+u(B-A)*/
	/*	(Px-Ax)(Bx-Ax) + (Py-Ay)(By-Ay)
	u = -------------------------------
			(Bx-Ax)^2 + (By-Ay)^2*/

	/*Check if the line is a point*/
	if(ax == bx && ay == by)
		return 0;

	tmp = (px-ax)*(bx-ax) + (py-ay)*(by-ay);
	tmp = tmp /(pow(bx-ax,2) + pow(by-ay,2));

	if(tmp >=0 && tmp <=1)
		return 1;

	return 0;	
}

void
getPointFromVector(int &px, int &py, int ax, int ay, int bx, int by, float u)
{
	/*Get P from the equation P = A+u(B-A)*/
	px = (int)((float)ax + u*(float)(bx-ax));
	py = (int)((float)ay + u*(float)(by-ay));
}

void
getPointFromVector(float &px, float &py, float ax, float ay, float bx, float by, float u)
{
	/*Get P from the equation P = A+u(B-A)*/
	px = ax + u*(bx-ax);
	py = ay + u*(by-ay);
}

void
calIntersectionCircleVector(HPoint3D v, HPoint3D p_c, float r, HPoint3D &int1, HPoint3D &int2)
{
	/*Solve equations:
	(x-px)^2 + (y - py)^2 = r^2
	Ax + By + C = 0*/

	float i,j,k, A_2;
	float a,b,c;
	float tmp;

	if(v.X == 0.0) {
		/*Avoid div by 0*/
		v.X = 0.000001;
	}

	i = -2*p_c.X;
	j = -2*p_c.Y;
	k = G_SQUARE(p_c.X) + G_SQUARE(p_c.Y) - G_SQUARE(r);

	A_2 = G_SQUARE(v.X);
	a = G_SQUARE(-v.Y)/A_2 + 1;
	b = -2*v.Z*-v.Y/A_2  - v.Y*i/v.X + j;
	c = G_SQUARE(v.Z)/A_2 - v.Z*i/v.X + k;	

	/*Solve a*Y^2 + b+Y + c = 0*/
	tmp = G_SQUARE(b) - 4*a*c;
	if(tmp<0) {
		/*No intersection*/
		int1.H = 0.0;
		int2.H = 0.0;	
		return;	
	}

	tmp = sqrt(tmp);
	int1.Y = (-b + tmp)/(2*a);
	int2.Y = (-b - tmp)/(2*a);

	/*Get X Coordinate*/
	int1.X = (-v.Y*int1.Y - v.Z)/v.X;
	int2.X = (-v.Y*int2.Y - v.Z)/v.X;	

	int1.H = 1.0;
	int2.H = 1.0;
}

float
gaussianSum(float x)
{
	return x * (x + 1.0)/2.0;
}

float
gaussianSum2(float x)
{
	return x * (x + 1.0) * (2.0*x + 1.0)/6.0;
}

