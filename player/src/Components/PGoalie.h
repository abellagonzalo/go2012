#ifndef PGoalie_H
#define PGoalie_H

#include "Component.h"
#include "BallDetector.h"
#include "Attention.h"
#include "Body.h"
#include "Singleton.h"

//BUILDER COMMENT. DO NOT REMOVE. auxinclude begin
//BUILDER COMMENT. DO NOT REMOVE. auxinclude end

class PGoalie : public Component, public Singleton<PGoalie>
{
public:

	PGoalie();
	~PGoalie();

	void step();
private:

	static const int Initial	= 0;
	static const int Esperar	= 1;
	static const int Tirarse	= 2;
	static const int Levantarse	= 3;
	static const int Tirado	= 4;
	int state;

	BallDetector *_BallDetector;
	Attention *_Attention;
	Body *_Body;

	void Initial_state_code(void);
	void Esperar_state_code(void);
	void Tirarse_state_code(void);
	void Levantarse_state_code(void);
	void Tirado_state_code(void);
	bool Initial2Esperar0_transition_code(void);
	bool Esperar2Tirarse0_transition_code(void);
	bool Tirado2Levantarse0_transition_code(void);
	bool Tirarse2Tirado0_transition_code(void);
//BUILDER COMMENT. DO NOT REMOVE. auxcode begin
public:
	void restart();
//BUILDER COMMENT. DO NOT REMOVE. auxcode end
};

#endif

