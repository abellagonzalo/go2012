/*
 * Leds.cpp
 *
 *  Created on: 26-may-2009
 *      Author: Carlos Agüero
 */

#include "Leds.h"

const float Leds::DEFAULT_DURATION = 0.01;

const int Leds::NOCOLOR		= 0x00000000;
const int Leds::RED 		= 0x00FF0000;
const int Leds::GREEN 		= 0x0000FF00;
const int Leds::BLUE 		= 0x000000FF;
const int Leds::YELLOW		= 0x00FFFF00;
const int Leds::WHITE 		= 0x00FFFFFF;
const int Leds::ORANGE 		= 0x00FF3500;
	
const string Leds::lednames[NUM_LEDS] = {
	string("ChestLeds"),
	string("LeftFootLeds"),
	string("RightFootLeds"),
	string("FaceLedsLeftBottom"),
	string("FaceLedsLeftTop"),
	string("FaceLedsLeftExternal"),
	string("FaceLedsLeftInternal"),
	string("RightFaceLeds")
	};

const string Leds::colornames[NUM_COLORS] = {
	string("NOCOLOR"),
	string("RED"),
	string("BLUE"),
	string("GREEN"),
	string("YELLOW"),
	string("WHITE"),
	string("ORANGE")
	};

Leds::Leds() {
	for(int i = 0; i < NUM_LEDS; i++)
	{
		led_idx[i] = 0;
		actual[i] = -1;
		
		for(int j = 0; j < NUM_COLORS; j++)
			leds[i][j] = false;
	}
	
	active = false;
	refreshLeds();
}

Leds::~Leds() {
}

void
Leds::refreshLeds()
{
	if(!active)
		return;

	bool set = false;

	for(int i = 0; i < NUM_LEDS; i++)
	{
		colorLed(i, led_idx[i]);
	}
	/*
		int old;

		old = led_idx[i];

		led_idx[i] = (led_idx[i] + 1) % NUM_COLORS;

		set = false;

		for(int j = 0; j < NUM_COLORS; j++)
		{

			if(!set)
			{
				if(leds[i][led_idx[i]] == true)
				{
					set = true;

					if(led_idx[i] != old)
						colorLed(i, led_idx[i]);
				}
				else
					led_idx[i] = (led_idx[i] + 1) % NUM_COLORS;
			}
		}

		if(!set)
			colorLed(i, COLOR_OFF);
	}*/
}

void
Leds::init(const string newName, AL::ALPtr<AL::ALBroker> parentBroker)
{
	try{
		pleds = AL::ALPtr<AL::ALLedsProxy>(new AL::ALLedsProxy(parentBroker));
		active = true;//s<<"OK"<<endl;
	}catch( AL::ALError& e)
	{
		//cerr<<"[Leds::init] : "<<e.toString()<<endl;
		active = false;
	}
}



void
Leds::setLed(int led, int color, bool value)
{

	//cerr<<"SetLed "<<led<<" "<<color<<endl;
	//if(leds[led][color] != value)
	//{
		leds[led][color] = value;
		led_idx[led] = color;
		if(led_idx[led] < 0) led_idx[led] = 0;

		if (((actual[led] == color) && !value) || ((actual[led] != color) && value))
			refreshLeds();
	//}
}

void
Leds::colorLed(int led, int color)
{

	actual[led] = color;

	if(active)
	{
		try {
			switch(color)
			{
			case COLOR_OFF:
				pleds->fadeRGB(lednames[led], NOCOLOR, DEFAULT_DURATION);
				break;
			case COLOR_RED:
				pleds->fadeRGB(lednames[led], RED, DEFAULT_DURATION);
				break;
			case COLOR_BLUE:
				pleds->fadeRGB(lednames[led], BLUE, DEFAULT_DURATION);
				break;
			case COLOR_GREEN:
				pleds->fadeRGB(lednames[led], GREEN, DEFAULT_DURATION);
				break;
			case COLOR_YELLOW:
				pleds->fadeRGB(lednames[led], YELLOW, DEFAULT_DURATION);
				break;
			case COLOR_WHITE:
				pleds->fadeRGB(lednames[led], WHITE, DEFAULT_DURATION);
				break;
			case COLOR_ORANGE:
				pleds->fadeRGB(lednames[led], ORANGE, DEFAULT_DURATION);
				break;
			default:
				cerr<<"Leds::colorLed\tcolor not valid"<<endl;
			};
		}catch(AL::ALError& e)
		{
			cerr<<"[Leds::colorLed()] Error: "<<e.toString()<<endl;
		}
	}
}


void
Leds::print(int led)
{
	/*ostringstream s;

	s<<lednames[led]<<": ";
	for(int j = 0; j < NUM_COLORS; j++)
	{
		s<<"\t"<<colornames[j]<<"(";
		if(leds[led][j] == true) s<<"on)";
		else s<<"off)";
	}
	s<<endl;
	writeLog(s.str());
	*/
}
