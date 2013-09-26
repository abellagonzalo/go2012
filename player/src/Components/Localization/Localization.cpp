#include "Localization.h"

Localization::Localization()
{
	_GeneticLocalization = GeneticLocalization::getInstance();
	_MontecarloLocalization = MontecarloLocalization::getInstance();
	_GTLocalization = GTLocalization::getInstance();
	_NEKFLocalization = NEKFLocalization::getInstance();
	_EKFLocalization = EKFLocalization::getInstance();
	_SRL = SRL::getInstance();
	_MovementModel = MovementModel::getInstance();
	_ObservationModel = ObservationModel::getInstance();
	_LocalizationHealth = LocalizationHealth::getInstance();

	this->currentLocalization = _GeneticLocalization;
}

Localization::~Localization()
{
}

void
Localization::init(const string newName, AL::ALPtr<AL::ALBroker> parentBroker)
{
	Component::init(newName, parentBroker);

	/*Init distances*/
	_LocalizationHealth->init();

	/*Init the algorithms*/
	_GeneticLocalization->init("GeneticLocalization", parentBroker);
	_MontecarloLocalization->init("MontecarloLocalization", parentBroker);
	_GTLocalization->init("GTLocalization", parentBroker);
	_NEKFLocalization->init("NEKFLocalization", parentBroker);
	_EKFLocalization->init("EKFLocalization", parentBroker);
	_SRL->init("SRL", parentBroker);
}

void
Localization::setLocalization(string type)
{
	if(type.compare("GeneticLocalization") == 0)
		this->currentLocalization = _GeneticLocalization;
	else if(type.compare("MontecarloLocalization") == 0)
		this->currentLocalization = _MontecarloLocalization;
	else if(type.compare("GTLocalization") == 0)
		this->currentLocalization = _GTLocalization;
	else if(type.compare("NEKFLocalization") == 0)
		this->currentLocalization = _NEKFLocalization;
	else if(type.compare("EKFLocalization") == 0)
		this->currentLocalization = _EKFLocalization;
	else if(type.compare("SRL") == 0)
		this->currentLocalization = _SRL;
}

void
Localization::step()
{
	this->currentLocalization->step();
}

void
Localization::getRealPosition(double &xpos, double &ypos, double &thetapos)
{
	_GTLocalization->getPosition(xpos, ypos, thetapos);
}

string
Localization::getRealPosition(const Ice::Current& c)
{
	std::stringstream out;
	double xpos, ypos, thetapos;

	_GTLocalization->getPosition(xpos, ypos, thetapos);

	out << xpos << ":" << ypos << ":" << thetapos;

	return out.str();
}

void
Localization::setRealPosition(double xpos, double ypos, double thetapos)
{
	_GTLocalization->setPosition(xpos, ypos, thetapos);
}

void 
Localization::setRealPosition(double xpos, double ypos, double thetapos, const Ice::Current& c)
{
	_GTLocalization->setPosition(xpos, ypos, thetapos);
}

void
Localization::getPosition(double &xpos, double &ypos, double &thetapos)
{
	this->currentLocalization->getPosition(xpos, ypos, thetapos);
}

void
Localization::getPosition(vector<tRobotHipothesis> &hipotheses)
{
	this->currentLocalization->getPosition(hipotheses);
}

float
Localization::getReliability(const Ice::Current& c)
{
	return this->currentLocalization->getReliability();
}

float
Localization::getReliability()
{
	return this->currentLocalization->getReliability();
}

void
Localization::resetLocalization()
{
	this->currentLocalization->reset();
}


void
Localization::resetToPosition(int state)
{
	this->currentLocalization->resetToPosition(state);
}


void
Localization::resetLocalization(const Ice::Current& c)
{
	this->currentLocalization->reset();
}

string
Localization::getLocalizationInfo(const Ice::Current& c)
{
	return this->currentLocalization->getInfo();
}

bica::ImageDataPtr
Localization::getLocalizationImg(const Ice::Current& c)
{
	return this->currentLocalization->getImgDebug();
}

void
Localization::setInfo(MotionInfo &motion, PerceptionInfo &perception)
{
	this->currentLocalization->setInfo(motion, perception);
}

void
Localization::run()
{
	this->currentLocalization->run();
}
