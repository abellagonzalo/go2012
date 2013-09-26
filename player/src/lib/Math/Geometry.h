#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <iostream>
#include <cmath>
#include "progeo.h"

#define GEOMETRY_PI 3.1415926535897932384626433832795
#define GEOMETRY_PI_2 1.570796327

#define GEOMETRY_SQRT_2 1.414213562

#ifndef G_SQUARE
#define G_SQUARE(a) ( (a) * (a) )
#endif

/*Distance between two points in 2D*/
extern float calcDistance2D(int x1, int y1, int x2, int y2);
extern float calcDistance2D(float x1, float y1, float x2, float y2);
extern float calcDistance2D(HPoint2D p1, HPoint2D p2);

/*Distance between two points in 3D*/
extern float calcDistance3D(HPoint3D p1, HPoint3D p2);

/*Distance between two points in 2D in a concrete axis*/
extern float calcDistanceAxis(float x1, float y1, float x2, float y2, float alpha);
extern float calcDistanceAxis(HPoint3D p1, HPoint3D p2, float alpha);
extern float calcDistanceAxis(HPoint3D p1, HPoint3D p2, float cosa, float sina);

/*Calc the positive angle of a vector*/
extern float calcVectorAngle(float x1, float y1, float x2, float y2);

/*Calc a 2D vector from 2 2D points*/
extern void calcVector2D(HPoint2D p1, HPoint2D p2, HPoint3D &v);
extern void calcVector2D(HPoint3D p1, HPoint3D p2, HPoint3D &v);

/*Calc a 2D normal vector from 3 2D points (normal vector of p1-p2 in p3)*/
extern void calcNormalVector2D(HPoint2D p1, HPoint2D p2, HPoint2D p3, HPoint3D &v);
extern void calcNormalVector2D(HPoint3D p1, HPoint3D p2, HPoint3D p3, HPoint3D &v);

/*Calc intersection between 2 2D vectors in a 2D point*/
extern void calcIntersection2D(HPoint3D v1, HPoint3D v2, HPoint2D &p);
extern void calcIntersection2D(HPoint3D v1, HPoint3D v2, HPoint3D &p);

/*Return true if the angles of two vectors can be considerated parallel*/
extern bool areVectorsParallel(float alpha1, float alpha2, float threshold);

/*Return true if the projection of the point P is "inside" the segment A-B*/
extern bool isPointInsideLine(float px, float py, float ax, float ay, float bx, float by);

/*Return a point belonging to the vector A-B*/
extern void getPointFromVector(int &px, int &py, int ax, int ay, int bx, int by, float u);
extern void getPointFromVector(float &px, float &py, float ax, float ay, float bx, float by, float u);

/*Return the intersection between a circle (radius and P_central) and a vector*/
extern void calIntersectionCircleVector(HPoint3D v, HPoint3D p_c, float r, HPoint3D &int1, HPoint3D &int2);

/*Gaussian sum*/
extern float gaussianSum(float x);
extern float gaussianSum2(float x);

#endif
