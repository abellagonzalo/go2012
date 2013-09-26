#ifndef LOCALIZATIONI_ICE
#define LOCALIZATIONI_ICE

#include "common.ice"
#include "image.ice"

module bica {
 
  /** 
  * Interface to the image provider.
  */
  interface WorldModelProvider
  {
    /** 
     * Returns the current world model.
     */
    idempotent string getWorldModel();    
  };
  
  /** 
  * Interface to the auto-localization
  */
  interface LocalizationProvider
  {
    /** 
     * Get an image to debug the observation model.
     */
    idempotent ImageData getLocalizationImg();
    idempotent string getLocalizationInfo();
    idempotent float getReliability();
    idempotent void resetLocalization();
    idempotent string getRealPosition();
	void setRealPosition(double xpos, double ypos, double thetapos);
  };
   
};

#endif //LOCALIZATIONI_ICE
