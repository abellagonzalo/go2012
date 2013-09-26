#ifndef Movie_H
#define Movie_H

#include "Component.h"
#include "Body.h"
#include "Music.h"
#include "TextSpeech.h"
#include "LedsControl.h"
#include "FootContact.h"
#include "Singleton.h"
#include <iostream>
#include <fstream>
#include <time.h>

//BUILDER COMMENT. DO NOT REMOVE. auxinclude begin
#include <IceE/IceE.h>
#include <multimediaI.h>

using namespace bica;
//BUILDER COMMENT. DO NOT REMOVE. auxinclude end

class Movie : public Component, public Singleton<Movie>, public MovieManager
{
public:

	Movie();
	~Movie();

	void step();
	void setFile(string file);
	void stop();
	void playPress(int line);
	int pausePress();
	int previousPress();
	int nextPress();
	bool saveFile(string file, string cont);
	std::vector<int> getBreakpoints();
	int getLineNow();
	int getBatteryLevel();

	static const int NUM_BREAK	 	= 200;

	time_t timewait,timenow;
	double twait;
	string cmd;
	string OrderWait;
	bool IsSetFileMovie;
	bool IsFinishMov;
	bool IsFinishMusic;
	bool IsFinishWait;
	bool IsFinishTalk;
	bool IsFinishLeghts;
	bool IsFinishWalk;
	bool IsPressLeft;
	bool IsPressRight;
	bool IsPressChest;
	bool IsplayPress;
	bool FirstTime;
	int lineStart;
	int lineNow;
	int numBreak;
	int Breakpoint[NUM_BREAK];
	int nextBreak;
	ifstream infile;
	ifstream filebreak;
	ofstream outfile;
private:
	string fileMovie;
	static const int Initial	= 0;
	static const int setFileMovie	= 1;
	static const int Running	= 2;
	static const int Waiting	= 3;
	int state;
	bool stiffness; //True esta activado, False no lo esta

	Body *_Body;
	Music *_Music;
	TextSpeech *_TextSpeech;
	LedsControl *_LedsControl;
	FootContact *_FootContact;
    
    AL::ALPtr<AL::ALProxy> psentinel;

	ALValue motors, allAngles, allTimes, angles, times;

	void Initial_state_code(void);
	void setFileMovie_state_code(void);
	void Running_state_code(void);
	void Waiting_state_code(void);
	bool Initial2setFileMovie0_transition_code(void);
	bool setFileMovie2Running0_transition_code(void);
	bool Running2Waiting0_transition_code(void);
	bool Waiting2Running0_transition_code(void);

	void get_command(string s);
	void InitBreakpoint(string file);
	list<string>  getTokens(string data,char delimiter);


//BUILDER COMMENT. DO NOT REMOVE. auxcode begin
public:
	void setFile(const string& file, const Ice::Current& c);
	void stop(const Ice::Current& c);
	void playPress(int line, const Ice::Current& c);
	int pausePress(const Ice::Current& c);
	int previousPress(const Ice::Current& c);
	int nextPress(const Ice::Current& c);
	bool saveFile(const string& file, const string& cont, const Ice::Current& c);
	std::vector<int> getBreakpoints(const Ice::Current& c);
	int getLineNow(const Ice::Current& c);
   	int getBatteryLevel(const Ice::Current& c);

	string getAngleRobot(const string& jointsChain, const Ice::Current& c);
	void setAngleRobot(const string& jointName, const Ice::Current& c);
	void initTest(const Ice::Current& c);
	void jointName(const string& joints, const Ice::Current& c);
	void setAngles(const string& anglesValues, const Ice::Current& c);
	void setTimes(const string& timesValues, const Ice::Current& c);
	void run(const Ice::Current& c);
	void disableStiffness(const string& joints, const Ice::Current& c);
//BUILDER COMMENT. DO NOT REMOVE. auxcode end
};

#endif

