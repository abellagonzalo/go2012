#ifndef MULTIMEDIAI_ICE
#define MULTIMEDIAI_ICE

#include "common.ice"

module bica {
 
  /** 
  * Interface to the Music component.
  */
  interface MusicManager
  {
  	idempotent void setFileMP3(string file);
	void playPress();
	idempotent void stopPress();
	bool isStopped();
	void setVolume(float volume);
	float getVolume();
  };
  
  /** 
  * Interface to the TextSpeech component.
  */
  interface TextSpeechManager
  {
  	idempotent void setLanguage(string language);
	idempotent void setVoice(string voice);
	idempotent void setVolume(float volume);
	idempotent void setParameter (string pEffectName, float pEffectValue);
	void setmiText(string text);
	string getInfo();
  };
    
  /** 
  * Interface to the Movie component.
  */
  sequence<int> Breakpoints;

  interface MovieManager
  {
  	idempotent void setFile(string file);
	idempotent void stop();
	void playPress(int line);
	idempotent int pausePress();
	int previousPress();
	int nextPress();
	idempotent bool saveFile(string file, string cont);
	Breakpoints getBreakpoints();
	int getLineNow();
	int getBatteryLevel();	
	
	
	idempotent string getAngleRobot(string jointsChain);
  	void setAngleRobot(string jointName);
  	void initTest();
  	void jointName(string joints);
  	void setAngles(string anglesValues);
  	void setTimes(string timesValues);
  	void run();
  	idempotent void disableStiffness(string joints);
  };  
    
};

#endif //MULTIMEDIAI_ICE
