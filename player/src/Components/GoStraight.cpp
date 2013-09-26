
#include "GoStraight.h"
#include "Body.h"
GoStraight::GoStraight()
{
	vel = 0.0;
}

GoStraight::~GoStraight()
{
	try{
		pmotion = getParentBroker()->getMotionProxy();
		pmotion->post.clearFootsteps();
	}catch( ALError& e)
	{
		cerr<<"GoStraight::step\tUnable to end walk : "<<e.toString()<<endl;
	}
}

void
GoStraight::init(const string newName, AL::ALPtr<AL::ALBroker> parentBroker)
{
	Component::init(newName, parentBroker);

	try{
		pmotion = getParentBroker()->getMotionProxy();
	}catch( ALError& e)
	{
		cerr<<"GoStraight::init : "<<e.toString()<<endl;
	}

	last_vel = 0.0;
}

void
GoStraight::stop()
{
	pmotion->clearFootsteps();
}

void
GoStraight::step(void)
{
	//startDebugInfo();

	if (!isTime2Run())
		return;

	if(((last_vel * vel) < 0.0) || ((vel == 0.0) && (fabs(last_vel) > 0.0)))
	{
		try{
			pmotion->post.clearFootsteps();
		}catch( ALError& e)
		{
			cerr<<"Parando el planificador"<<endl;
	cerr<<"GoStraight::step\tUnable to end walk : "<<e.toString()<<endl;
		}
	}

	if(!pmotion->walkIsActive() && (fabs(vel) > 0.0))
	{
		try
		{
			if(vel < 0.0)
				pmotion->post.walkStraight(-10.0, Body::NSPS+10);//20);//90);
			else
				pmotion->post.walkStraight(10.0, Body::NSPS+10);//90);

		}catch( ALError& e)
		{
			cerr<<"GoStraight::step\tUnable to end walk: "<<e.toString()<<endl;
		}
	}

	last_vel = vel;

	//endDebugInfo();
}
