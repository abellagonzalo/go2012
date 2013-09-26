#include "MontecarloLocalization.h"

MontecarloLocalization::MontecarloLocalization()
{
	_Perception = Perception::getInstance();
	_Kinematics = Kinematics::getInstance();
	_Body = Body::getInstance();
	_FSR = FSR::getInstance();
	_MovementModel = MovementModel::getInstance();
	_ObservationModel = ObservationModel::getInstance();
	_LocalizationHealth = LocalizationHealth::getInstance();
	_LineDetector = LineDetector::getInstance();
	_GoalDetector = GoalDetector::getInstance();

	// Freqs of each component
	this->setFreqTime(MY_FREQ);

	/*Genetic parameters*/
	this->isInitMontecarlo=0;
	this->reliability=0.0;
	this->auxtime=0;

	this->maxmov_pos = 100;
	this->maxmov_angle = 15*M_PI/180;
}

MontecarloLocalization::~MontecarloLocalization()
{
}

void
MontecarloLocalization::init(const string newName, AL::ALPtr<AL::ALBroker> parentBroker)
{
	Component::init(newName, parentBroker);
}

/*#############################################################################################################################
															Movement Model
##############################################################################################################################*/

void
MontecarloLocalization::checkValidParticle(TParticle * particle)
{
	_MovementModel->checkValidPosition(particle->x, particle->y, particle->theta);
}

void
MontecarloLocalization::moveParticles()
{
	int i;
	float movx, movy, movt;

	/*Get movement, if false, the robot has not moved*/
	if(!_MovementModel->getMovement(movx, movy, movt))
		return;

	for(i=0;i<NUMPART;i++)
		_MovementModel->getNewPosition(movx, movy, movt, this->particles[i].x, this->particles[i].y, this->particles[i].theta);

	_MovementModel->getNewPosition(movx, movy, movt, this->mainParticle.x, this->mainParticle.y, this->mainParticle.theta);
}

/*#############################################################################################################################
															Montecarlo Algorithm
##############################################################################################################################*/

void
MontecarloLocalization::updateReliability()
{

	float maxprob = 0.8;
	float minprob = 0.7;
	float maxrel = 0.6;
	float minrel = 0.3;
	float maxchange = 0.15;
	float minchange = 0.03;
	float current_reliability = _ObservationModel->getImageReliability();

	/*Big prob and image very good*/
	if(mainParticle.prob > maxprob && current_reliability > maxrel) {
		this->reliability += maxchange * (((1 - maxprob) - (1.0-mainParticle.prob))/(1 - maxprob));

	/*Big prob and image fair*/
	} else if(mainParticle.prob > maxprob && current_reliability > minrel) {
		this->reliability += minchange * (((1 - maxprob) - (1.0-mainParticle.prob))/(1 - maxprob));
	}

	/*Bad prob and image is very good or fair*/
	if(mainParticle.prob < minprob && current_reliability > minrel) {
		this->reliability -= maxchange * ((minprob-mainParticle.prob)/minprob);

	/*Bad prob and image is bad*/
	} else if(mainParticle.prob < minprob) {
		this->reliability -= minchange * ((minprob-mainParticle.prob)/minprob);
	}

	if(this->reliability > 1.0)
		this->reliability = 1.0;
	if(this->reliability < 0.0)
		this->reliability = 0.0;	
}

void
MontecarloLocalization::saveElitist(TParticle * elitist, TParticle candidate, int size)
{
	int i, j;

	for(i=0;i<size;i++) {
		/*Ordenado de mayor a menor*/
		if(candidate.prob>=elitist[i].prob) {
			/*Move values*/
			for(j=size-1;j>i;j--) {							/*TODO: ¿OPTIMIZAR con lista enlazada?*/
				elitist[j].x = elitist[j-1].x;
				elitist[j].y = elitist[j-1].y;
				elitist[j].theta = elitist[j-1].theta;
				elitist[j].prob = elitist[j-1].prob;			
			}
			elitist[i].x = candidate.x;
			elitist[i].y = candidate.y;
			elitist[i].theta = candidate.theta;
			elitist[i].prob = candidate.prob;
			return;
		}
	}
}

void
MontecarloLocalization::calcParticlesHealth()
{

	int i;
	int numelit = NUMELIT;
	TParticle elitist[numelit];
	int tmpelit;

	/*Reset elitists and main particle*/
	memset(elitist, 0, sizeof(TParticle)*numelit);

	for(i=0;i<NUMPART;i++) {
		particles[i].prob = _LocalizationHealth->getHealth(	particles[i].x, particles[i].y, particles[i].theta, this->robotConfig, this->objects);
		/*Save elitist*/
		if(particles[i].prob > elitist[numelit-1].prob) {
			saveElitist(elitist, particles[i], numelit);
		}
	}

	/*Save the position*/
	for(i=0;i<numelit;i++) {
		mainParticle.x += elitist[i].x;
		mainParticle.y += elitist[i].y;
		mainParticle.theta += elitist[i].theta;
		mainParticle.prob += elitist[i].prob;
	}
	mainParticle.x = mainParticle.x/(float)numelit;
	mainParticle.y = mainParticle.y/(float)numelit;
	mainParticle.theta = elitist[0].theta;//mainParticle.theta/(float)numelit;				/*##########Hacer media con los vectores#########*/
	mainParticle.prob = mainParticle.prob/(float)numelit;		

	/*Update elitist*/
	tmpelit=0;
	if(elitist[numelit-1].prob > 0.4) { /*Set a minimal value*/
		for(i=0;i<NUMPART;i++) {
			if(particles[i].prob >= elitist[numelit-1].prob && tmpelit<numelit) {
				particles[i].elitist = 1;
				tmpelit++;
			} else
				particles[i].elitist = 0;				
		}
	}

	this->updateReliability();
}

/*#############################################################################################################################
															Resampling
##############################################################################################################################*/

int
MontecarloLocalization::binary_search(float number)
{
	int li, ls, middle, found;

	li = 0;
	ls = NUMPART;
	middle = (li + ls) / 2;
	found = 0;
	while (!found) {
		if ((number <= accumulation[middle].accumulation) && (number >= accumulation[middle].low))
			found = 1;
		else if (number > accumulation[middle].accumulation)
			li = middle;
		else
			ls = middle;
		middle = (ls + li) / 2;
	}
	return middle;
}

void
MontecarloLocalization::resampling()
{
	float max_accumulation;
	float random_number;
	int i;
	int id;
	int signx, signy, signang;
	float noisex, noisey, noiseangle;
	float maxpos = maxmov_pos;
	float maxangle = maxmov_angle;
	static int count = 0;

	max_accumulation = 0.0;

	/*Check if the prob of the main particle is low*/
	if (mainParticle.prob <= 0.6) {
		count++;
		if(count>=8) {
			this->resetLocalization = 1;
			count = 0;
		}
		return;
	}

	/*Calculate accumulations*/
	for (i=0;i<NUMPART;i++) {
		accumulation[i].low = max_accumulation;
		accumulation[i].accumulation = particles[i].prob + max_accumulation;
		accumulation[i].x = particles[i].x;
		accumulation[i].y = particles[i].y;
		accumulation[i].theta = particles[i].theta;
		max_accumulation = accumulation[i].accumulation;
	}	
	
	/*Generamos las nuevas partículas*/
	for (i=0;i<NUMPART;i++) {
		
		/*If particle is not elitist resample it*/
		if(!particles[i].elitist ) {

			/*binary search*/
			random_number = max_accumulation * ((float)rand()/(float)RAND_MAX);

			id = this->binary_search(random_number);

			/*Random values*/
			signx = rand()%2==0?1:-1;
			signy = rand()%2==0?1:-1;
			signang = rand()%2==0?1:-1;
			noisex = maxpos * ((float)rand()/(float)RAND_MAX);
			noisey = maxpos * ((float)rand()/(float)RAND_MAX);
			noiseangle = maxangle * ((float)rand()/(float)RAND_MAX);

			this->particles[i].x = accumulation[id].x + noisex*signx;
			this->particles[i].y = accumulation[id].y + noisey*signy;
			this->particles[i].theta = accumulation[id].theta + noiseangle*signy;
			this->particles[i].elitist = 0;
			checkValidParticle(&(this->particles[i]));
		} 
	}
}

/*#############################################################################################################################
															Main algorithm
##############################################################################################################################*/

void
MontecarloLocalization::initMontecarlo() 
{
	int i;

	srand(time(0)+this->auxtime);
	this->auxtime++;
	
	/*Restart particles*/
	for(i=0;i<NUMPART;i++) {
		this->particles[i].x = (rand()%(MAXX*2))-MAXX;
		this->particles[i].y = (rand()%(MAXY*2))-MAXY;
		this->particles[i].theta = (2*M_PI*((float)rand()/(float)RAND_MAX))-M_PI;
		this->particles[i].prob = 0.5;
		this->particles[i].elitist = 0;	
	}
	this->reliability = 0.0;
	this->mainParticle.x = 0.0;
	this->mainParticle.y = 0.0;
	this->mainParticle.theta = 0.0;

	/*Update movement values*/
	_MovementModel->initMovement();

	this->resetLocalization = 0;
}

void
MontecarloLocalization::saveState()
{
	/*Movement model*/
	this->moveParticles();

	/*Configure robot*/
	_Kinematics->cloneKinematics(&(this->robotConfig));

	/*Get parameters from real image*/
	if(!_ObservationModel->getImageParameters(this->objects, this->hasGoals)) {
		this->validState = 0;
		return;
	}

	this->validState = 1;
}


void
MontecarloLocalization::resetToPosition(int state)
{
	//Reset depending on the game situation and the game player
}

void
MontecarloLocalization::iterateMontecarlo()
{
	/*Calc current particles health*/
	this->calcParticlesHealth();

	/*Resample particles*/
	this->resampling();
}

void
MontecarloLocalization::step(void)
{
	struct timeval a,b;
	long totalb,totala;
	int isTouching;

	// Steps of its lower components
	_Perception->step();
	_Kinematics->step();
	_LineDetector->step();
	_GoalDetector->step();


	if (!isTime2Run())
		return;

	/*Total time*/
	gettimeofday(&a,NULL);
	totala=a.tv_sec*1000000+a.tv_usec;

	/*If the robot is not touching the ground, it's a kidnapping*/
	isTouching = _FSR->isTouchingGround();
	this->resetLocalization = this->resetLocalization || !isTouching;

	/*Init or restart the algorithm*/
	if(!isInitMontecarlo || this->resetLocalization) {
		initMontecarlo();
		isInitMontecarlo = 1;
	}

	/*Save the current state of the robot*/
	this->saveState();

	if(this->validState)
		this->iterateMontecarlo();
		
	/*Total time*/
	gettimeofday(&b,NULL);
	totalb=b.tv_sec*1000000+b.tv_usec;
	/*ostringstream sout;
	cout << "Genetic localization takes " << totalb-totala << " microseconds" << endl;
	sout << "Genetic localization takes " << totalb-totala << " microseconds" << endl;
	this->writeLog(sout.str());*/
}

void
MontecarloLocalization::getPosition(vector<tRobotHipothesis> &hipotheses)
{
	tRobotHipothesis aux;
	getPosition(aux.x, aux.y, aux.t);
	aux.prob = getReliability();

	hipotheses.clear();
	hipotheses.push_back(aux);
}

void
MontecarloLocalization::getPosition(double &xpos, double &ypos, double &thetapos)
{
	xpos = (double)this->mainParticle.x;
	ypos = (double)this->mainParticle.y;
	thetapos = (double)this->mainParticle.theta;

	/*Blue player, attack in yellow net*/
	if(this->getPlayerColor() == TEAM_BLUE)
	{
		xpos = -1.0*xpos;
		ypos = -1.0*ypos;
		thetapos = thetapos+M_PI;
		if(thetapos>M_PI)
			thetapos = thetapos - 2*M_PI;
	}
}

float
MontecarloLocalization::getReliability()
{
	return this->reliability;
}

/*#############################################################################################################################
															Debug
##############################################################################################################################*/

ImageDataPtr
MontecarloLocalization::getImgDebug()
{
	return _ObservationModel->getImgPoints();
}

string 
MontecarloLocalization::getInfo()
{
	int i;

	std::stringstream out;
	out << 0;

	for(i=0;i<NUMPART;i=i+1) {
		if(particles[i].elitist)
			out << "R" << 0 << ":" << particles[i].x << ":" << particles[i].y << ":" << particles[i].theta << ":" << particles[i].prob << ":" << 0;
	}

	//Winner
	out << "R" << mainParticle.x << ":" << mainParticle.y << ":" << mainParticle.theta;
	
	out << endl;
	string s = out.str();

	return s;
}


void
MontecarloLocalization::setInfo(MotionInfo &motion, PerceptionInfo &perception)
{
}

void
MontecarloLocalization::UpdateInfo()
{
}

void
MontecarloLocalization::run()
{
}
