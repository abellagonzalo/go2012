#include "GeneticLocalization.h"

GeneticLocalization::GeneticLocalization()
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
	this->isInitGenetic=0;
	this->numraces=0;
	this->minimal_race_prob=0.6;
	this->lastWinner=-1;
	this->reliability=0.0;
	this->auxtime=0;

	this->maxmov_pos = 100;
	this->maxmov_angle = 15*M_PI/180;

	this->create_races = 0;
	this->races_id = 0;
}

GeneticLocalization::~GeneticLocalization()
{
}

void
GeneticLocalization::setInfo(MotionInfo &motion, PerceptionInfo &perception)
{
}

void
GeneticLocalization::UpdateInfo()
{
}

void
GeneticLocalization::run()
{
}

void
GeneticLocalization::init(const string newName, AL::ALPtr<AL::ALBroker> parentBroker)
{
	Component::init(newName, parentBroker);
}

/*#############################################################################################################################
															Movement Model
##############################################################################################################################*/

void
GeneticLocalization::checkValidIndividual(TIndividual * individual)
{
	_MovementModel->checkValidPosition(individual->x, individual->y, individual->theta);
}

void
GeneticLocalization::resetToPosition(int state)
{

	TIndividual ind;


	switch(state){

		case (AbstractLocalization::reset2Initial):
		case (AbstractLocalization::reset2Ready):
					initGenetic();
					switch(GameController::getInstance()->getPlayerNumber()){
						case 1:
							ind.x=-2000.0; ind.y=2000.0; ind.theta=toRadians(-90.0);
							ind.prob = 1.0;
							saveNewRace(ind);
							//ind.x=-2000.0; ind.y=-2000.0; ind.theta=toRadians(90.0);
							//ind.prob = 1.0;
							//saveNewRace(ind);
						break;
						case 2:
							ind.x=-1500.0; ind.y=2000.0; ind.theta=toRadians(-90.0);
							ind.prob = 1.0;
							saveNewRace(ind);
							//ind.x=-1500.0; ind.y=-2000.0; ind.theta=toRadians(90.0);
							//ind.prob = 1.0;
							//saveNewRace(ind);
						break;
						case 3:
							//ind.x=-1000.0; ind.y=2000.0; ind.theta=toRadians(-90.0);
							//ind.prob = 1.0;
							//saveNewRace(ind);
							ind.x=-1000.0; ind.y=-2000.0; ind.theta=toRadians(90.0);
							ind.prob = 1.0;
							saveNewRace(ind);
						break;
						case 4:
							//ind.x=-500.0; ind.y=2000.0; ind.theta=toRadians(-90.0);
							//ind.prob = 1.0;
							//saveNewRace(ind);
							ind.x=-500.0; ind.y=-2000.0; ind.theta=toRadians(90.0);
							ind.prob = 1.0;
							saveNewRace(ind);
						break;
					};

					break;


		case (AbstractLocalization::reset2Set):
		case (AbstractLocalization::reset2Playing):

			initGenetic();
			switch(GameController::getInstance()->getPlayerNumber()){
				case 1:
					ind.x=-2500.0; ind.y=0.0; ind.theta=0.0;
					ind.prob = 1.0;
					saveNewRace(ind);
				break;
				case 2:
					if(GameController::getInstance()->isMyKickOff())
					{
						ind.x=-1500.0; ind.y=1500.0; ind.theta=0.0;
					}else
					{
						ind.x=-2000.0; ind.y=1500.0; ind.theta=0.0;
					}
					ind.prob = 1.0;
					saveNewRace(ind);
				break;
				case 3:
					if(GameController::getInstance()->isMyKickOff())
					{
						ind.x=-1000.0; ind.y=0.0; ind.theta=0.0;
					}else
					{
						ind.x=-2000.0; ind.y=250.0; ind.theta=0.0;
					}
					ind.prob = 1.0;
					saveNewRace(ind);
				break;
				case 4:
					if(GameController::getInstance()->isMyKickOff())
					{
						ind.x=-1500.0; ind.y=-1500.0; ind.theta=0.0;
					}else
					{
						ind.x=-2000.0; ind.y=-1500.0; ind.theta=0.0;
					}
					ind.prob = 1.0;
					saveNewRace(ind);
				break;
			};

			break;
		case (AbstractLocalization::resetFromPenalised):

				initGenetic();
				ind.x=-2000.0; ind.y=2000.0; ind.theta=toRadians(-90.0);
				ind.prob = 1.0;
				saveNewRace(ind);
				ind.x=-2000.0; ind.y=-2000.0; ind.theta=toRadians(90.0);
				ind.prob = 1.0;
				saveNewRace(ind);
			break;
		default:
			cerr<<"NEKFLocalization::resetToPosition\tBad GameState"<<endl;
			break;

	};
}

void GeneticLocalization::addPosition(float x, float y, float theta)
{

}

void
GeneticLocalization::moveRaces()
{
	int i, j;
	float movx, movy, movt;

	/*Get movement, if false, the robot has not moved*/
	if(!_MovementModel->getMovement(movx, movy, movt))
		return;

	for(i=0;i<NUMRACES;i++) {
		if(races[i].valid) {
			for(j=0;j<NUMEXPLOITERS;j++)
				_MovementModel->getNewPosition(movx, movy, movt, this->exploiters[i][j].x, this->exploiters[i][j].y, this->exploiters[i][j].theta);

			_MovementModel->getNewPosition(movx, movy, movt, this->races[i].x, this->races[i].y, this->races[i].theta);
		}
	}
}

/*#############################################################################################################################
															Genetic Algorithm
##############################################################################################################################*/

void
GeneticLocalization::updateReliability(int index)
{

	float maxprob = 0.8;
	float minprob = 0.7;
	float maxrel = 0.6;
	float minrel = 0.3;
	float maxchange = 0.15;
	float minchange = 0.03;
	float current_reliability = _ObservationModel->getImageReliability();

	/*Big prob and image very good*/
	if(races[index].prob > maxprob && current_reliability > maxrel) {
		this->reliability += maxchange * (((1 - maxprob) - (1.0-races[index].prob))/(1 - maxprob));

	/*Big prob and image fair*/
	} else if(races[index].prob > maxprob && current_reliability > minrel) {
		this->reliability += minchange * (((1 - maxprob) - (1.0-races[index].prob))/(1 - maxprob));
	}

	/*Bad prob and image is very good or fair*/
	if(races[index].prob < minprob && current_reliability > minrel) {
		this->reliability -= maxchange * ((minprob-races[index].prob)/minprob);

	/*Bad prob and image is bad*/
	} else if(races[index].prob < minprob) {
		this->reliability -= minchange * ((minprob-races[index].prob)/minprob);
	}

	if(this->reliability > 1.0)
		this->reliability = 1.0;
	if(this->reliability < 0.0)
		this->reliability = 0.0;	
}

float
GeneticLocalization::distancePoints(float xini, float yini, float xfin, float yfin)
{
	return sqrt((xini-xfin)*(xini-xfin) + (yini-yfin)*(yini-yfin));
}

void 
GeneticLocalization::calcRaceWinner()
{

	int i;
	int bestpos;
	int winner;
	int numwins;
	float bestprob;
	float diffprob;
	int searchwinner=1;
	float dist;
	
	/*Get the best race in this iteration*/
	bestpos = -1;
	bestprob = 0;
	for(i=0;i<NUMRACES;i++) {
		if(races[i].valid) {
			if(races[i].prob > bestprob) {
				bestprob = races[i].prob;
				bestpos = i;
			}

			/*Decrease wins if the prob is not good*/
			if(races[i].prob < minimal_race_prob-0.2 && races[i].wins>0)
				races[i].wins--;
		}
	}

	/*Search winner if the prob is good enough*/
	if(bestprob >= minimal_race_prob && bestpos >= 0) {

		/*Increase life of the current winner*/
		races[bestpos].life = RACELIFE*3;

		/*Check reliability to change to another race just when we are not well located*/
		if(lastWinner!=bestpos && lastWinner >= 0) {
			if(this->reliability > 0.8) {
				/*If current localization has a great reliability*/
				diffprob = 0.1;
			} else if(_ObservationModel->getImageReliability() < 0.5) {
				/*Don't see the goal*/
				diffprob = 0.1;
			} else {
				/*The reliability is not perfect and we see some parts of the goal*/
				diffprob = 0.01 + (0.09 * this->reliability);
			}

			if(fabs(races[lastWinner].prob - races[bestpos].prob) < diffprob) {
				searchwinner=0;
			}
		}

		if(searchwinner) {	
			winner=0;
			numwins=-1;
			/*Update winners*/
			for(i=0;i<NUMRACES;i++) {
				if(races[i].valid) {
					if(i==bestpos) {	
						/*Add wins*/
						races[i].wins++;
						if(races[i].wins > MAXWINS)
							races[i].wins = MAXWINS;
					} else {
						/*Subtract wins*/
						races[i].wins--;
						if(races[i].wins < 0)
							races[i].wins = 0;				
					}
					if(races[i].wins > numwins) {
						numwins = races[i].wins;
						winner = i;
					}
				}
			}
			/*Update reliability when the winner changes*/
			if(winner != lastWinner && lastWinner >= 0) {
				dist = this->distancePoints(races[lastWinner].x, races[lastWinner].y, races[winner].x, races[winner].y);
				if(dist > MAX_DISTANCE_WINNER)
					this->reliability = 0;
				else
					this->reliability = this->reliability * (1.0 - dist/(float)MAX_DISTANCE_WINNER);

				/*Increase races id*/
				this->races_id += 2;
			}
			lastWinner = winner;	
		}
	}

	/*Update reliability*/
	if(lastWinner>=0)
		this->updateReliability(lastWinner);

	/*Save the position*/
	if(lastWinner>=0) {
		float myx, myy, mytheta;
		myx = races[lastWinner].x;
		myy = races[lastWinner].y;
		mytheta = races[lastWinner].theta;
		/*Blue player, attack in yellow net*/
		if(this->getPlayerColor() == TEAM_BLUE) {
			myx = -1.0*myx;
			myy = -1.0*myy;
			mytheta = mytheta+M_PI;
			if(mytheta>M_PI)
				mytheta = mytheta - 2*M_PI;
		}
	}
}

void
GeneticLocalization::saveElitist(TIndividual * elitist, TIndividual candidate, int size)
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
GeneticLocalization::calcExploitersHealth()
{

	int i,j;
	int numelit = NUMEXPLOITERS/5;
	TIndividual elitist[numelit];
	int tmpelit;

	if(numraces<=0)
		return;

	for(i=0;i<NUMRACES;i++) {
		if(races[i].valid) {
			/*Reset elitists and race*/
			memset(elitist, 0, sizeof(TIndividual)*numelit);
			races[i].x = 0.0;
			races[i].y = 0.0;
			races[i].theta = 0.0;
			races[i].prob = 0.0;

			/*Calc health of the exploiters*/
			for(j=0;j<NUMEXPLOITERS;j++) {

				exploiters[i][j].prob = _LocalizationHealth->getHealth(exploiters[i][j].x, exploiters[i][j].y, exploiters[i][j].theta, this->robotConfig, this->objects);

				/*Save elitist*/
				if(exploiters[i][j].prob > elitist[numelit-1].prob) {
					saveElitist(elitist, exploiters[i][j], numelit);
				}
			}
			/*Update race main member*/
			for(j=0;j<numelit;j++) {
				races[i].x += elitist[j].x;
				races[i].y += elitist[j].y;
				races[i].theta += elitist[j].theta;
				races[i].prob += elitist[j].prob;
			}
			races[i].x = races[i].x/(float)numelit;
			races[i].y = races[i].y/(float)numelit;
			races[i].theta = elitist[0].theta;//races[i].theta/(float)numelit;				/*##########Hacer media con los vectores#########*/
			races[i].prob = races[i].prob/(float)numelit;	

			/*Update elitist*/
			tmpelit=0;	
			if(elitist[numelit-1].prob > 0.4) { /*Set a minimal value*/
				for(j=0;j<NUMEXPLOITERS;j++) {
					if(exploiters[i][j].prob >= elitist[numelit-1].prob && tmpelit<numelit) {
						exploiters[i][j].elitist = 1;
						tmpelit++;
					} else
						exploiters[i][j].elitist = 0;				
				}
			}
		}
	}	
}

void
GeneticLocalization::calcRacesHealth()
{

	int i;

	for(i=0;i<NUMRACES;i++) {
		if(races[i].valid) {
			races[i].prob = _LocalizationHealth->getHealth(	races[i].x, races[i].y, races[i].theta, this->robotConfig, this->objects);
		}
	}

}

void
GeneticLocalization::deleteRaces()
{
	int i;

	if(numraces==0)
		return;	

	for(i=0;i<NUMRACES;i++) {
		if(races[i].valid) {
			if(races[i].prob<minimal_race_prob && races[i].wins==0 && races[i].life==0) {
				races[i].valid = 0;
				numraces--;
			}
		}
	}

}

int
GeneticLocalization::similarRaces(TIndividual race1, TIndividual race2)
{
	float maxpos = 2*maxmov_pos;
	float maxangle = 2*maxmov_angle;
	float ang_aux;

	/*Case angles near +-M_PI*/
	if(race1.theta < race2.theta)
		ang_aux = race2.theta - (race1.theta + 2+M_PI);
	else
		ang_aux = race1.theta - (race2.theta + 2*M_PI);
		

	return 	fabs(race1.x - race2.x) < maxpos && 
			fabs(race1.y - race2.y) < maxpos && 
			(fabs(race1.theta - race2.theta) < maxangle || fabs(ang_aux) < maxangle);
}

void
GeneticLocalization::fusionRaces()
{

	int i,j;
	if(numraces==0)
		return;

	for(i=0;i<NUMRACES-1;i++) {
		if(races[i].valid) {
			/*Check races[i].valid inside "for" because it can change*/
			for(j=i+1;j<NUMRACES && races[i].valid;j++) {
				if(races[j].valid) {
					/*If both races are similar*/
					if(this->similarRaces(races[i], races[j])) {
						//Ponemos valid 0 a la que no sea ganadora
						if(races[i].wins > races[j].wins)
							races[j].valid = 0;
						else if(races[i].wins < races[j].wins)
							races[i].valid = 0;
						else {
							/*Tenemos en cuenta la probability*/
							if(races[i].prob > races[j].prob)
								races[j].valid = 0;
							else
								races[i].valid = 0;
						}
						numraces--;
					}
				}
			}
		}
	}
}

void
GeneticLocalization::saveNewRace(TIndividual mainindividual)
{

	int i;
	int pos=-1;
	int found=0;
	float minimal=1.01;

	/*If there are free positions*/
	if(numraces<NUMRACES) {
		/*Find a free position in races*/
		for(i=0; i<NUMRACES && !found;i++) {
			if(!races[i].valid) {
				pos=i;
				found=1;
			}
		}
		numraces++;
	} else {
		/*Find the worst and replace it*/
		for(i=0; i<NUMRACES;i++) {
			if(races[i].valid && races[i].prob < minimal && races[i].wins==0 && races[i].life==0) {
				minimal = races[i].prob;
				pos=i;
				found=1;
			}
		}		
	}

	if(!found) {
		/*All the individuals have life or the winner is the worst*/
		return;
	}

	races[pos].x = mainindividual.x;
	races[pos].y = mainindividual.y;
	races[pos].theta = mainindividual.theta;
	races[pos].prob = mainindividual.prob;
	races[pos].elitist = 1;
	races[pos].valid = 1;
	races[pos].isnew = 1;
	races[pos].wins = 0;
	races[pos].life = 0;
}

float
GeneticLocalization::getMinProbRace()
{
	int i;
	float minimal=1.01;

	if(numraces<NUMRACES)	
		return 0;

	for(i=0;i<NUMRACES;i++) {
		if(races[i].valid && races[i].prob < minimal)
			minimal = races[i].prob;
	}

	return minimal;
}

void
GeneticLocalization::createNewRaces()
{

	int i,j;
	float minval;
	int foundsimilar;

	minval = getMinProbRace();

	/*Create explorers*/
	for(i=0;i<NUMEXPLORERS;i++) {
		if(explorers[i].valid) {
			/*Invalid similar explorers with worse prob (list is in order)*/
			for(j=i+1;j<NUMEXPLORERS;j++) {
				if(explorers[j].valid) {
					if(this->similarRaces(explorers[i], explorers[j])) {
						explorers[j].valid = 0;
					}
				}
			}

			/*Compare with current races*/
			foundsimilar = 0;
			for(j=0;j<NUMRACES;j++) {
				if(races[j].valid) {
					if(this->similarRaces(explorers[i], races[j])) {
						/*Increase life of the race*/
						if(races[j].life < RACELIFE)
							races[j].life = RACELIFE;
						foundsimilar = 1;
					}
				}
			}
			if(foundsimilar)
				continue;

			/*Save explorer if improves races*/
			if(explorers[i].prob>minimal_race_prob)	{
				/*If the explorer is better than some race, replace it*/
				if(explorers[i].prob>minval) {
					saveNewRace(explorers[i]);
					minval = getMinProbRace();
				}
			}
		}
	}	
}

void
GeneticLocalization::createExploiters(int index, TIndividual mainindividual)
{

	int i;
	int signx, signy, signang;
	float noisex, noisey, noiseangle;
	float maxpos = maxmov_pos;
	float maxangle = maxmov_angle;

	/*If it's in in the central circle and can't see the goal, it loses easy*/
	int angok = fabs(mainindividual.theta) < 55.0*M_PI/180.0 || fabs(mainindividual.theta) > 125.0*M_PI/180.0;
	if(!this->hasGoals && angok && fabs(mainindividual.x) < 500.0 && fabs(mainindividual.y) < 500.0)
		maxangle = maxmov_angle/10.0;

	/*Create exploiters*/
	for (i=0; i < NUMEXPLOITERS; i++) {
		/*Random values*/
		signx = rand()%2==0?1:-1;
		signy = rand()%2==0?1:-1;
		signang = rand()%2==0?1:-1;
		noisex = maxpos * ((float)rand()/(float)RAND_MAX);
		noisey = maxpos * ((float)rand()/(float)RAND_MAX);
		noiseangle = maxangle * ((float)rand()/(float)RAND_MAX);

		this->exploiters[index][i].x = mainindividual.x + noisex*signx;
		this->exploiters[index][i].y = mainindividual.y + noisey*signy;
		this->exploiters[index][i].theta = mainindividual.theta + noiseangle*signang;
		this->exploiters[index][i].prob = 0.5;
		this->exploiters[index][i].elitist = 0;
		this->exploiters[index][i].valid = 1;

		this->checkValidIndividual(&(this->exploiters[index][i]));
	}
}

void
GeneticLocalization::evolutionExploiters(int index, TIndividual mainindividual)
{

	int i;
	int signx, signy, signang;
	float noisex, noisey, noiseangle;
	float maxpos = maxmov_pos;
	float maxangle = maxmov_angle;
	int numcross = 0;
	int expini, expfin;

	/*If it's in in the central circle and can't see the goal, it loses easy*/
	int angok = fabs(mainindividual.theta) < 55.0*M_PI/180.0 || fabs(mainindividual.theta) > 125.0*M_PI/180.0;
	if(!this->hasGoals && angok && fabs(mainindividual.x) < 500.0 && fabs(mainindividual.y) < 500.0)
		maxangle = maxmov_angle/10.0;

	/*Create exploiters*/
	for (i=0; i < NUMEXPLOITERS; i++) {
		if(!exploiters[index][i].elitist) {		/*20% elitists*/
			if(numcross < NUMEXPLOITERS/5) {	/*20% cross*/
				/*Cross exploiters*/
				expini = rand()%NUMEXPLOITERS;
				expfin = rand()%NUMEXPLOITERS;	
				this->exploiters[index][i].x = (exploiters[index][expini].x + exploiters[index][expfin].x)/2;
				this->exploiters[index][i].y = (exploiters[index][expini].y + exploiters[index][expfin].y)/2;
				/*Select the first angle (is random)*/
				this->exploiters[index][i].theta = exploiters[index][expini].theta;								/*####Hacer la media con los vectores####*/
				this->exploiters[index][i].prob = 0.5;
				this->exploiters[index][i].elitist = 0;
				this->exploiters[index][i].valid = 1;
				numcross++;
			} else {
				/*Random values*/
				signx = rand()%2==0?1:-1;
				signy = rand()%2==0?1:-1;
				signang = rand()%2==0?1:-1;
				noisex = maxpos * ((float)rand()/(float)RAND_MAX);
				noisey = maxpos * ((float)rand()/(float)RAND_MAX);
				noiseangle = maxangle * ((float)rand()/(float)RAND_MAX);

				this->exploiters[index][i].x = mainindividual.x + noisex*signx;
				this->exploiters[index][i].y = mainindividual.y + noisey*signy;
				this->exploiters[index][i].theta = mainindividual.theta + noiseangle*signang;
				this->exploiters[index][i].prob = 0.5;
				this->exploiters[index][i].elitist = 0;
				this->exploiters[index][i].valid = 1;

				this->checkValidIndividual(&(exploiters[index][i]));
			}
		}
	}
}

void
GeneticLocalization::evolutionRace()
{

	int i;

	for(i=0;i<NUMRACES;i++) {
		if(races[i].valid) {
			/*If the race is new, create random exploiters*/
			if(races[i].isnew) {
				createExploiters(i, races[i]);
				races[i].life = RACELIFE;
			} else {
				evolutionExploiters(i, races[i]);
				races[i].life--;
				if(races[i].life < 0)
					races[i].life = 0;
			}		
			races[i].isnew = 0;
		}
	}
}

/*#############################################################################################################################
															Explorers list
##############################################################################################################################*/

void
GeneticLocalization::evolutionExplorers()
{
	vector<HPoint2D> positions;
	int numset;
	static int num_it = 0;

	/*Only when we can see the goal*/
	if(!this->hasGoals) {
		num_it = 0;
		return;
	}

	/*Don't create explorers every iteration*/
	if(num_it%EXPLORERS_ITS != 0) {
		num_it++;
		return;
	}
	num_it++;

	/*Check how many races can be replaced*/
	numset = 0;
	for(int i=0;i<NUMRACES;i++) {
		if(races[i].valid) {
			if(races[i].wins > 0 || races[i].life > 0)
				numset++;
		}
	}
	/*If all races are set, return*/
	if(numset==NUMRACES)
		return;

	/*Get explorers positions*/
	_LocalizationHealth->getBestPositions(this->objects, positions);

	/*Check if there are valid positions*/
	if(positions.size() == 0)
		return;

	for(vector<HPoint2D>::iterator it=positions.begin(); it!=positions.end(); it++) {
		this->insertPart((*it).x, (*it).y, (*it).h);
	}

	this->saveListExplorers();
	this->list_explorers.clear();
	this->create_races = 1;	
}

void
GeneticLocalization::insertPart(float x, float y, float theta)
{
	TIndividual individual;
	float maxnoisepos = 500;
	float maxnoiseangle = 20*M_PI/180.0;
	float noisex, noisey, noiset;
	int signx, signy, signt;

	static int auxtime=0;
	srand(time(0)+auxtime);
	auxtime++;

	signx = rand()%2==0?1:-1;
	signy = rand()%2==0?1:-1;
	signt = rand()%2==0?1:-1;
	noisex = maxnoisepos * ((float)rand()/(float)RAND_MAX);
	noisey = maxnoisepos * ((float)rand()/(float)RAND_MAX);
	noiset = maxnoiseangle * ((float)rand()/(float)RAND_MAX);

	/*Create new position*/
	individual.x = x + noisex*signx;
	individual.y = y + noisey*signy;
	individual.theta = theta + noiset*signt;
	this->checkValidIndividual(&individual);
	individual.prob = _LocalizationHealth->getHealth(individual.x, individual.y, individual.theta, this->robotConfig, this->objects);	

	/*If prob is very low, not insert*/
	if(individual.prob <= 0.3)
		return;

	/*Insert*/
	/*List is empty*/
	if(this->list_explorers.empty()) {
		this->list_explorers.push_back(individual);
		return;
	}

	/*Not empty*/
	for(list<TIndividual>::iterator it=this->list_explorers.begin(); it!=this->list_explorers.end(); it++) {
		if(individual.prob < (*it).prob)
			continue;

		/*Insert element*/
		this->list_explorers.insert(it, individual);
		return;		
	}

	/*Last element*/
	this->list_explorers.push_back(individual);
}

void
GeneticLocalization::saveListExplorers()
{
	int count;

	memset(this->explorers, 0, sizeof(TIndividual)*NUMEXPLORERS);

	count = 0;
	list<TIndividual>::iterator it = this->list_explorers.begin();
	while(it!=this->list_explorers.end() && count<NUMEXPLORERS) {
		this->explorers[count].x = (*it).x;
		this->explorers[count].y = (*it).y;
		this->explorers[count].theta = (*it).theta;
		this->explorers[count].prob = (*it).prob;
		this->explorers[count].valid = 1;

		it++;
		count++;
	}	
}

/*#############################################################################################################################
															Main algorithm
##############################################################################################################################*/

void
GeneticLocalization::initGenetic() 
{
	int i;

	srand(time(0)+this->auxtime);
	this->auxtime++;
	
	/*Restart races*/
	for(i=0;i<NUMRACES;i++) {
		this->races[i].valid = 0;
	}
	this->numraces = 0;
	this->reliability = 0.0;

	/*Update movement values*/
	_MovementModel->initMovement();

	this->resetLocalization = 0;
}

void
GeneticLocalization::saveState()
{
	/*Movement model*/
	this->moveRaces();

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
GeneticLocalization::iterateGenetic()
{
	/*Create and evolution explorers*/
	evolutionExplorers();

	/*Calc current races health*/
	calcRacesHealth();

	/*Create new races*/
	if(this->create_races) {
		this->create_races = 0;
		createNewRaces();
	}

	/*Delete races*/
	deleteRaces();

	/*Fusion similar races*/
	fusionRaces();

	/*Evolution the race, create or generate new exploiters*/
	evolutionRace();

	/*Calc exploiters health and improve the races main member*/
	calcExploitersHealth();	

	/*Calc the best race*/
	calcRaceWinner();
}

void
GeneticLocalization::step(void)
{
	struct timeval a,b;
	long totalb,totala;
	int isTouching;

	// Steps of its lower components
	_Perception->step();
	_Kinematics->step();
	_LineDetector->step();
	_GoalDetector->step();
	_Body->step();

	if (!isTime2Run())
		return;

	/*Total time*/
	gettimeofday(&a,NULL);
	totala=a.tv_sec*1000000+a.tv_usec;

	/*If the robot is not touching the ground, it's a kidnapping*/
	isTouching = _FSR->isTouchingGround();
	this->resetLocalization = this->resetLocalization || !isTouching;

	/*Init or restart the algorithm*/
	if(!isInitGenetic || this->resetLocalization) {
		initGenetic();
		isInitGenetic = 1;
	}

	/*Save the current state of the robot*/
	this->saveState();

	if(this->validState)
		this->iterateGenetic();
		
	/*Total time*/
	gettimeofday(&b,NULL);
	totalb=b.tv_sec*1000000+b.tv_usec;
	/*ostringstream sout;
	cout << "Genetic localization takes " << totalb-totala << " microseconds" << endl;
	sout << "Genetic localization takes " << totalb-totala << " microseconds" << endl;
	this->writeLog(sout.str());*/
}

void
GeneticLocalization::getPosition(vector<tRobotHipothesis> &hipotheses)
{
	tRobotHipothesis aux, aux2;
	getPosition(aux.x, aux.y, aux.t);
	aux.prob = getReliability();
	aux.id = this->races_id;

	/*TODO: Test!*/
	aux2.x = -1.0*aux.x;
	aux2.y = -1.0*aux.y;
	aux2.t = aux.t+M_PI;
	if(aux2.t>M_PI)
		aux2.t = aux2.t - 2*M_PI;	
	aux2.prob = aux.prob;
	aux2.id = this->races_id+1;

	hipotheses.clear();
	hipotheses.push_back(aux);
	hipotheses.push_back(aux2);
}

void
GeneticLocalization::getPosition(double &xpos, double &ypos, double &thetapos)
{
	if(lastWinner>=0)
	{
		xpos = (double)this->races[lastWinner].x;
		ypos = (double)this->races[lastWinner].y;
		thetapos = (double)this->races[lastWinner].theta;

		/*Blue player, attack in yellow net*/
		if(this->getPlayerColor() == TEAM_BLUE)
		{
			xpos = -1.0*xpos;
			ypos = -1.0*ypos;
			thetapos = thetapos+M_PI;
			if(thetapos>M_PI)
				thetapos = thetapos - 2*M_PI;
		}
	}else
	{
		xpos = 0.0;
		ypos = 0.0;
		thetapos = 0.0;
	}
}

float
GeneticLocalization::getReliability()
{
	return this->reliability;
}

/*#############################################################################################################################
															Debug
##############################################################################################################################*/

ImageDataPtr
GeneticLocalization::getImgDebug()
{
	return _ObservationModel->getImgPoints();
}

string 
GeneticLocalization::getInfo()
{

	int i;
	float x,y,theta;

	std::stringstream out;

	out << this->numraces;
	for(i=0;i<NUMRACES;i++) {
		if(races[i].valid) {
			x = races[i].x;
			y = races[i].y;
			theta = races[i].theta;
			/*Blue player, attack in yellow net*/
			if(this->getPlayerColor() == TEAM_BLUE) {
				x = -1.0*x;
				y = -1.0*y;
				theta = theta+M_PI;
				if(theta>M_PI)
					theta = theta - 2*M_PI;
			}

			out << "R" << races[i].wins << ":" << x << ":" << y << ":" << theta << ":" << races[i].prob << ":" << races[i].life;
		}
	}

	//Show explorers	
	for(i=0;i<NUMEXPLORERS;i++) {
		if(explorers[i].valid) {
			x = explorers[i].x;
			y = explorers[i].y;
			theta = explorers[i].theta;
			/*Blue player, attack in yellow net*/
			if(this->getPlayerColor() == TEAM_BLUE) {
				x = -1.0*x;
				y = -1.0*y;
				theta = theta+M_PI;
				if(theta>M_PI)
					theta = theta - 2*M_PI;
			}
			out << "R" << explorers[i].wins << ":" << x << ":" << y << ":" << theta << ":" << 0.0 << ":" << explorers[i].life;
		}
	}	

	//Winner
	if(lastWinner>=0) {
		x = races[lastWinner].x;
		y = races[lastWinner].y;
		theta = races[lastWinner].theta;
		/*Blue player, attack in yellow net*/
		if(this->getPlayerColor() == TEAM_BLUE) {
			x = -1.0*x;
			y = -1.0*y;
			theta = theta+M_PI;
			if(theta>M_PI)
				theta = theta - 2*M_PI;
		}
		out << "R" << x << ":" << y << ":" << theta;
	} else
		out << "R" << "0.0" << ":" << "0.0" << ":" << "0.0";
	
	out << endl;
	string s = out.str();

	return s;
}
