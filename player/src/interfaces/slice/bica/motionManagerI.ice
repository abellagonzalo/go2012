#ifndef MOTION_MANAGERI_ICE
#define MOTION_MANAGERI_ICE

#include "common.ice"
//#include <Ice/Identity.ice>

module bica {
  
  /** 
  * Interface to the head motion.
  */
  interface HeadMotionManager
  {
  	idempotent void setPan(float newPanVel);
  	idempotent void setPanPos (float rads, float vel);
  	idempotent void setTilt(float tiltVel);
  	idempotent void setTiltPos(float rads, float vel);
  	idempotent void stop();
  };
  
  /** 
  * Interface to the body motion.
  */
  interface BodyMotionManager
  {
  	void setVel(float v, float w, float l);
  	idempotent void poseInit();
  	void doMove(string kick);
	bool isWalking();
	bool isMoving();
	//void addClient(Ice::Identity ident);
  };
  
  /** 
  * Interface to the robot joints
  */
  //interface JointsManager
  //{
  	
  //};

  /**
  * Interface CallbackReceiver
  */
  //interface CallbackReceiver
  //{
  //  void callback(int num);
  //};
};

#endif //MOTION_MANAGERI_ICE
