#include "TextSpeech.h"

TextSpeech::TextSpeech()
{

	state = Initial;
}

TextSpeech::~TextSpeech()
{

}

void TextSpeech::stop(){
	miText="";
	IsActive = false;
	IsSaying = false;
	ptspeech->pCall("stop",idText);

}

void TextSpeech::Initial_state_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Initial_state_code
//BUILDER COMMENT. DO NOT REMOVE. end Initial_state_code
}

void TextSpeech::Speech_state_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Speech_state_code
	/*El robot habla.
	 *
	 */ 
	//ptspeech->callVoid("say",miText);
	if (not IsSaying){

		idText=ptspeech->pCall("say",miText);
		IsSaying=true;
		IsActive = true;
		time(&timesaying);

	}else
		time(&timenow);

		if (difftime(timenow,timesaying)>=twait){
				if (not ptspeech->call<bool>("isRunning","say")){
						IsActive=false;
						miText = "";
				}
	}
	/*Despues de hablar miText = ""*/

//BUILDER COMMENT. DO NOT REMOVE. end Speech_state_code
}

bool TextSpeech::Speech2Initial0_transition_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Speech2Initial0_transition_code
return true;//BUILDER COMMENT. DO NOT REMOVE. end Speech2Initial0_transition_code
}

bool TextSpeech::Initial2Speech0_transition_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Initial2Speech0_transition_code
if (miText!="") return true;
else return false;//BUILDER COMMENT. DO NOT REMOVE. end Initial2Speech0_transition_code
}

void TextSpeech::setLanguage(string language){
	//printf("Lenguage %s\n",language.c_str());
	ptspeech->callVoid("setLanguage",language);

}

void TextSpeech::setVoice(string voice){
	ptspeech->callVoid("setVoice",voice);
}

void TextSpeech::setVolume(float volume){
	ptspeech->callVoid("setVolume",volume);
}

void TextSpeech::setParameter (string pEffectName,float pEffectValue){
	ptspeech->callVoid("setParameter",pEffectName,pEffectValue);
}



void TextSpeech::init(const string newName, AL::ALPtr<AL::ALBroker> parentBroker) {
		Component::init(newName, parentBroker);
	try {
	ptspeech = getParentBroker()->getProxy("ALTextToSpeech");
	ptspeech->callVoid("setParameter","pitchShift",1.3);

	} catch (ALError& e) {
		cerr << "[TextToSpeech::init()] " << e.toString() << endl;
	}


}


void
TextSpeech::step(void)
{
	switch (state)
	{
	case Initial:
		if (isTime2Run()) {
			Initial_state_code();

			if (Initial2Speech0_transition_code()) {
				state = Speech;
			}
		}

		break;
	case Speech:
		if (isTime2Run()) {
			Speech_state_code();

			if (Speech2Initial0_transition_code()) {
				state = Initial;
			}
		}

		break;
	default:
		cout << "[TextSpeech::step()] Invalid state\n";
	}
}
void TextSpeech::setmiText(string text){
	miText = text;
	IsSaying = false;
	IsActive = true;
}


bool TextSpeech::isStopped(){

	return not IsActive;
}

//BUILDER COMMENT. DO NOT REMOVE. auxcode begin
void
TextSpeech::setLanguage(const string& language, const Ice::Current& c)
{
	setLanguage(language);
}

void
TextSpeech::setVoice(const string& voice, const Ice::Current& c)
{
	setVoice(voice);
}

void
TextSpeech::setVolume(float volume, const Ice::Current& c)
{
	setVolume(volume);
}

void
TextSpeech::setParameter (const string& pEffectName, float pEffectValue, const Ice::Current& c)
{
	setParameter(pEffectName, pEffectValue);
}

void
TextSpeech::setmiText(const string& text, const Ice::Current& c)
{
	setmiText(text);
}

string
TextSpeech::getInfo(const Ice::Current& c)
{
	std::stringstream out;
	string language, voice;
	float volume;

	language=ptspeech->call<string>("getLanguage");
	voice=ptspeech->call<string>("getVoice");
	volume=ptspeech->call<float>("getVolume");

	out << language << ":" << voice << ":" << volume << ":" << endl;
	return out.str();
}
//BUILDER COMMENT. DO NOT REMOVE. auxcode end
