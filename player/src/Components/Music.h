#ifndef Music_H
#define Music_H

#include "Component.h"
#include "Singleton.h"

//BUILDER COMMENT. DO NOT REMOVE. auxinclude begin
#include <IceE/IceE.h>
#include <multimediaI.h>
#include <iostream>
#include <fstream>

extern "C" {
	#include <gst/gst.h>
	#include <glib.h>
}

using namespace bica;
//BUILDER COMMENT. DO NOT REMOVE. auxinclude end

class Music : public Component, public Singleton<Music>, public MusicManager
{
public:

	Music();
	~Music();

	void step();
private:

	static const int Initial	= 0;
	static const int Ready	= 1;
	static const int Playing	= 2;
	int state;


	void Initial_state_code(void);
	void Ready_state_code(void);
	void Playing_state_code(void);
	bool Initial2Ready0_transition_code(void);
	bool Ready2Playing0_transition_code(void);
	bool Playing2Initial0_transition_code(void);
//BUILDER COMMENT. DO NOT REMOVE. auxcode begin

	GMainLoop *loop;
  	GstElement *play;
  	GstBus *bus;
	string fileMp3;

	bool isSetFileMp3;
	bool isPlayPress;
	bool isStopPress;

public:
	void init(const string newName, AL::ALPtr<AL::ALBroker> parentBroker);

	void setFileMP3(string file);
	void playPress();
	void stopPress();
	void stop();
	bool isStopped();
	float getVolume();
	void setVolume(float volume);

	void setFileMP3(const string& file, const Ice::Current& c);
	void playPress(const Ice::Current& c);
	void stopPress(const Ice::Current& c);
	bool isStopped(const Ice::Current& c);
	void setVolume(float volume, const Ice::Current& c);
	float getVolume(const Ice::Current& c);
//BUILDER COMMENT. DO NOT REMOVE. auxcode end
};

#endif

