/*
 * Leds.h
 *
 *  Created on: 26-may-2009
 *      Author: Carlos Agüero
 *  Modified on: 25-jun-2009
 *      Author: Francisco Martín
 */

#ifndef LEDS_H_
#define LEDS_H_

#include "Component.h"
#include "alproxies/alledsproxy.h"
#include "Singleton.h"

class Leds: public Singleton<Leds>
{
public:
	Leds();
	virtual ~Leds();

	void init(const string newName, AL::ALPtr<AL::ALBroker> parentBroker);

	void setLed(int led, int color, bool value);

	void step(void);

	static const int NUM_LEDS	 	= 8;
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


	void refreshLeds();
	void colorLed(int led, int color);
	void fadeRGB(string, int, float);

	static const string lednames[NUM_LEDS];
	static const string colornames[NUM_COLORS];

	bool leds[NUM_LEDS][NUM_COLORS];
	int led_idx[NUM_LEDS];

	int actual[NUM_LEDS];

	void print(int led);

	AL::ALPtr<AL::ALLedsProxy> pleds;
	
	bool active;

};

#endif /* LEDS_H_ */
