#include "FootContact.h"

FootContact::FootContact()
{

	state = Initial;
}

FootContact::~FootContact()
{

}

void FootContact::init(const string newName, AL::ALPtr<AL::ALBroker> parentBroker,string moduleName) {
		Component::init(newName, parentBroker);
	/*try {

		pmemory = getParentBroker()->getMemoryProxy();
		pmemory->subscribeToEvent("RightBumperPressed", moduleName, "dataChanged");
		pmemory->subscribeToEvent("LeftBumperPressed", moduleName, "dataChanged");
		pmemory->subscribeToEvent("ChestButtonPressed", moduleName, "dataChanged");
		resetPress();
	} catch (ALError& e) {
		cerr << "[FootContact::init()] " << e.toString() << endl;
	}*/
}

void FootContact::dataChanged(const std::string& pDataName, const ALValue& pValue,
		const std::string& pMessage) {

	//cerr<<"FootContact::dataChanged"<<endl;
	 if (pDataName.compare("RightBumperPressed")==0)
	 {

		 IsFootRight=true;
	 }
	 if (pDataName.compare("LeftBumperPressed")==0)
 	 {
		 IsFootLeft = true;

 	 }
	 if (pDataName.compare("ChestButtonPressed")==0)
	 {
		 IsChest = true;

	 }
}

bool FootContact::isPressFootLeft(){
	return IsFootLeft;
}
bool FootContact::isPressFootRight(){
	return IsFootRight;
}
bool FootContact::isPressChest(){
	return IsChest;
}

void FootContact::resetPress(){

	IsFootLeft = false;
	IsFootRight = false;
	IsChest = false;

}


void FootContact::Initial_state_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Initial_state_code

//BUILDER COMMENT. DO NOT REMOVE. end Initial_state_code
}

void FootContact::Contact_state_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Contact_state_code
//BUILDER COMMENT. DO NOT REMOVE. end Contact_state_code
}

void FootContact::NoContact_state_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin NoContact_state_code
	IsFootRight = false;
	IsFootLeft = false;
	IsChest = false;
//BUILDER COMMENT. DO NOT REMOVE. end NoContact_state_code
}

bool FootContact::Initial2NoContact0_transition_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Initial2NoContact0_transition_code
return true;
//BUILDER COMMENT. DO NOT REMOVE. end Initial2NoContact0_transition_code
}

bool FootContact::NoContact2Contact0_transition_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin NoContact2Contact0_transition_code
	if (IsFootRight or IsFootLeft or IsChest) return true;
	else return false;
	//BUILDER COMMENT. DO NOT REMOVE. end NoContact2Contact0_transition_code
}

bool FootContact::Contact2NoContact0_transition_code(void)
{
//BUILDER COMMENT. DO NOT REMOVE. begin Contact2NoContact0_transition_code
return true;
//BUILDER COMMENT. DO NOT REMOVE. end Contact2NoContact0_transition_code
}

void
FootContact::step(void)
{
	switch (state)
	{
	case Initial:

		if (isTime2Run()) {
			Initial_state_code();

			if (Initial2NoContact0_transition_code()) {
				state = NoContact;
			}
		}

		break;
	case Contact:

		if (isTime2Run()) {
			Contact_state_code();

			if (Contact2NoContact0_transition_code()) {
				state = NoContact;
			}
		}

		break;
	case NoContact:

		if (isTime2Run()) {
			NoContact_state_code();

			if (NoContact2Contact0_transition_code()) {
				state = Contact;
			}
		}

		break;
	default:
		cout << "[FootContact::step()] Invalid state\n";
	}
}
//BUILDER COMMENT. DO NOT REMOVE. auxcode begin
//BUILDER COMMENT. DO NOT REMOVE. auxcode end


