#include "LedsControl.h"


const float LedsControl::DEFAULT_DURATION = 5.01;

const int LedsControl::NOCOLOR		= 0x00000000;
const int LedsControl::RED 		= 0x00FF0000;
const int LedsControl::GREEN 		= 0x0000FF00;
const int LedsControl::BLUE 		= 0x000000FF;
const int LedsControl::YELLOW		= 0x00FFFF00;
const int LedsControl::WHITE 		= 0x00FFFFFF;
const int LedsControl::ORANGE 		= 0x00FF3500;

const string LedsControl::lednames[NUM_LEDS] = {
	string("AllLeds"),
	string("EarLeds"),
	string("ChestLeds"),
	string("LeftFootLeds"),
	string("RightFootLeds"),
	string("FaceLeds"),
	string("LeftFaceLeds"),
	string("RightFaceLeds"),
	string("FeetLeds"),
	string("RigthEarLeds"),
	string("RigthEarLedsBack"),
	string("RigthEarLedsEven"),
	string("RightEarLedsFront"),
	string("LeftEarLeds"),
	string("LeftEarLedsBack"),
	string("LeftEarLedsEven"),
	string("LeftEarLedsFront"),
	string("FaceLedsLeftBottom"),
	string("FaceLedsLeftTop"),
	string("FaceLedsLeftExternal"),
	string("FaceLedsLeftInternal"),
	string("FaceLedsRightBottom"),
	string("FaceLedsRightTop"),
	string("FaceLedsRightExternal"),
	string("FaceLedsRightInternal")
	};

const string LedsControl::colornames[NUM_COLORS] = {
	string("NOCOLOR"),
	string("RED"),
	string("BLUE"),
	string("GREEN"),
	string("YELLOW"),
	string("WHITE"),
	string("ORANGE")
	};


LedsControl::LedsControl()
{

	state = Initial;
	for(int i = 0; i < NUM_LEDS; i++)
		{
			//printf("LedName[%d]: %s\n",i,lednames[i].c_str());
			led_idx[i] = 0;
			actual[i] = -1;

			for(int j = 0; j < NUM_COLORS; j++)
				leds[i][j] = false;
		}

		active = false;
		//refreshLeds();

}

LedsControl::~LedsControl()
{

}

void
LedsControl::init(const string newName, AL::ALPtr<AL::ALBroker> parentBroker)
{
	try{
		parent = parentBroker;
		pleds = parentBroker->getLedsProxy();
		active = true;//s<<"OK"<<endl;
		isSetLed = false;
		isAutomatic = false;
	}catch( AL::ALError& e)
	{
		cerr<<"****************[LedsControl::init] : "<<e.toString()<<endl;
		active = false;
	}
}



void LedsControl::Initial_state_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Initial_state_code
//BUILDER COMMENT. DO NOT REMOVE. end Initial_state_code
}

void LedsControl::SetUpdate_state_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin SetUpdate_state_code
	if (isSetLed)
		refreshLeds();
//BUILDER COMMENT. DO NOT REMOVE. end SetUpdate_state_code
}

void LedsControl::Change_state_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Change_state_code
//BUILDER COMMENT. DO NOT REMOVE. end Change_state_code
}

bool LedsControl::Initial2SetUpdate0_transition_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Initial2SetUpdate0_transition_code
	return true;
//BUILDER COMMENT. DO NOT REMOVE. end Initial2SetUpdate0_transition_code
}

bool LedsControl::SetUpdate2Change0_transition_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin SetUpdate2Change0_transition_code
	return true;
//BUILDER COMMENT. DO NOT REMOVE. end SetUpdate2Change0_transition_code
}

bool LedsControl::Change2SetUpdate0_transition_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Change2SetUpdate0_transition_code
return true;
//BUILDER COMMENT. DO NOT REMOVE. end Change2SetUpdate0_transition_code
}

void
LedsControl::step(void)
{
	switch (state)
	{
	case Initial:

		if (isTime2Run()) {
			Initial_state_code();

			if (Initial2SetUpdate0_transition_code()) {
				state = SetUpdate;
			}
		}

		break;
	case SetUpdate:

		if (isTime2Run()) {
			SetUpdate_state_code();

			if (SetUpdate2Change0_transition_code()) {
				state = Change;
			}
		}

		break;
	case Change:

		if (isTime2Run()) {
			Change_state_code();

			if (Change2SetUpdate0_transition_code()) {
				state = SetUpdate;
			}
		}

		break;
	default:
		cout << "[LedsControl::step()] Invalid state\n";
	}
}


void
LedsControl::setLed(int led, int color, bool value, bool automatic)
{
	isAutomatic=automatic;
	if(leds[led][color] != value)
	{
		leds[led][color] = value;
	    //led_idx[led] = color-1;
		led_idx[led] = color;
		if(led_idx[led] < 0) led_idx[led] = 0;

	//	if (((actual[led] == color) && !value) || ((actual[led] != color) && value)){
		if ((actual[led] != color)){
			isSetLed = true;
			//refreshLeds();
		}else isSetLed = false;
	}
}

void
LedsControl::refreshLeds()
{
	//printf("VAmos a refrescar los led\n");
	if(!active)
		return;

	bool set = false;

	for (int i=0; i < NUM_LEDS;i++){
		if (actual[i] != led_idx[i]){
			colorLed(i,led_idx[i]);
			if (isAutomatic){
				if (i==0) pleds->post.rasta(DEFAULT_DURATION);
				else if (i==1) pleds->post.earLedsSetAngle(270,DEFAULT_DURATION,true);
				else if(i==5) pleds->post.randomEyes(DEFAULT_DURATION);
			}
		}

	}
	isSetLed = false;
	/*for(int i = 0; i < NUM_LEDS; i++)
	{
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
	}
	isSetLed=false;*/
}

void
LedsControl::colorLed(int led, int color)
{
	printf("Entramos en Color Led con %d %d \n",led,color);
	//if(!active)
	//		return;
	if(actual[led] == color)
		return;

	actual[led] = color;

	if(active)
	{
		try {
			switch(color)
			{
			case COLOR_OFF:
				pleds->post.fadeRGB(lednames[led], NOCOLOR, DEFAULT_DURATION);
				break;
			case COLOR_RED:
				pleds->post.fadeRGB(lednames[led], RED, DEFAULT_DURATION);
				break;
			case COLOR_BLUE:
				pleds->post.fadeRGB(lednames[led], BLUE, DEFAULT_DURATION);
				break;
			case COLOR_GREEN:
				pleds->post.fadeRGB(lednames[led], GREEN, DEFAULT_DURATION);
				break;
			case COLOR_YELLOW:
				pleds->post.fadeRGB(lednames[led], YELLOW, DEFAULT_DURATION);
				break;
			case COLOR_WHITE:
				pleds->post.fadeRGB(lednames[led], -WHITE, DEFAULT_DURATION);
				break;
			case COLOR_ORANGE:
				pleds->post.fadeRGB(lednames[led], ORANGE, DEFAULT_DURATION);
				break;
			default:
				cerr<<"LedsControl::colorLed\tcolor not valid"<<endl;
			};
		}catch(AL::ALError& e)
		{
			cerr<<"[LedsControl::colorLed()] Error: "<<e.toString()<<endl;
		}
	}
}
//BUILDER COMMENT. DO NOT REMOVE. auxcode begin
//BUILDER COMMENT. DO NOT REMOVE. auxcode end


