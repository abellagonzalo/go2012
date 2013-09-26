#ifndef COMPONENTSI_ICE
#define COMPONENTSI_ICE

#include "common.ice"
#include "image.ice"

module bica {
 
  /** 
  * Interface to the Go2gpos component.
  */
  interface Go2posManager
  {
  	idempotent void setGoalPos( double x, double y);
  };
  
   /** 
  * Interface to the pruebag2gpos component.
  */
  interface pruebag2gposManager
  {
  	idempotent void setGoalPos( double x, double y);
  };
  
  /** 
  * Interface to the NEKFLocalization component.
  */
  interface NEKFLineDbgManager
  {
  	idempotent void setPos( float x, float y, float t);
  };
  
  
  /** 
  * Interface to the GHead component.
  */
  interface GHeadManager
  {
  	idempotent void lookAt( float x, float y, float z);
  };
  
 
  /** 
  * Interface to the Obstacles component.
  */
  interface ObstaclesManager
  {
  	idempotent ImageData getDbgImg();
  };
  
  /** 
  * Interface to a generic component.
  */
  interface ComponentInfoProvider
  {
  	idempotent string getDebugData(string component);
  };
  
  
  /**
   *  Static description of the visual memory object.
   */
  class VisualMemoryObj 
  {
    float x;
    float y;
   	float dx;
    float dy;
    float quality;
    float time;
    string reliability;
  };
  /**
  * Interface to the ball detector
  */
  interface BallDetectorManager
  {
	idempotent VisualMemoryObj getVisualMemoryObject();
	idempotent void predictionUpdate();
	idempotent void measurementUpdate();
	void setGTBallAbs( double x, double y);
  };
 
   /**
  * Interface to the W3D ball detector
  */
  interface W3DDetectorManager
  {
	idempotent VisualMemoryObj getVisualMemoryObject();
	idempotent void predictionUpdate();
	idempotent void measurementUpdate();
  };
  
   /**
  * Interface to the goal detector
  */
  interface GoalDetectorManager
  {
  	idempotent VisualMemoryObj getVisualMemoryObject(string obj);
	idempotent void predictionUpdate();
	idempotent void measurementUpdate();
  };
    
};

#endif //COMPONENTSI_ICE
