#include "GameController.h"

#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>

using namespace std;

GameController::GameController()
{

	rgc = new RoboCupGameControlData;

	static string CFG_FILE = "/home/nao/bica/conf/team.cfg";
	initPlayerConfig(CFG_FILE.c_str());
	leds = Leds::getInstance();

	initRGC(rgc);

	last_gamestate = -1;
	penalised = false;


	initUDPConn(GAMECONTROLLER_PORT);
}

void GameController::UDPmanageRecv(int size)
{
	char *data;

	data = getUDPBuffer();

	if(validatePacket((RoboCupGameControlData*)data)) {
		processRGC((RoboCupGameControlData*)data);
		//printRGC((RoboCupGameControlData*)data);
	}
}

bool GameController::validatePacket(RoboCupGameControlData *data) {

	/* check the right structure header has come in */
	if (!(checkHeader(data->header))) {
		return false;
	}

	/* check for partial packets */
	if (sizeof(*data) != sizeof(RoboCupGameControlData)) {
		////cerr<<"RECEIVED PARTIAL PACKET"<<endl;
		////cerr << "data: " << sizeof(*data) << " " << "controller: " << sizeof(RoboCupGameControlData) << endl;
		return false;
	}

	/* check the right version of the structure is being used */
	if (data->version != GAMECONTROLLER_STRUCT_VERSION) {
		////cerr<<"DATA VERSION MISMATCH"<<endl;
		////cerr<<"local version is version "<<GAMECONTROLLER_STRUCT_VERSION<<", receiving version "<<data->version;
		return false;
	}

	/* check whether this packet belongs to this game at all */
	if (!isThisGame(data)) return false;

	return true;
}

bool GameController::checkHeader(char* header) {
	for (int i=0; i<4; i++)
		if (header[i] != GAMECONTROLLER_STRUCT_HEADER[i]) return false;
	return true;
}

/* checks whether a packet even belongs in this game */
bool GameController::isThisGame(RoboCupGameControlData* gameData) {

	if (gameData->teams[TEAM_BLUE].teamNumber != teamNumber &&
			gameData->teams[TEAM_RED].teamNumber  != teamNumber) {
		return false;
	}
	return true;
}

void GameController::processRGC(RoboCupGameControlData *rgc_data)
{
	static uint16_t last_penalty = PENALTY_NONE;

	if(rgc_data != NULL) //not from UDP server
		memcpy(rgc, rgc_data, sizeof(RoboCupGameControlData));

	if (rgc->teams[TEAM_BLUE].teamNumber == teamNumber)
		myTeam = &rgc->teams[TEAM_BLUE];
	else if (rgc->teams[TEAM_RED].teamNumber == teamNumber)
		myTeam =  &rgc->teams[TEAM_RED];


	//leds->setLed(Leds::CHEST_LED, Leds::COLOR_RED, true, false);

	updateState(rgc->state);

	//Only change conditions among states and things that happends in each state
	//s<<"State: ";//<<rgc->state<<endl;
	//cout << rgc->state << endl;
	switch((int)rgc->state)
	{
	case STATE_INITIAL:

		leds->setLed(Leds::CHEST_LED, Leds::COLOR_OFF, true);

		//cerr<<"1";
		if (rgc->teams[TEAM_BLUE].teamNumber == teamNumber)
		{
			leds->setLed(Leds::LFOOT_BUTTON, Leds::COLOR_BLUE, true);
			if(rgc->kickOffTeam == TEAM_BLUE)
			{
				leds->setLed(Leds::RFOOT_BUTTON, Leds::COLOR_WHITE, true);
			}
			else
			{
				leds->setLed(Leds::RFOOT_BUTTON, Leds::COLOR_OFF, true);
			}
		}
		else if (rgc->teams[TEAM_RED].teamNumber == teamNumber)
		{
			leds->setLed(Leds::LFOOT_BUTTON, Leds::COLOR_RED, true);
			if(rgc->kickOffTeam == TEAM_RED)
			{
				leds->setLed(Leds::RFOOT_BUTTON, Leds::COLOR_WHITE, true);
			}
			else
			{
				leds->setLed(Leds::RFOOT_BUTTON, Leds::COLOR_OFF, true);
			}
		}

		break;
		//cerr<<"10"<<endl;

	case STATE_READY:

		leds->setLed(Leds::CHEST_LED, Leds::COLOR_BLUE, true);

		if (rgc->teams[TEAM_BLUE].teamNumber == teamNumber)
		{
			leds->setLed(Leds::LFOOT_BUTTON, Leds::COLOR_BLUE, true);
			if(rgc->kickOffTeam == TEAM_BLUE)
			{
				leds->setLed(Leds::RFOOT_BUTTON, Leds::COLOR_WHITE, true);
			}
			else
			{
				leds->setLed(Leds::RFOOT_BUTTON, Leds::COLOR_OFF, true);
			}
		}
		else if (rgc->teams[TEAM_RED].teamNumber == teamNumber)
		{
			leds->setLed(Leds::LFOOT_BUTTON, Leds::COLOR_RED, true);
			if(rgc->kickOffTeam == TEAM_RED)
			{
				leds->setLed(Leds::RFOOT_BUTTON, Leds::COLOR_WHITE, true);
			}
			else
			{
				leds->setLed(Leds::RFOOT_BUTTON, Leds::COLOR_OFF, true);
			}
		}
		//s<<"STATE_READY"<<endl;
		//striker->writeLog(s.str());
		break;
	case STATE_SET:
		leds->setLed(Leds::CHEST_LED, Leds::COLOR_YELLOW, true);

		if (rgc->teams[TEAM_BLUE].teamNumber == teamNumber)
		{
			leds->setLed(Leds::LFOOT_BUTTON, Leds::COLOR_BLUE, true);
			if(rgc->kickOffTeam == TEAM_BLUE)
			{
				leds->setLed(Leds::RFOOT_BUTTON, Leds::COLOR_WHITE, true);
			}
			else
			{
				leds->setLed(Leds::RFOOT_BUTTON, Leds::COLOR_OFF, true);
			}
		}
		else if (rgc->teams[TEAM_RED].teamNumber == teamNumber)
		{
			leds->setLed(Leds::LFOOT_BUTTON, Leds::COLOR_RED, true);
			if(rgc->kickOffTeam == TEAM_RED)
			{
				leds->setLed(Leds::RFOOT_BUTTON, Leds::COLOR_WHITE, true);
			}
			else
			{
				leds->setLed(Leds::RFOOT_BUTTON, Leds::COLOR_OFF, true);
			}
		}
		//s<<"STATE_SET"<<endl;
		//striker->writeLog(s.str());

		break;
	case STATE_PLAYING:

		leds->setLed(Leds::CHEST_LED, Leds::COLOR_GREEN, true);

		//leds->setLed(Leds::LFOOT_BUTTON, Leds::COLOR_OFF, true);
		leds->setLed(Leds::RFOOT_BUTTON, Leds::COLOR_OFF, true);
		//s<<"STATE_PLAYING"<<endl;
		//striker->writeLog(s.str());
		////cerr<<"State Playing"<<endl;

		//s<<"P [0] "<<myTeam->players[0].penalty<<endl;
		//s<<"P [1] "<<myTeam->players[1].penalty<<endl;
		//s<<"P [2] "<<myTeam->players[2].penalty<<endl;
		//s<<"P [3] "<<myTeam->players[3].penalty<<endl;

		//s<<"PN: "<<playerNumber-1<<"("<<myTeam->players[playerNumber-1].penalty<<")\tLast["<<last_penalty<<"]\t";
		//striker->writeLog(s.str());
		//if(myTeam->players[playerNumber-1].penalty != last_penalty)
		//{
		////cerr<<"penalty changed"<<endl;
		if(myTeam->players[playerNumber-1].penalty != PENALTY_NONE)
		{
			//striker->writeLog("processRGC  pause (penalised)\n");
			//pause();
			leds->setLed(Leds::CHEST_LED, Leds::COLOR_RED, true);
			penalised = true;
		}

		if((myTeam->players[playerNumber-1].penalty == PENALTY_NONE))
		{
			//run("Default");
			leds->setLed(Leds::CHEST_LED, Leds::COLOR_GREEN, true);
			penalised = false;
		}

		//last_penalty = myTeam->players[playerNumber-1].penalty;
		//}

		break;
	case STATE_FINISHED:
		//s<<"STATE_FINISHED"<<endl;
		//striker->writeLog(s.str());
		////cerr<<"State Finished"<<endl;
		//pause();

		break;
	default:
		cerr<<"State1 not valid"<<endl;
	};


	//striker->callVoid("activate"); //This call works
	//printRGC(rgc);
	//leds->step();
}

uint8 GameController::getMyColor()// { return myTeam->teamColour;};
{
	if(rgc->teams[TEAM_BLUE].teamNumber == teamNumber)
		return TEAM_BLUE;
	else
		return TEAM_RED;

}

void GameController::updateState( uint8_t st){

	int verylast;

	if (last_gamestate != st){

		//Aunque se llame varias veces seguidas, minimizamos la posibilidad de condiciones de carrera
		verylast = last_gamestate;
		last_gamestate = st;

		turnoff(verylast);

		switch ((int)st){
		case STATE_INITIAL:
			//cout << "[GameController::Initial]\n";
			//leds->setLed(Leds::CHEST_LED, Leds::COLOR_OFF, true);
			//leds->setLed(Leds::CHEST_LED, Leds::COLOR_OFF, true, false);
			//pause();
			break;
		case STATE_READY:
			//cout << "[GameController::Ready]\n";
			//leds->setLed(Leds::CHEST_LED, Leds::COLOR_BLUE, true, false);
			//leds->setLed(Leds::CHEST_LED, Leds::COLOR_BLUE, true);
			break;
		case STATE_SET:
			//leds->setLed(Leds::CHEST_LED, Leds::COLOR_YELLOW, true);
			//leds->setLed(Leds::CHEST_LED, Leds::COLOR_YELLOW, true, false);
			break;
		case STATE_PLAYING:
			//leds->setLed(Leds::CHEST_LED, Leds::COLOR_GREEN, true);
			//leds->setLed(Leds::CHEST_LED, Leds::COLOR_GREEN, true, false);
			break;
		case STATE_FINISHED:
			//leds->setLed(Leds::CHEST_LED, Leds::COLOR_OFF, true);
			//leds->setLed(Leds::CHEST_LED, Leds::COLOR_OFF, true, false);
			break;
		default:
			cerr<<"State2 not valid"<<endl;
		};

	}
}

void GameController::initRGC(RoboCupGameControlData *rgc_data)
{
	////cerr<<"RGC iniciado"<<endl;

	//Init RoboCupGameControlData data if the udp data is not received
	memcpy(rgc_data->header, GAMECONTROLLER_STRUCT_HEADER, sizeof(rgc_data->header));
	rgc_data->version       = GAMECONTROLLER_STRUCT_VERSION;

	rgc_data->playersPerTeam = 4;
	if (playerNumber > 4)
		rgc_data->playersPerTeam = playerNumber;

	rgc_data->state         = STATE_INITIAL;
	rgc_data->firstHalf     = 1;
	rgc_data->kickOffTeam   = TEAM_BLUE;
	rgc_data->secondaryState= STATE2_NORMAL;
	rgc_data->secsRemaining = 0;

	TeamInfo* blueTeam = &(rgc_data->teams[TEAM_BLUE]);
	TeamInfo* redTeam  = &(rgc_data->teams[TEAM_RED]);

	// Set the color of the team (red or blue)
	std::ifstream in2("/home/nao/bica/conf/player.cfg");
	string colorTeam;
	if (in2) {
		in2 >> colorTeam;
	}

	blueTeam->teamColour = TEAM_BLUE;
	blueTeam->score      = 0;
	redTeam->teamColour  = TEAM_RED;
	redTeam->score       = 0;

	if (colorTeam == "blue") {
		blueTeam->teamNumber = teamNumber;
		redTeam->teamNumber  = (uint8_t)-1;        // undefined team
		myTeam = blueTeam;
	} else if (colorTeam == "red") {
		blueTeam->teamNumber = (uint8_t)-1;		   // undefined team
		redTeam->teamNumber  = teamNumber;
		myTeam = redTeam;
	} else {	// Blue
		blueTeam->teamNumber = teamNumber;
		redTeam->teamNumber  = (uint8_t)-1;        // undefined team
		myTeam = blueTeam;
		cerr << begin_red << "GameController::initRGC() Error reading player.cfg file\n" << end_color ;
	}

	for (int team=0; team<1; team++) {
		for (int player=0; player<MAX_NUM_PLAYERS; player++) {
			rgc_data->teams[team].players[player].penalty = PENALTY_NONE;
			rgc_data->teams[team].players[player].secsTillUnpenalised = 0;
		}
	}

	processRGC(NULL);
}

void GameController::sendManualPenalise(void)
{
	//cerr<<"***************************************************************************"<<endl;

	char buf[512];

	RoboCupGameControlReturnData returnPacket;

	memcpy(returnPacket.header, GAMECONTROLLER_RETURN_STRUCT_HEADER, sizeof(returnPacket.header));
	returnPacket.version = GAMECONTROLLER_RETURN_STRUCT_VERSION;
	returnPacket.team = teamNumber;
	returnPacket.player = playerNumber;
	returnPacket.message = myTeam->players[playerNumber-1].penalty?GAMECONTROLLER_RETURN_MSG_MAN_PENALISE:
	GAMECONTROLLER_RETURN_MSG_MAN_UNPENALISE;

	memcpy(buf, &returnPacket, sizeof(RoboCupGameControlReturnData));

	sendUDP(sizeof(RoboCupGameControlReturnData));

	//cerr<<"Sending ["<<returnPacket.message<<"]"<<endl;
}

void GameController::initPlayerConfig(const char *cfgfile)
{
	std::ifstream in(cfgfile);

	if (in) {
		in >> teamNumber >> playerNumber;
		////cerr << "I am player number " << playerNumber << " on team " << teamNumber << endl;
	}
}

/* changes the current team to the new specified team colour */
void GameController::swapTeams(int team) {

	cout << "SWAP\n";

	/* don't swap unnecessarily */
	if ((uint32_t)team == myTeam->teamColour) return;

	rawSwapTeams(rgc);

	/* adjust myTeam */
	if (team == TEAM_BLUE) {
		myTeam = &(rgc->teams[TEAM_BLUE]);
	} else {
		myTeam = &(rgc->teams[TEAM_RED]);
	}

	/* don't change the team colours */
	rgc->teams[TEAM_BLUE].teamColour = TEAM_BLUE;
	rgc->teams[TEAM_RED].teamColour  = TEAM_RED;

	////cerr << "Swapped teams" << endl;

}

void GameController::rawSwapTeams(RoboCupGameControlData* data) {
	size_t    teamSize = sizeof(TeamInfo);
	TeamInfo* blueTeam = &(data->teams[TEAM_BLUE]);
	TeamInfo* redTeam  = &(data->teams[TEAM_RED]);

	TeamInfo tempTeam;
	memcpy(&tempTeam, blueTeam, teamSize);

	/* swap the teams */
	memcpy(blueTeam, redTeam, teamSize);
	memcpy(redTeam, &tempTeam, teamSize);
}

/* compare two byte streams, returns true (match) or false (no match) */
bool GameController::dataEqual(void* data, void* previous) {
	if (!memcmp(previous, data, sizeof(RoboCupGameControlData)))
		return true;
	return false;
}

void GameController::turnoff (uint8_t st)
{
	switch ((int)st){
	case STATE_INITIAL:
		//Turn off chest
		//		leds->setLed(Leds::CHEST_LED, Leds::COLOR_OFF, true, false);
		//leds->setLed(Leds::CHEST_LED, Leds::COLOR_OFF, true, false);

		//Turn off foot leds
		/*if (rgc->teams[TEAM_BLUE].teamNumber == teamNumber)
			leds->setLed(Leds::RFOOT_BUTTON, Leds::COLOR_OFF, true, false);
		else if (rgc->teams[TEAM_RED].teamNumber == teamNumber)
			leds->setLed(Leds::RFOOT_BUTTON, Leds::COLOR_OFF, true, false);
		if(rgc->teams[rgc->kickOffTeam].teamNumber == teamNumber)
			leds->setLed(Leds::LFOOT_BUTTON, Leds::COLOR_OFF, true, false);
		else
			leds->setLed(Leds::LFOOT_BUTTON, Leds::COLOR_OFF, true, false);*/
		break;
	case STATE_READY:
		//leds->setLed(Leds::CHEST_LED, Leds::COLOR_BLUE, true, false);
		//leds->setLed(Leds::CHEST_LED, Leds::COLOR_BLUE, true, false);
		break;
	case STATE_SET:
		//leds->setLed(Leds::CHEST_LED, Leds::COLOR_YELLOW, true, false);
		//leds->setLed(Leds::CHEST_LED, Leds::COLOR_YELLOW, true, false);
		break;
	case STATE_PLAYING:
		//leds->setLed(Leds::CHEST_LED, Leds::COLOR_GREEN, true, false);
		//leds->setLed(Leds::CHEST_LED, Leds::COLOR_GREEN, true, false);
		break;
	case STATE_FINISHED:
		//leds->setLed(Leds::CHEST_LED, Leds::COLOR_OFF, true, false);
		//leds->setLed(Leds::CHEST_LED, Leds::COLOR_OFF, true, false);
		break;
	default:
		cerr<<"State3 not valid"<<endl;
	};

}

void GameController::printRGC(RoboCupGameControlData *rgc_data)
{
	cout<<"Header: "<<rgc_data->header<<" ("<<rgc_data->version<<")"<<endl;
	cout<<"Players per team: "<<(int)rgc_data->playersPerTeam<<endl;
	if(rgc_data->firstHalf == 1) cout<<"First half"<<endl;else cout<<"Second half"<<endl;
	if(rgc_data->kickOffTeam == TEAM_BLUE) cout<<"KickOff Blue"<<endl; else cout<<"KickOff Red"<<endl;
	cout<<"Secondary state: "; if(rgc_data->secondaryState == STATE2_NORMAL) cout<<"Normal"<<endl; else cout<<"Penalty"<<endl;
	if(rgc_data->dropInTeam == TEAM_BLUE) cout<<"Drop In Blue"<<endl; else cout<<"Drop In Red"<<endl;
	cout<<"Drop In Time: "<<rgc_data->dropInTime<<endl;
	cout<<"Seconds remaining in half: "<<rgc_data->secsRemaining<<" seconds."<<endl;

	cout<<"\nBlue Team"<<endl;
	cout<<"\tTeam number: "<<(int)rgc_data->teams[TEAM_BLUE].teamNumber<<endl;
	if(rgc_data->teams[TEAM_BLUE].teamColour == TEAM_BLUE)
		cout<<"\tTeam color: Blue"<<endl;
	else
		cout<<"\tTeam color: Red"<<endl;
	cout<<"\tScore: "<<rgc_data->teams[TEAM_BLUE].score<<endl;

	for(int i=0; i<3/*(int)rgc_data->playersPerTeam*/; i++)
	{
		cout<<"\t\trobot: "<<i<<endl;
		cout<<"\t\t\tPenalty state: ";
		switch(rgc_data->teams[TEAM_BLUE].players[i].penalty)
		{
		case PENALTY_NONE:
			cout<<"PENALTY_NONE"<<endl;
			break;
		case PENALTY_SPL_BALL_HOLDING:
			cout<<"PENALTY_BALL_HOLDING"<<endl;
			break;
		case PENALTY_SPL_PLAYER_PUSHING:
			cout<<"PENALTY_PLAYER_PUSHING"<<endl;
			break;
		case PENALTY_SPL_OBSTRUCTION:
			cout<<"PENALTY_OBSTRUCTION"<<endl;
			break;
		case PENALTY_SPL_INACTIVE_PLAYER:
			cout<<"PENALTY_SPL_INACTIVE_PLAYER"<<endl;
			break;
		case PENALTY_SPL_ILLEGAL_DEFENDER:
			cout<<"PENALTY_ILLEGAL_DEFENDER"<<endl;
			break;
		case PENALTY_SPL_LEAVING_THE_FIELD:
			cout<<"PENALTY_SPL_LEAVING_THE_FIELD"<<endl;
			break;
		case PENALTY_SPL_PLAYING_WITH_HANDS:
			cout<<"PENALTY_SPL_PLAYING_WITH_HANDS"<<endl;
			break;
		case PENALTY_SPL_REQUEST_FOR_PICKUP:
			cout<<"PENALTY_SPL_REQUEST_FOR_PICKUP"<<endl;
			break;
		case PENALTY_MANUAL:
			cout<<"PENALTY_MANUAL"<<endl;
			break;
		default:
			cout<<"Penalty state not valid"<<endl;
		};

		cout<<"\t\t\tSeconds until unpenalize: "<<rgc_data->teams[TEAM_BLUE].players[i].secsTillUnpenalised<<endl;
	}

	cout<<"\nRed Team"<<endl;
	cout<<"\tTeam number: "<<(int)rgc_data->teams[TEAM_RED].teamNumber<<endl;
	if(rgc_data->teams[TEAM_RED].teamColour == TEAM_RED)
		cout<<"\tTeam color: Red"<<endl;
	else
		cout<<"\tTeam color: Blue"<<endl;
	cout<<"\tScore: "<<rgc_data->teams[TEAM_RED].score<<endl;

	for(int i=0;  i<3/*(int)rgc_data->playersPerTeam*/; i++)
	{
		cout<<"\t\trobot: "<<i<<endl;
		cout<<"\t\t\tPenalty state: ";
		switch(rgc_data->teams[TEAM_RED].players[i].penalty)
		{
		case PENALTY_NONE:
			cout<<"PENALTY_NONE"<<endl;
			break;
		case PENALTY_SPL_BALL_HOLDING:
			cout<<"PENALTY_BALL_HOLDING"<<endl;
			break;
		case PENALTY_SPL_PLAYER_PUSHING:
			cout<<"PENALTY_PLAYER_PUSHING"<<endl;
			break;
		case PENALTY_SPL_OBSTRUCTION:
			cout<<"PENALTY_OBSTRUCTION"<<endl;
			break;
		case PENALTY_SPL_INACTIVE_PLAYER:
			cout<<"PENALTY_SPL_INACTIVE_PLAYER"<<endl;
			break;
		case PENALTY_SPL_ILLEGAL_DEFENDER:
			cout<<"PENALTY_ILLEGAL_DEFENDER"<<endl;
			break;
		case PENALTY_SPL_LEAVING_THE_FIELD:
			cout<<"PENALTY_SPL_LEAVING_THE_FIELD"<<endl;
			break;
		case PENALTY_SPL_PLAYING_WITH_HANDS:
			cout<<"PENALTY_SPL_PLAYING_WITH_HANDS"<<endl;
			break;
		case PENALTY_SPL_REQUEST_FOR_PICKUP:
			cout<<"PENALTY_SPL_REQUEST_FOR_PICKUP"<<endl;
			break;
		case PENALTY_MANUAL:
			cout<<"PENALTY_MANUAL"<<endl;
			break;
		default:
			cout<<"Penalty state not valid"<<endl;
		};

		cout<<"\t\t\tSeconds until unpenalize: "<<rgc_data->teams[TEAM_RED].players[i].secsTillUnpenalised<<endl;
	}
	cout<<"____________________________________________________"<<endl;
}

bool GameController::isPenalized()
{
	return (myTeam->players[playerNumber-1].penalty != PENALTY_NONE);
}

bool
GameController::isPenalized(int robotId)
{
	return (myTeam->players[robotId-1].penalty != PENALTY_NONE);
}

void GameController::LeftBumperPressed()
{
	/*if(rgc->state == STATE_INITIAL)
	{
		if(myTeam->teamColour == TEAM_BLUE)
		{
			//cerr<<"Changed to red"<<endl;
			swapTeams(TEAM_RED);
		}
		else
		{
			//cerr<<"Changed to blue"<<endl;
			swapTeams(TEAM_BLUE);
		}

		rgc->kickOffTeam = rgc->kickOffTeam ^ 1;

	}
	processRGC(NULL);*/
}
void GameController::RightBumperPressed()
{

	/*cerr<<"GameController::RightBumperPressed()"<<endl;
	if(rgc->state == STATE_INITIAL)
	{
		 if (rgc->kickOffTeam == TEAM_BLUE)
		 {
			 rgc->kickOffTeam = TEAM_RED;
		 } else {
			 rgc->kickOffTeam = TEAM_BLUE;
		 }
	}
	processRGC(NULL);*/
}
void GameController::ChestButtonPressed()
{
	////cerr<<"GameController::ChestButtonPressedd"<<endl;
	switch(rgc->state)
	{
	case STATE_INITIAL:
	case STATE_READY:
	case STATE_SET:
		if(myTeam->players[playerNumber-1].penalty == PENALTY_NONE)
			myTeam->players[playerNumber-1].penalty = PENALTY_MANUAL;
		else
		{
			myTeam->players[playerNumber-1].penalty = PENALTY_NONE;
			rgc->state = STATE_PLAYING;
		}
		rgc->state = STATE_PLAYING;
		sendManualPenalise();
		break;
	case STATE_PLAYING:
		if(myTeam->players[playerNumber-1].penalty == PENALTY_NONE)
			myTeam->players[playerNumber-1].penalty = PENALTY_MANUAL;
		else
		{
			myTeam->players[playerNumber-1].penalty = PENALTY_NONE;
			rgc->state = STATE_PLAYING;
		}
		sendManualPenalise();
		break;
	default:
		cerr<<"state not recognized"<<endl;
	}
	processRGC(NULL);
}

bool GameController::isMyKickOff()
{
	return rgc->kickOffTeam == myTeam->teamColour;
}
