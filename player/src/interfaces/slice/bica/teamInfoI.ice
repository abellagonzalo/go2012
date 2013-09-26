#ifndef TEAMINFOI_ICE
#define TEAMINFOI_ICE

#include "common.ice"

module bica {
  
  /**
   *  Static description of the opinion object used in Switch (Role allocation).
   */
  class SwitchOpinion 
  {
    int 	observerRobot;
	int 	observedRobot;
	int 	x;
	int 	y;
	float 	theta;
	string 	posReliability;
	float   ballDistance;
	float   ballX;
	float   ballY;
	int 	ballTimeSinceLastSeen;
	string	ballReliability;
	float	utilityGoalie;
	float 	utilityKicker;
	float	utilityDefender;
	float	utilitySupporter;
	short	role;
  };
  
   /**
   *  Static description of the opinionSet used in Switch (Role allocation).
   */
  sequence<SwitchOpinion> OpinionsList; 
 
  /** 
  * Interface to the TeamInfo.
  */
  interface TeamInfoProvider
  {
  	idempotent string getTeamColor();
  	//["ami"] SwitchOpinion getSwitchInfo();
  	void putSwitchInfo(SwitchOpinion opinion);
  	OpinionsList getOpinionsList();
  	void hello(int robotId);
  };
    
};

#endif //TEAMINFOI_ICE
