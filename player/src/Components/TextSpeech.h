#ifndef TextSpeech_H
#define TextSpeech_H

#include "Component.h"
#include "Singleton.h"
#include <time.h>

//BUILDER COMMENT. DO NOT REMOVE. auxinclude begin
#include <IceE/IceE.h>
#include <multimediaI.h>

using namespace bica;
//BUILDER COMMENT. DO NOT REMOVE. auxinclude end

class TextSpeech : public Component, public Singleton<TextSpeech>, public TextSpeechManager
{
public:

	TextSpeech();
	~TextSpeech();

	void stop();
	void step();
	void init(const string newName, AL::ALPtr<AL::ALBroker> parentBroker);
	void setmiText(string text);
	void setLanguage(string language);
	void setVoice(string voice);
	void setVolume(float volume);
	void setParameter (string pEffectName,float pEffectValue);

	string miText;
	int idText;
	bool isStopped();
	bool IsSaying;
	bool IsActive;
		time_t timenow,timesaying;


private:
	static const double twait = 1.5;

	static const int Initial	= 0;
	static const int Speech	= 1;

	int state;


	void Initial_state_code(void);
	void Speech_state_code(void);
	bool Speech2Initial0_transition_code(void);
	bool Initial2Speech0_transition_code(void);
	AL::ALPtr<AL::ALProxy> ptspeech;
//BUILDER COMMENT. DO NOT REMOVE. auxcode begin
public:
	virtual void setLanguage(const string& language, const Ice::Current& c);
	virtual void setVoice(const string& voice, const Ice::Current& c);
	virtual void setVolume(float volume, const Ice::Current& c);
	virtual void setParameter (const string& pEffectName, float pEffectValue, const Ice::Current& c);
	virtual void setmiText(const string& text, const Ice::Current& c);
	virtual string getInfo(const Ice::Current& c);
//BUILDER COMMENT. DO NOT REMOVE. auxcode end
};

#endif

