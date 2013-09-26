#ifndef GOSTRAIGHT_H
#define GOSTRAIGHT_H

#include "Component.h"
#include "alptr.h"
#include "almotionproxy.h"
#include "Singleton.h"



class GoStraight : public Component, public Singleton<GoStraight>
{
public:

	GoStraight();
	~GoStraight();

	void init(const string newName, AL::ALPtr<AL::ALBroker> parentBroker);
	void step();

	void stop();
	void setVel(float vel){ this->vel = vel;};


private:

	AL::ALPtr<AL::ALMotionProxy> pmotion;
	float vel, last_vel;
};

#endif
