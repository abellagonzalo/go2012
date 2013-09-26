#ifndef LedsControl_H
#define LedsControl_H

#include "Component.h"
#include "Singleton.h"
#include "alproxies/alledsproxy.h"

//BUILDER COMMENT. DO NOT REMOVE. auxinclude begin
//BUILDER COMMENT. DO NOT REMOVE. auxinclude end

class LedsControl : public Component, public Singleton<LedsControl>
{
public:

	LedsControl();
	~LedsControl();

	void init(const string newName, AL::ALPtr<AL::ALBroker> parentBroker);
	void step();

	void setLed(int led, int color, bool value,bool automatic);

	bool isSetLed;
	bool isAutomatic;

	static const int NUM_LEDS	 	= 25;
	static const int CHEST_LED 		= 0;
	static const int LFOOT_LED	 	= 1;
	static const int RFOOT_LED	 	= 2;
	static const int LEYEL_LED	 	= 3;
	static const int LEYEU_LED	 	= 4;
	static const int LEYEE_LED	 	= 5;
	static const int LEYEI_LED	 	= 6;
	static const int REYE_LED	 	= 7;

	static const int NUM_COLORS		= 7;
	static const int COLOR_OFF 		= 0;
	static const int COLOR_RED 		= 1;
	static const int COLOR_BLUE	 	= 2;
	static const int COLOR_GREEN 	= 3;
	static const int COLOR_YELLOW 	= 4;
	static const int COLOR_WHITE 	= 5;
	static const int COLOR_ORANGE 	= 6;

	static const int NUM_BUTTONS 	= 3;
	static const int CHEST_BUTTON 	= 0;
	static const int LFOOT_BUTTON 	= 1;
	static const int RFOOT_BUTTON 	= 2;

	static const int NOCOLOR;
	static const int RED;
	static const int GREEN;
	static const int BLUE;
	static const int YELLOW;
	static const int WHITE;
	static const int ORANGE;

	static const float DEFAULT_DURATION;
private:

	static const int Initial	= 0;
	static const int SetUpdate	= 1;
	static const int Change	= 2;
	int state;

	void refreshLeds();
	void colorLed(int led, int color);


	static const string lednames[NUM_LEDS];
	static const string colornames[NUM_COLORS];

	bool leds[NUM_LEDS][NUM_COLORS];
	int led_idx[NUM_LEDS];

	int actual[NUM_LEDS];

	void print(int led);

	//AL::ALPtr<AL::ALLedsProxy> pleds;
	ALPtr<ALLedsProxy>	pleds;
	ALPtr<ALBroker> parent;


	bool active;

	void Initial_state_code(void);
	void SetUpdate_state_code(void);
	void Change_state_code(void);
	bool Initial2SetUpdate0_transition_code(void);
	bool SetUpdate2Change0_transition_code(void);
	bool Change2SetUpdate0_transition_code(void);
//BUILDER COMMENT. DO NOT REMOVE. auxcode begin
//BUILDER COMMENT. DO NOT REMOVE. auxcode end
};

#endif

