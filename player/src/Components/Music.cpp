#include "Music.h"

//BUILDER COMMENT. DO NOT REMOVE. auxinclude begin
//BUILDER COMMENT. DO NOT REMOVE. auxinclude end

Music::Music()
{
	state = Initial;
}

Music::~Music()
{

}

void Music::Initial_state_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Initial_state_code
//BUILDER COMMENT. DO NOT REMOVE. end Initial_state_code
}

void Music::Ready_state_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Ready_state_code
//BUILDER COMMENT. DO NOT REMOVE. end Ready_state_code
}

void Music::Playing_state_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Playing_state_code
	#ifdef WEBOTS
	return;
	#endif

	GstMessage* message = gst_bus_timed_pop(bus, 0);
	if ((message != NULL) && (GST_MESSAGE_TYPE(message) == GST_MESSAGE_EOS)) {
		isStopPress = true;
	}
//BUILDER COMMENT. DO NOT REMOVE. end Playing_state_code
}

bool Music::Initial2Ready0_transition_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Initial2Ready0_transition_code
	#ifdef WEBOTS
	return false;
	#endif

	if (!isSetFileMp3) { return false; }

	ifstream file(fileMp3.c_str(), ifstream::in);
	if (!file.good()) { return false; }
	file.close();

	g_object_set (G_OBJECT (play), "uri", fileMp3.c_str(), NULL);
	gst_element_set_state (play, GST_STATE_PAUSED);

	return true;
//BUILDER COMMENT. DO NOT REMOVE. end Initial2Ready0_transition_code
}

bool Music::Ready2Playing0_transition_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Ready2Playing0_transition_code
	#ifdef WEBOTS
	return false;
	#endif

	if (!isPlayPress) { return false; }
	gst_element_set_state(play, GST_STATE_PLAYING);
	return true;
//BUILDER COMMENT. DO NOT REMOVE. end Ready2Playing0_transition_code
}

bool Music::Playing2Initial0_transition_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Playing2Initial0_transition_code
	#ifdef WEBOTS
	return false;
	#endif

	if (!isStopPress) { return false; }

	isSetFileMp3 = false;
	isPlayPress = false;
	isStopPress = false;
	gst_element_set_state (play, GST_STATE_READY);

	return true;
//BUILDER COMMENT. DO NOT REMOVE. end Playing2Initial0_transition_code
}

void
Music::step(void)
{
	switch (state)
	{
	case Initial:

		if (isTime2Run()) {
			Initial_state_code();

			if (Initial2Ready0_transition_code()) {
				state = Ready;
			}
		}

		break;
	case Ready:

		if (isTime2Run()) {
			Ready_state_code();

			if (Ready2Playing0_transition_code()) {
				state = Playing;
			}
		}

		break;
	case Playing:

		if (isTime2Run()) {
			Playing_state_code();

			if (Playing2Initial0_transition_code()) {
				state = Initial;
			}
		}

		break;
	default:
		cout << "[Music::step()] Invalid state\n";
	}
}

//BUILDER COMMENT. DO NOT REMOVE. auxcode begin
void Music::init(const string newName, AL::ALPtr<AL::ALBroker> parentBroker) {
	Component::init(newName, parentBroker);

	#ifdef WEBOTS
	return;
	#endif

	// init GStreamer
	gst_init (NULL, NULL);
  	loop = g_main_loop_new (NULL, FALSE);

  	// set up
  	play = gst_element_factory_make ("playbin2", "play");
	bus = gst_pipeline_get_bus (GST_PIPELINE (play));

	// set state
	gst_element_set_state (play, GST_STATE_READY);

	isSetFileMp3 = false;
	isPlayPress = false;
	isStopPress = false;
}

void Music::setFileMP3(string file)
{
	fileMp3 = "file://";
	fileMp3 += file;
	isSetFileMp3 = true;
}

void Music::playPress()
{
	isPlayPress = true;
}

void Music::stopPress()
{
	isStopPress = true;
}

void Music::stop()
{
	isStopPress = true;
}

bool Music::isStopped(){
	return !isPlayPress;
}

void Music::setVolume(float volume){
	#ifdef WEBOTS
	return;
	#endif

	if (volume < 0.0) volume = 0.0;
	if (volume > 1.0) volume = 1.0;
	g_object_set(play,"volume",volume,NULL);
}

float Music::getVolume(){
	#ifdef WEBOTS
	return 1.0;
	#endif

	double volume;
	g_object_get(play, "volume", &volume, NULL);
	return volume;
}

void
Music::setFileMP3(const string& file, const Ice::Current& c)
{
	setFileMP3(file);
}

void
Music::playPress(const Ice::Current& c)
{
	playPress();
}

void
Music::stopPress(const Ice::Current& c)
{
	stopPress();
}

bool
Music::isStopped(const Ice::Current& c){
	return isStopped();
}

void 
Music::setVolume(float volume, const Ice::Current& c)
{
	setVolume(volume);
}

float
Music::getVolume(const Ice::Current& c)
{
	return getVolume();
}
//BUILDER COMMENT. DO NOT REMOVE. auxcode end

