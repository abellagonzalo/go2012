#ifndef GENETICLOCALIZATION_H
#define GENETICLOCALIZATION_H

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

typedef struct {
	float x;
	float y;
	float theta;
	float prob;
	int elitist;
	int valid;
	int isnew;
	int wins;
	int life;
} TIndividual;

class GeneticLocalization : public AbstractLocalization, public Singleton<GeneticLocalization>
{
public:

	GeneticLocalization();
	~GeneticLocalization();

	void init(const string newName, AL::ALPtr<AL::ALBroker> parentBroker);

	void step();

	float getReliability();
	void getPosition(double &xpos, double &ypos, double &thetapos);
	void getPosition(vector<tRobotHipothesis> &hipotheses);
	void resetToPosition(int state);
	void addPosition(float x, float y, float theta);

	/*Debug*/
	//ALValue getImgDebug();
	ImageDataPtr getImgDebug();
	string getInfo();

	void setInfo(MotionInfo &motion, PerceptionInfo &perception);
	void UpdateInfo();
	void run();

private:

	/*Movement model*/
	void moveRaces();
	void checkValidIndividual(TIndividual * individual);

	/*Genetic*/
	void initGenetic();
	void saveState();
	void iterateGenetic();
	void createNewRaces();
	void createExploiters(int index, TIndividual mainindividual);
	void saveNewRace(TIndividual mainindividual);
	void evolutionRace();
	void evolutionExploiters(int index, TIndividual mainindividual);
	void calcRacesHealth();
	void calcExploitersHealth();
	int similarRaces(TIndividual race1, TIndividual race2);
	void fusionRaces();
	void deleteRaces();
	float getMinProbRace();
	void saveElitist(TIndividual * elitist, TIndividual candidate, int size);
	float distancePoints(float xini, float yini, float xfin, float yfin);
	void calcRaceWinner();
	void updateReliability(int index);

	/*Explorers List */
	void evolutionExplorers();
	void insertPart(float x, float y, float theta);
	void saveListExplorers();

	static const int MY_FREQ = 500;

	/*Genetic Algorithm*/
	static const int MAXPOINTSIMG = 100;
	static const int NUMRACES = 4;
	static const int NUMEXPLOITERS = 30;
	static const int MAXWINS = 20;
	static const int RACELIFE = 3;
	static const int MAX_DISTANCE_WINNER = 2000;
	static const int EXPLORERS_ITS = 4;
	static const int NUMEXPLORERS = 6;

	Perception	 		*_Perception;
	Kinematics 			*_Kinematics;
	Body 				*_Body;
	FSR 				*_FSR;
	MovementModel		*_MovementModel;
	ObservationModel	*_ObservationModel;
	LocalizationHealth	*_LocalizationHealth;
	LineDetector 	*_LineDetector;
	GoalDetector 	*_GoalDetector;

	/*State vectors*/
	int validState;
	bool hasGoals;
	TKinematics robotConfig;
	vector<TImageObject> objects;

	float reliability;

	/*Movement*/
	float maxmov_pos;
	float maxmov_angle;

	/*Explorers */
	int create_races;
	TIndividual explorers[NUMEXPLORERS];
	list<TIndividual> list_explorers;

	/*Others*/
	TIndividual races[NUMRACES];
	TIndividual exploiters[NUMRACES][NUMEXPLOITERS];
	int isInitGenetic;
	int numraces;
	float minimal_race_prob;
	int lastWinner;
	int valid_observations;
	int auxtime;

	int races_id;
};

#endif
