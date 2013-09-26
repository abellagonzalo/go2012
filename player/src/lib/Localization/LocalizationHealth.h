#ifndef LOCALIZATIONHEALTH_H
#define LOCALIZATIONHEALTH_H

#include "Component.h"
#include "Singleton.h"
#include "Kinematics.h"
#include "ObservationModel.h"
#include "MovementModel.h"
#include "GameController.h"
#include "Geometry.h"
#include <math.h>

#ifndef LOC_PI
#define LOC_PI 3.1415926535897932384626433832795
#endif
#ifndef LOC_PI_2
#define LOC_PI_2 1.570796327
#endif

class LocalizationHealth : public Singleton<LocalizationHealth>
{
public:

	LocalizationHealth();
	~LocalizationHealth();
	
	void init();

	/*Get the health of a robot position*/
	double getHealth(float x, float y, float theta, TKinematics robotConfig, vector<TImageObject> objects);

	/*Get best positions*/
	void getBestPositions(vector<TImageObject> objects, vector<HPoint2D> &positions);

private:

	/*Health calculation*/
	double getDistanceLine(TPinHoleCamera * camera, HPoint2D p2d, HPoint3D p3d);
	double distancePoints(float xini, float yini, float xfin, float yfin);

	/*Best positions calculation*/
	void getPositionsPost(TImageObject post, vector<HPoint2D> &positions, int post_type, int team_type);
	bool getPositionXY(HPoint2D center, double radius, double angle, double &X, double &Y);

	/*Load distances*/
	void loadDistances();

	double getHealthPost(float x, float y, float theta, float post_x, float post_y, int post_color, int post_type, int team_type);
	double getHealthCircle(float x, float y, float theta, float circle_x, float circle_y);
	double getHealthMidLine(float x, float y, float theta, float midline_x, float midline_y);
	double calc_distance_prob(double robot_x, double robot_y, double obj_x, double obj_y, double real_obj_x, double real_obj_y);
	double calc_angle_prob(double robot_x, double robot_y, double robot_t, double obj_x, double obj_y, double real_obj_x, double real_obj_y);
	double calc_angle(double x1, double y1, double x2, double y2);
	void normalize_angle(double &angle);

	/*Field size*/
	static const int MAXY = 2025;
	static const int MAXX = 3025;

	/*Load Distances*/
	static const int XERROR_LINES = 1000;
	static const int YERROR_LINES = 1000;
	static const int JUMP_LINES = 25;
	static const int ROWS_DISTANCES_LINES = ((MAXX*2+XERROR_LINES*2)/JUMP_LINES);
	static const int COLS_DISTANCES_LINES = ((MAXY*2+YERROR_LINES*2)/JUMP_LINES);
	static const int DISTANCE_INFINITE = 10000;

	Kinematics	*_Kinematics;
	MovementModel	*_MovementModel;	
	GameController * _GameController;

	HPoint3D distancesLines[ROWS_DISTANCES_LINES][COLS_DISTANCES_LINES];
	float max_distance;
};

#endif
