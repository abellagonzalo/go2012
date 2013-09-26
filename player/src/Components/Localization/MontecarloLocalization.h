#ifndef MONTECARLOLOCATION_H
#define MONTECARLOLOCATION_H

#include "AbstractLocalization.h"
#include "Singleton.h"
#include "Perception.h"
#include "ImageInput.h"
#include "Kinematics.h"
#include "Body.h"
#include "FSR.h"
#include "progeo.h"
#include "Matrix.h"
#include "almath/tools/almath.h"
#include "vision/VisionDefinitions.h"
#include "MovementModel.h"
#include "ObservationModel.h"
#include "LocalizationHealth.h"
#include "LineDetector.h"
#include "GoalDetector.h"
#include <pthread.h>
#include "image.h"

typedef struct {
	float x;
	float y;
	float theta;
	float prob;
	int elitist;
} TParticle;

typedef struct {
	float low;
	float accumulation;
	float x;
	float y;
	float theta;
} TAccumulation;

class MontecarloLocalization : public AbstractLocalization, public Singleton<MontecarloLocalization>
{
public:

	MontecarloLocalization();
	~MontecarloLocalization();

	void init(const string newName, AL::ALPtr<AL::ALBroker> parentBroker);

	void step();

	float getReliability();
	void getPosition(double &xpos, double &ypos, double &thetapos);
	void getPosition(vector<tRobotHipothesis> &hipotheses);


	void resetToPosition(int state);

	/*Debug*/
	//ALValue getImgDebug();
	bica::ImageDataPtr getImgDebug();
	string getInfo();

	void setInfo(MotionInfo &motion, PerceptionInfo &perception);
	void UpdateInfo();
	void run();

private:

	/*Movement model*/
	void moveParticles();
	void checkValidParticle(TParticle * particle);

	/*Genetic*/
	void initMontecarlo();
	void saveState();
	void iterateMontecarlo();	
	void calcParticlesHealth();
	void saveElitist(TParticle * elitist, TParticle candidate, int size);
	void resampling();
	int binary_search(float number);
	void updateReliability();

	static const int MY_FREQ = 500;

	/*Field size*/
	static const int MAXY = 2025;
	static const int MAXX = 3025;

	/*Genetic Algorithm*/
	static const int MAXPOINTSIMG = 100;
	static const int NUMPART = 200;
	static const int NUMELIT = 20;

	Perception	 		*_Perception;
	Kinematics 			*_Kinematics;
	Body 				*_Body;
	FSR 				*_FSR;
	MovementModel		*_MovementModel;
	ObservationModel	*_ObservationModel;
	LocalizationHealth	*_LocalizationHealth;
	LineDetector 		*_LineDetector;
	GoalDetector 		*_GoalDetector;

	/*State vectors*/
	int validState;
	bool hasGoals;
	TKinematics robotConfig;
	vector<TImageObject> objects;

	float reliability;

	/*Movement*/
	float maxmov_pos;
	float maxmov_angle;

	/*Others*/
	TParticle particles[NUMPART];
	TParticle mainParticle;
	TAccumulation accumulation[NUMPART];
	int isInitMontecarlo;
	int auxtime;
};

#endif
