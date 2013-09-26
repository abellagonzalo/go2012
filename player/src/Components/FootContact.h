#ifndef FootContact_H
#define FootContact_H

#include "Component.h"
#include "Singleton.h"

//BUILDER COMMENT. DO NOT REMOVE. auxinclude begin
#include "alproxies/almemoryproxy.h"
//BUILDER COMMENT. DO NOT REMOVE. auxinclude end

class FootContact : public Component, public Singleton<FootContact>
{
public:

	FootContact();
	~FootContact();

	void init(const string newName, AL::ALPtr<AL::ALBroker> parentBroker,string moduleName);


	void dataChanged(const std::string& pDataName, const ALValue& pValue, const std::string& pMessage);

	void step();

	void resetPress();
	bool isPressFootLeft();
	bool isPressFootRight();
	bool isPressChest();

	/*Controla si ha sido pulsado o no*/
		bool IsFootLeft;
		bool IsFootRight;
		bool IsChest;
private:

	static const int Initial	= 0;
	static const int Contact	= 1;
	static const int NoContact	= 2;
	int state;

	AL::ALPtr<AL::ALMemoryProxy> pmemory;




	void Initial_state_code(void);
	void Contact_state_code(void);
	void NoContact_state_code(void);
	bool Initial2NoContact0_transition_code(void);
	bool NoContact2Contact0_transition_code(void);
	bool Contact2NoContact0_transition_code(void);
//BUILDER COMMENT. DO NOT REMOVE. auxcode begin
//BUILDER COMMENT. DO NOT REMOVE. auxcode end
};

#endif

