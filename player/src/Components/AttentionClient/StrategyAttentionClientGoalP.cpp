#include "StrategyAttentionClientGoalP.h"
#include "GoalDetector.h"

StrategyAttentionClientGoalP::StrategyAttentionClientGoalP()
{

}

StrategyAttentionClientGoalP::~StrategyAttentionClientGoalP()
{
}

void
StrategyAttentionClientGoalP::getAP(tapoint **ap){

	GoalDetector *_GoalDetector = GoalDetector::getInstance();

	//static long ts = 0;
	ObjectState *pr, *pl;
	long ElapsedTimeSinceLastObs;
	long p0Time, p1Time;

	p0Time = _GoalDetector->goals->p0ElapsedTimeSinceLastObs;
	p1Time = _GoalDetector->goals->p1ElapsedTimeSinceLastObs;

	if(p1Time < 0.0 ) p1Time = ObjectState::NEVER;
	if(p0Time < 0.0 ) p0Time = ObjectState::NEVER;


	if(p0Time<p1Time)
	{
		pr = &_GoalDetector->goals->p0RightEstimate;
		pl = &_GoalDetector->goals->p0LeftEstimate;
		ElapsedTimeSinceLastObs = p0Time;
	}else
	{
		pr = &_GoalDetector->goals->p1RightEstimate;
		pl = &_GoalDetector->goals->p1LeftEstimate;
		ElapsedTimeSinceLastObs = p1Time;
	}

	//cerr<<"GoalDetector::getAP {"<<ElapsedTimeSinceLastObs<<"}"<<endl;

	//if(goals->blueP0Estimate->estimate.getReliability()>=SingleObjectState::MOST_RELIABLE)
	if((ElapsedTimeSinceLastObs<ObjectState::LONG_TIME) && (!forcesearch))
	{
		if(((ap_actual.time) > 2000000) || (ap_actual.x<0.0))
		{
			if(post == 0)
				post = 1;
			else
				post = 0;

			//ts = getCurrentTime();
			ap_actual.time = 0;
			ap_actual.its = getCurrentTimeBis();
			ap_actual.ts = getCurrentTimeBis();

			//cerr<<"GoalDetector::getAP\tskip: = ["<<skip<<"]"<<endl;
			skip++;

			if(skip>1)
			{
				skip = 0;
				nextelem = true;
			}
		}

		if(nextelem)
		{
			//cerr<<"GoalDetector::getAP NULL***************"<<endl;
			nextelem = false;
			*ap = NULL;
			return;
		}

		ap_actual.ts = getCurrentTimeBis();

		if(post == 0)
		{
			Vector2<double> pos = pl->getPositionInRelativeCoordinates();
			ap_actual.x = pos[0];
			ap_actual.y = pos[1];
			ap_actual.z = 0.0;

			//cerr<<"0: ["<<ap_actual.time<<"]\t"<<ap_actual.x<<","<<ap_actual.y<<endl;

		}else
		{
			Vector2<double> pos = pr->getPositionInRelativeCoordinates();
			ap_actual.x = pos[0];
			ap_actual.y = pos[1];
			ap_actual.z = 0.0;
			//cerr<<"1: ["<<ap_actual.time<<"]\t"<<ap_actual.x<<","<<ap_actual.y<<endl;
		}

		*ap = &ap_actual;
		//else
		//	*ap = NULL;

	}else
	{


		long inpos = getCurrentTimeBis()-(*it_search_aps)->ts;
		inpos = (*it_search_aps)->time;

		if(forcesearch || ((((*it_search_aps)->ap_at_image) && (inpos>1000000)) || (inpos>3000000)))
		{

			it_search_aps++;
			if(it_search_aps == search_aps.end())
				it_search_aps = search_aps.begin();

			(*it_search_aps)->time = 0;
			(*it_search_aps)->its = getCurrentTimeBis();
			(*it_search_aps)->ts = getCurrentTimeBis();
		}

		*ap = (*it_search_aps);
		forcesearch = false;
	}
}
void
StrategyAttentionClientGoalP::atAP(){

	//cerr<<"GoalDetector::atAP\ttime = ["<<ap_actual.time<<"]"<<endl;
	if((ap_actual.time) > 1000000)
	{
		if(post == 0)
			post = 1;
		else
			post = 0;

		//ts = getCurrentTime();
		ap_actual.time = 0;
		ap_actual.its = getCurrentTimeBis();
		ap_actual.ts = getCurrentTimeBis();

		//cerr<<"GoalDetector::atAP\tskip: = ["<<skip<<"]"<<endl;

		skip++;

		if(skip>1)
		{
			skip = 0;
			nextelem = true;
		}
	}

}

void
StrategyAttentionClientGoalP::initListAP(){

//	cout<<"initListaAP GoalRR:"<<endl;
	search_aps.clear();

	addAP(3000.0, 0.0, 400.0);
	addAP(1000.0, 8000.0, 400.0);
	addAP(1000.0, -8000.0, 400.0);

	it_search_aps = search_aps.begin();

	post = 0;
	nextelem = false;
	skip = 0;
	forcesearch = false;
}
void
StrategyAttentionClientGoalP::APnotValid(){
	forcesearch = true;

}
void
StrategyAttentionClientGoalP::updateAP()
{

}
