#include "StrategyAttentionClientGoalQ.h"
#include "GoalDetector.h"

StrategyAttentionClientGoalQ::StrategyAttentionClientGoalQ()
{
	lastPostSeen = "none";
	ap_pLeft = new tapoint;
	ap_pRight = new tapoint;
	ap_pAux = new tapoint;
	seenL = false;
	seenR = false;
	toAux = false;
}

StrategyAttentionClientGoalQ::~StrategyAttentionClientGoalQ()
{
}

void
StrategyAttentionClientGoalQ::getAP(tapoint **ap){

	float aux,aux2;

	if((*ap_pointer)->salience > 1 || (*ap_pointer)->salience < 0|| (*ap_pointer)->x < 0.1){
		/*cerr<<"FAIL";
		cerr<<"["<<(*ap_pointer)->x<<","<<(*ap_pointer)->y<<","<<(*ap_pointer)->z<<"]S:";
		cerr<<((int)((*ap_pointer)->salience*1000))/1000.0;
		cerr<<endl;*/
		(*ap_pointer)->salience = 0.0;
		ap_pointer = search_aps.begin();
	}
	if((*ap_pointer)->salience > 0.1){
//		cerr<<"\tRETRY";
		*ap = (*ap_pointer);
	}
	else{
//		cerr<<"\tLOOK ";
		ap_pointer++;
		if(ap_pointer == search_aps.end()){
			ap_pointer = search_aps.begin();
		}

		for(it_search_aps=search_aps.begin();it_search_aps!=search_aps.end();it_search_aps++){
			aux = (*it_search_aps)->salience;
			aux2 = (*ap_pointer)->salience;
			if(aux > aux2 && (*it_search_aps)->x > 10){
				ap_pointer = it_search_aps;
			}
		}
		*ap = (*ap_pointer);
	}
//	cout<<"\tAPGOAL["<<(*ap_pointer)->x<<","<<(*ap_pointer)->y<<","<<(*ap_pointer)->z;
//	cout<<"]S:"<<((int)((*ap_pointer)->salience*1000))/1000.0;
}

void
StrategyAttentionClientGoalQ::atAP(){

}

void
StrategyAttentionClientGoalQ::initListAP(){

	search_aps.clear();

	addAP(3000.0, 0.0, 400.0);
	addAP(3000.0, 3000.0, 400.0);
	addAP(3000.0, -3000.0, 400.0);

	ap_pLeft->x = 0.0;
	ap_pLeft->y = 0.0;
	ap_pLeft->z = 0.0;
	ap_pLeft->ts = getCurrentTimeBis();
	ap_pLeft->its = getCurrentTimeBis();
	ap_pLeft->time = 0;
	ap_pLeft->ap_at_image = false;
	ap_pLeft->salience = 1.0;
	ap_pLeft->last_time = getCurrentTimeBis()/1000;
	ap_pLeft->dynamic = true;

	search_aps.push_back(ap_pLeft);

	ap_pRight->x = 0.0;
	ap_pRight->y = 0.0;
	ap_pRight->z = 0.0;
	ap_pRight->ts = getCurrentTimeBis();
	ap_pRight->its = getCurrentTimeBis();
	ap_pRight->time = 0;
	ap_pRight->ap_at_image = false;
	ap_pRight->salience = 1.0;
	ap_pRight->last_time = getCurrentTimeBis()/1000;
	ap_pRight->dynamic = true;

	search_aps.push_back(ap_pRight);

	ap_pAux->x = 0.0;
	ap_pAux->y = 0.0;
	ap_pAux->z = 0.0;
	ap_pAux->ts = getCurrentTimeBis();
	ap_pAux->its = getCurrentTimeBis();
	ap_pAux->time = 0;
	ap_pAux->ap_at_image = false;
	ap_pAux->salience = 0.0;
	ap_pAux->last_time = getCurrentTimeBis()/1000;
	ap_pAux->dynamic = true;

	search_aps.push_back(ap_pAux);

	it_search_aps = search_aps.begin();
	ap_pointer = search_aps.begin();

}
void
StrategyAttentionClientGoalQ::APnotValid(){

}

void
StrategyAttentionClientGoalQ::updateAP()
{
	ObjectState leftPost;
	ObjectState rightPost;
	float angle;
	Vector2<double> pos;
	float p0LeftG, p0RightG;
	float p1LeftG, p1RightG;
	float qP0, qP1;
	float posX, posY;

	GoalDetector *_GoalDetector = GoalDetector::getInstance();

	p0LeftG = _GoalDetector->goals->p0LeftEstimate.getQuality();
	p0RightG = _GoalDetector->goals->p0RightEstimate.getQuality();
	qP0 = (p0LeftG + p0RightG)/2.0;

	p1LeftG = _GoalDetector->goals->p1LeftEstimate.getQuality();
	p1RightG = _GoalDetector->goals->p1RightEstimate.getQuality();
	qP1 = (p1LeftG + p1RightG)/2.0;

	if(qP0 == qP1){
		if(_GoalDetector->goals->p0LeftEstimate.getPositionInRelativeCoordinates().x>0.0)
		{
			leftPost = _GoalDetector->goals->p0LeftEstimate;
			rightPost = _GoalDetector->goals->p0RightEstimate;
		}else
		{
			leftPost = _GoalDetector->goals->p1LeftEstimate;
			rightPost = _GoalDetector->goals->p1RightEstimate;
		}
	}else if(qP0 > qP1){
		leftPost = _GoalDetector->goals->p0LeftEstimate;
		rightPost = _GoalDetector->goals->p0RightEstimate;
	}else{
		leftPost = _GoalDetector->goals->p1LeftEstimate;
		rightPost = _GoalDetector->goals->p1RightEstimate;
	}

	pos = rightPost.getPositionInRelativeCoordinates();
	ap_pRight->x = refreshPosition(ap_pRight->x,(float)pos[0]);
	ap_pRight->y = refreshPosition(ap_pRight->x,(float)pos[1]);
	ap_pRight->z = 0.0;

	pos = leftPost.getPositionInRelativeCoordinates();
	ap_pLeft->x = refreshPosition(ap_pLeft->x,(float)pos[0]);
	ap_pLeft->y = refreshPosition(ap_pLeft->x,(float)pos[1]);
	ap_pLeft->z = 0.0;

	if(leftPost.getReliability()>=ObjectState::MOST_RELIABLE &&
			rightPost.getReliability()>=ObjectState::MOST_RELIABLE){

		if(rightPost.getQuality() < leftPost.getQuality()){
			ap_pRight->salience = 1.0;
			ap_pLeft->salience = 0.0;
		}else{
			ap_pRight->salience = 0.0;
			ap_pLeft->salience = 1.0;
		}

		seenL = true;
		seenR = true;
		lastPostSeen = "both";

		ap_pAux->salience = 0.0;

		for(it_search_aps=search_aps.begin();it_search_aps!=search_aps.end();it_search_aps++){
			if(!(*it_search_aps)->dynamic){
				(*it_search_aps)->salience = 0.0;
			}
		}
		//cerr<<"\tG2P";

	}else if(rightPost.getReliability()>=ObjectState::MOST_RELIABLE){

		if(!(toAux && (lastPostSeen == "right"))){
			if(!seenR){
				for(it_search_aps=search_aps.begin();it_search_aps!=search_aps.end();it_search_aps++){
					(*it_search_aps)->salience = 0.0;
				}
				ap_pRight->salience = 1.0;
				seenR = true;
			}else{
				angle = atan2(ap_pRight->x,ap_pRight->y);

				float modulo = sqrt(pow(ap_pRight->y,2)+pow(ap_pRight->x,2));
				float auxangle = asin(POST_DIST/modulo);

				posX = (float)(modulo * cos(auxangle) * sin(angle-auxangle));
				posY = (float)(modulo * cos(auxangle) * cos(angle-auxangle));

				ap_pLeft->x = refreshPosition(ap_pLeft->x,posX);
				ap_pLeft->y = refreshPosition(ap_pLeft->x,posY);
				ap_pLeft->z = 0.0;

				if(ap_pLeft->x < 10){
					ap_pLeft->salience = 0.0;
				}else{
					ap_pLeft->salience = 1.0;
				}

				posX = (float)(modulo * cos(auxangle) * sin(angle+auxangle));
				posY = (float)(modulo * cos(auxangle) * cos(angle+auxangle));

				ap_pAux->x = refreshPosition(ap_pAux->x,posX);
				ap_pAux->y = refreshPosition(ap_pAux->x,posY);
				ap_pAux->z = 0.0;

				if(ap_pAux->x < 10){
					ap_pAux->salience = 0.0;
				}else{
					ap_pAux->salience = 0.5;
				}

				for(it_search_aps=search_aps.begin();it_search_aps!=search_aps.end();it_search_aps++){
					if(!(*it_search_aps)->dynamic){
						(*it_search_aps)->salience = 0.0;
					}
				}
			}
		}

		lastPostSeen = "right";
		//cerr<<"\tGPD";

	}else if(leftPost.getReliability()>=ObjectState::MOST_RELIABLE){

		if(!(toAux && (lastPostSeen == "left"))){
			if(!seenL){
				for(it_search_aps=search_aps.begin();it_search_aps!=search_aps.end();it_search_aps++){
					(*it_search_aps)->salience = 0.0;
				}
				ap_pLeft->salience = 1.0;
				seenL = true;
			}else{
				angle = atan2(ap_pRight->x,ap_pRight->y);

				float modulo = sqrt(pow(ap_pRight->y,2)+pow(ap_pRight->x,2));
				float auxangle = asin(POST_DIST/modulo);

				posX = (float)(modulo * cos(auxangle) * sin(angle+auxangle));
				posY = (float)(modulo * cos(auxangle) * cos(angle+auxangle));

				ap_pRight->x = refreshPosition(ap_pRight->x,posX);
				ap_pRight->y = refreshPosition(ap_pRight->x,posY);
				ap_pRight->z = 0.0;

				if(ap_pRight->x < 10){
					ap_pRight->salience = 0.0;
				}else{
					ap_pRight->salience = 1.0;
				}

				posX = (float)(modulo * cos(auxangle) * sin(angle-auxangle));
				posY = (float)(modulo * cos(auxangle) * cos(angle-auxangle));

				ap_pAux->x = refreshPosition(ap_pAux->x,posX);
				ap_pAux->y = refreshPosition(ap_pAux->x,posY);
				ap_pAux->z = 0.0;

				if(ap_pAux->x < 10){
					ap_pAux->salience = 0.0;
				}else{
					ap_pAux->salience = 0.5;
				}

				for(it_search_aps=search_aps.begin();it_search_aps!=search_aps.end();it_search_aps++){
					if(!(*it_search_aps)->dynamic){
						(*it_search_aps)->salience = 0.0;
					}
				}
			}
		}
		lastPostSeen = "left";
		//cerr<<"\tGPI";
	}else{
		//cerr<<"\tNOT";
	}

	long now;
	float newS;

	for(it_search_aps = search_aps.begin(); it_search_aps != search_aps.end();++it_search_aps){
		now = getCurrentTimeBis()/1000;
		if((now - (*it_search_aps)->last_time) > 2.0 * Component::SHORT_RATE){
			(*it_search_aps)->last_time = now;
		}
		if(APReachedQ(*it_search_aps)){
			newS = (*it_search_aps)->salience - ((now - (*it_search_aps)->last_time)*(1.0/1500));
			if(newS > 0){
				(*it_search_aps)->salience = newS;
			}else{
				(*it_search_aps)->salience = 0.0;
			}

		}else{
			if((*ap_pointer)==(*it_search_aps)){
				newS = (*it_search_aps)->salience - ((now - (*it_search_aps)->last_time)*(1.0/2000));
				if(newS > 0){
					(*it_search_aps)->salience = newS;
				}else{
					(*it_search_aps)->salience = 0.0;
				}
			}else{
				newS = (*it_search_aps)->salience + ((now - (*it_search_aps)->last_time)*(1.0/2000));
				if(newS < 1){
					(*it_search_aps)->salience = newS;
				}else{
					(*it_search_aps)->salience = 1.0;
				}
			}
		}

		//cerr << "\t" << ((int)((*it_search_aps)->salience*1000))/1000.0;

		(*it_search_aps)->last_time = getCurrentTimeBis()/1000;
	}

	toAux = (((*ap_pointer)->x == ap_pAux->x) && ((*ap_pointer)->y == ap_pAux->y));

	if(ap_pRight->x < 10){
		ap_pRight->salience = 0.0;
	}
	if(ap_pLeft->x < 10){
		ap_pLeft->salience = 0.0;
	}

	if(rightPost.getReliability()<=ObjectState::MEDIUM_RELIABLE){
		seenR = false;
	}

	if(leftPost.getReliability()<=ObjectState::MEDIUM_RELIABLE){
		seenL = false;
	}

	/*cerr << "\tL" << ((int)(ap_pLeft->salience*1000))/1000.0;
	cerr << "\tR" << ((int)(ap_pRight->salience*1000))/1000.0;
	cerr << "\tA" << ((int)(ap_pAux->salience*1000))/1000.0;
	cerr << endl;*/
	it_search_aps = search_aps.begin();
}
