#ifndef GameController_H
#define GameController_H

#include "RoboCupGameControlData.h"
#include "Net/UdpServer.h"
#include "Leds.h"

class GameController: public UdpServer, public Singleton<GameController>
{
public:
	GameController();

	void processRGC(RoboCupGameControlData *rgc_data);
    void printRGC(RoboCupGameControlData *rgc_data);
    void initRGC(RoboCupGameControlData *rgc_data);
	bool validatePacket(RoboCupGameControlData *data);
	void UDPmanageRecv(int size);
	RoboCupGameControlData *getRgc() { return rgc;};
	bool isPenalized();
	bool isPenalized(int robotId);
	uint8 getMyColor();// { return myTeam->teamColour;};

	int getPlayerNumber() {return playerNumber;};
	bool isMyKickOff();

	void LeftBumperPressed();
	void RightBumperPressed();
	void ChestButtonPressed();
private:

	RoboCupGameControlData *rgc;

	uint8_t last_gamestate;
    TeamInfo* myTeam;
    uint32_t    teamNumber;
    int       playerNumber;

	void updateState( uint8_t st);
	void turnoff (uint8_t st);
	void sendManualPenalise(void);

	void colorLed(int led, int color);
	void initPlayerConfig(const char *cfgfile);
	void rawSwapTeams(RoboCupGameControlData* data);
	void swapTeams(int team);
	bool checkHeader(char* header);
	bool dataEqual(void* data, void* previous);
	bool isThisGame(RoboCupGameControlData* gameData);

	bool penalised;
	Leds *leds;
};

#endif
