#ifndef Body_H
#define Body_H

#include "Component.h"
#include "Fallen.h"
#include "Singleton.h"

//BUILDER COMMENT. DO NOT REMOVE. auxinclude begin

#include "almath/tools/almath.h"
#include "alproxies/almotionproxy.h"
#include "Common.h"

#include <iostream>
#include <fstream>
#include <pthread.h>

#include <IceE/IceE.h>
#include <motionManagerI.h>
using namespace AL;


typedef struct
{
	ALValue joints;
	int steps;
	int numjoints;
	float** angles;
	float** times;
	long time;
}tmov;
//BUILDER COMMENT. DO NOT REMOVE. auxinclude end

class Body : public Component, public Singleton<Body>, public BodyMotionManager
{
public:

	Body();
	~Body();

	void step();
private:

	static const int Initial	= 0;
	static const int Walking	= 1;
	static const int Stopped	= 2;
	static const int Moving	= 3;
	int state;

	Fallen *_Fallen;

	void Initial_state_code(void);
	void Walking_state_code(void);
	void Stopped_state_code(void);
	void Moving_state_code(void);
	bool Initial2Stopped0_transition_code(void);
	bool Stopped2Walking0_transition_code(void);
	bool Walking2Stopped0_transition_code(void);
	bool Moving2Stopped0_transition_code(void);
	bool Stopped2Moving0_transition_code(void);
//BUILDER COMMENT. DO NOT REMOVE. auxcode begin
public:

	void init(const string newName, AL::ALPtr<AL::ALBroker> parentBroker);

	void stop();
	void poseInit(float speed);

	void setVel(float v, float w, float l);
	void setVelV(float v);
	void setVelL(float l);
	void setVelW(float w);
	void setFreq(float freq);
	void setStiffness(float val);

	inline float getV() {return v;};
	inline float getW() {return w;};
	inline float getL() {return l;};
	void getGlobalMovement(float &x, float &y, float &theta);
	bool hasMoved(float lastx, float lasty, float lastt);
	void getRelativeMovement(float lastx, float lasty, float lastt, float &movx, float &movy, float &movt);
	void getAbsoluteMovement(float relx, float rely, float relt, float absx, float absy, float abst, float &movx, float &movy, float &movt);

	void selKick(int kicksel);
	void selKick(string kicksel);
	void doFixMove(string move);
	void doParamKick(int foot);

	void doKickL(int foot);
	void doKickL2(int foot);


	void setDebugMov(bool d);

	inline bool isStopped() {return ((state == Stopped) && (kick==NOKICK));};

	static const int NOKICK			= -1;
	static const int LFOOT			= 0;
	static const int RFOOT			= 1;
	static const int GBACK			= 2;
	static const int GFRONT			= 3;
	static const int POSEINIT		= 4;
	static const int FIXED			= 5;
	static const int SLFOOT			= 6;
	static const int SRFOOT			= 7;
	static const int LLFOOT			= 9;
	static const int LRFOOT			= 10;

	//TXT Kicks
	static const int TXTFWDRR		= 11; // Forward Dribble Right
	static const int TXTFWDRL		= 12; // Forward Dribble Left
	static const int TXTFWNR		= 13; // Forward Right
	static const int TXTFWNL		= 14; // Forward Left
	static const int TXTFWSTR		= 15; // Forward Strong Right
	static const int TXTFWSTL		= 16; // Forward Strong Left
	static const int TXTSIDE2RI		= 17; // Side In Mini To Right
	static const int TXTSIDE2LI		= 18; // Side In Mini To Left
	static const int TXTSIDE2RSI	= 19; // Strong Side In To Right
	static const int TXTSIDE2LSI	= 20; // Strong Side In To Left
	static const int TXTSIDE2RO		= 21; // Side Out To Right
	static const int TXTSIDE2LO		= 22; // Side Out To Left
	static const int TXTOB2R		= 23; // Oblique To Right
	static const int TXTOB2L		= 24; // Oblique To Left
	static const int TXTSTANDB		= 25; // Stand up
	static const int TXTSTANDF		= 26; // Stand up
	static const int TXTSUICIDE		= 27; // Suicide
	static const int TXTBWR 		= 28; // Backward Kick Right
	static const int TXTBWL			= 29; // Backward Kick Left

	// Movimientos de la fisioterapia del centro de dia
	static const int TXTFCAA		= 30; // Fisio Cabeza Arriba-Abajo
	static const int TXTFCID		= 31; // Fisio Cabeza Izquierda-Derecha
	static const int TXTFBPAA		= 32; // Fisio Brazos Paralelos Arriba-Abajo
	static const int TXTFBAAA		= 33; // Fisio Brazos Alternos Arriba-Abajo
	static const int TXTFBCAC		= 34; // Fisio Brazos Cruz Abrir-Cerrar
	static const int TXTFBTH		= 35; // Fisio Brazos Tocar Hombro
	static const int TXTFMPV		= 36; // Fisio Muneca Palma-Vuelta

	virtual void setVel(float v, float w, float l, const Ice::Current& c);
	virtual void poseInit(const Ice::Current& c);
	virtual void doMove(const string& kick, const Ice::Current& c);
	virtual bool isWalking(const Ice::Current& c);
	virtual bool isMoving(const Ice::Current& c);


private:

	int kick;
	string fixkick;
	long kicktime;
	long timeMove;

	void calcMovement();
	void initFixMoves();
	void loadMove(string);
	void Tokenize(const string&, ALValue&, const string& = " ");
	void TokenizeS(const string&, ALValue&, const string& = " ");

	//motions
	void forwardDribbleRight();
	void forwardDribbleLeft();
	void forwardKickRight();
	void forwardKickLeft();
	void forwardStrongRight();
	void forwardStrongLeft();

	void sideMiniToRightIn();
	void sideMiniToLeftIn();
	void sideStrongToRightIn();
	void sideStrongToLeftIn();
	void sideKickToRightOut();
	void sideKickToLeftOut();

	void obliqueKickToRight();
	void obliqueKickToLeft();

	void backwardKickRight();
	void backwardKickLeft();

	void StandUpFront();
	void StandUpBack();

	void suicide();	

	void fisioCabezaArribaAbajo();
	void fisioCabezaIzquierdaDerecha();
	void fisioBrazosParalelosArribaAbajo();
	void fisioBrazosAlternosArribaAbajo();
	void fisioBrazosCruzAbrirCerrar();
	void fisioBrazosTocarHombro();
	void fisioMunecaPalmaVuelta();

	AL::ALPtr<AL::ALMotionProxy> pmotion;

	static const int MIN_TIME_INTERVAL = 2000;

	float last_v, last_w, last_l, last_f;
	float v, w, l, freq;

	float fdebug;
	bool finished;

	int initmovv;
	int initmovw;
	int initmovl;
	float movx, movy, movtheta;
	long time_last_move;
	long time_init_move;

	vector<int> movet;

	static const int INIT = -1;
	static const int STOPPED = 0;
	static const int WALKING = 1;
	static const int MOVING = 2;

	static const float MIN_MOVEMENT;
	static const float MAX_MOVEMENT;
	static const float MIN_MOVEMENT_THETA;

	static const float ODOMETRY_FACTOR_X;
	static const float ODOMETRY_FACTOR_Y;
	static const float ODOMETRY_FACTOR_T;

	map<string, tmov> fixmovs;

	pthread_mutex_t mutex;	// Mutex for v, w, l.


//BUILDER COMMENT. DO NOT REMOVE. auxcode end
};

#endif

