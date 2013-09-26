#include "StrategyAttentionClient.h"
#include "Kinematics.h"
#include "Horizon.h"
#include "../lib/vision/VisionDefinitions.h"

StrategyAttentionClient::StrategyAttentionClient()
{

}

StrategyAttentionClient::~StrategyAttentionClient()
{
}
void
StrategyAttentionClient::addAP(float px, float py, float pz)
{
	tapoint *newap;
	newap = new tapoint;

	newap->x = px;
	newap->y = py;
	newap->z = pz;
	newap->ts = getCurrentTimeBis();
	newap->its = getCurrentTimeBis();
	newap->time = 0;
	newap->ap_at_image = false;

	newap->salience = 1.0;
	newap->last_time = getCurrentTimeBis()/1000;
	newap->dynamic = false;

	search_aps.push_back(newap);
//	cout<<"APcreado("<<px<<","<<py<<","<<pz<<")"<<endl;
}

bool
StrategyAttentionClient::APReachedQ(tapoint *tap)
{

	HPoint3D p1, p2, p3, p4;
	HPoint2D p1i, p2i, p3i, p4i;
	Point2D aux1, aux2;
	float hor;

	Kinematics *_Kinematics = Kinematics::getInstance();
	Horizon *_Horizon = Horizon::getInstance();
	hor = (float)_Horizon->getHorizonRow();

	aux1.x = (float)ImageInput::IMG_WIDTH * 0.25f;
	aux1.y = (float)ImageInput::IMG_HEIGHT * 0.25f;
	aux1.h = 1.0f;
	aux2.x = (float)ImageInput::IMG_WIDTH * 0.75f;
	aux2.y = (float)ImageInput::IMG_HEIGHT * 0.25f;
	aux2.h = 1.0f;

	if(aux1.y < hor && hor < 200){
		aux1.y = hor-10;
	}
	if(aux2.y < hor && hor < 200){
		aux2.y = hor-10;
	}

	p1i.x = aux1.x;
	p1i.y = aux1.y;
	p1i.h = aux1.h;

	p2i.x = aux2.x;
	p2i.y = aux2.y;
	p2i.h = aux2.h;

	p3i.x = (float)ImageInput::IMG_WIDTH * 0.25f;
	p3i.y = (float)ImageInput::IMG_HEIGHT;
	p3i.h = 1.0f;

	p4i.x = (float)ImageInput::IMG_WIDTH * 0.75f;
	p4i.y = (float)ImageInput::IMG_HEIGHT;
	p4i.h = 1.0f;

	_Kinematics->get3DPositionZ(p1, p1i, 0.0);
	_Kinematics->get3DPositionZ(p2, p2i, 0.0);
	_Kinematics->get3DPositionZ(p3, p3i, 0.0);
	_Kinematics->get3DPositionZ(p4, p4i, 0.0);
/*
	cerr<<"\thorizonte: "<<hor;
	cerr<<"\tP1["<<p1i.x<<","<<p1i.y<<"]["<<p1.X<<","<<p1.Y<<","<<p1.Z<<"]";
	cerr<<" P2["<<p2i.x<<","<<p2i.y<<"]["<<p2.X<<","<<p2.Y<<","<<p2.Z<<"]";
	cerr<<" P3["<<p3i.x<<","<<p3i.y<<"]["<<p3.X<<","<<p3.Y<<","<<p3.Z<<"]";
	cerr<<" P4["<<p4i.x<<","<<p4i.y<<"]["<<p4.X<<","<<p4.Y<<","<<p4.Z<<"]";
	cerr<< endl;
*/
	float px[4], py[4];

	px[0] = p1.X / p1.H;
	px[1] = p2.X / p2.H;
	px[2] = p3.X / p3.H;
	px[3] = p4.X / p4.H;
	py[0] = p1.Y / p1.H;
	py[1] = p2.Y / p2.H;
	py[2] = p3.Y / p3.H;
	py[3] = p4.Y / p4.H;

	return (_Kinematics->pointInPolygon(4, px, py, tap->x, tap->y));

}

int
StrategyAttentionClient::refreshPosition(float posA, float posB){
	if(abs(posA-posB)>50){
		return posB;
	}else{
		return posA;
	}
}
