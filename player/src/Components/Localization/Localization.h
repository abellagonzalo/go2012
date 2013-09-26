#ifndef LOCALIZATION_H
#define LOCALIZATION_H

#include "Component.h"
#include "Singleton.h"
#include "AbstractLocalization.h"
#include "GeneticLocalization.h"
#include "MontecarloLocalization.h"
#include "GTLocalization.h"
#include "SRL.h"
#include "NEKFLocalization.h"
#include "EKFLocalization.h"
#include "MovementModel.h"
#include "ObservationModel.h"
#include "LocalizationHealth.h"
#include <IceE/IceE.h>
#include <image.h>

using namespace bica;

class Localization : public Component, public Singleton<Localization>, public LocalizationProvider
{
public:

	Localization();
	~Localization();

	void init(const string newName, AL::ALPtr<AL::ALBroker> parentBroker);

	void step();

	/*Set the localization algorithm to use*/
	void setLocalization(string type);

	/*Get the real position of the robot*/
	void getRealPosition(double &xpos, double &ypos, double &thetapos);

	/*Get the real position of the robot*/
	string getRealPosition(const Ice::Current& c);

	/*Set the real position of the robot*/
	void setRealPosition(double xpos, double ypos, double thetapos);

	/*Set the real position of the robot*/
	void setRealPosition(double xpos, double ypos, double thetapos, const Ice::Current& c);	

	/*Get the current position of the robot*/
	void getPosition(double &xpos, double &ypos, double &thetapos);
	void getPosition(vector<tRobotHipothesis> &hipotheses);

	/*Get the reliability of the current localization*/
	float getReliability(const Ice::Current& c);

	/*Get the reliability of the current localization*/
	float getReliability();

	/*Reset the localization algorithm depending on the gamestate*/
	void resetToPosition(int state);

	/*Reset the localization algorithm*/
	void resetLocalization();

	/*Reset the localization algorithm*/
	void resetLocalization(const Ice::Current& c);

	/*Get information of the localization algorithm*/
	string getLocalizationInfo(const Ice::Current& c);

	/*Get an image to debug the observation model*/
	//ALValue getLocalizationImg();

	/*Get an image to debug the observation model*/
	virtual bica::ImageDataPtr getLocalizationImg(const Ice::Current& c);

	void setInfo(MotionInfo &motion, PerceptionInfo &perception);
	void run();

private:

	GeneticLocalization 	*_GeneticLocalization;
	MontecarloLocalization 	*_MontecarloLocalization;
	GTLocalization 			*_GTLocalization;
	NEKFLocalization 		*_NEKFLocalization;
	EKFLocalization 		*_EKFLocalization;
	SRL 					*_SRL;
	MovementModel 			*_MovementModel;
	ObservationModel		*_ObservationModel;
	LocalizationHealth 		*_LocalizationHealth;

	AbstractLocalization	*currentLocalization;
};

#endif
