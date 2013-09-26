#include "Debug.h"

Debug::Debug()
{
	_sched = CascadeScheduler::getInstance();
}

Debug::~Debug()
{

}

void
Debug::init(const string newName, AL::ALPtr<AL::ALBroker> parentBroker)
{
	Component::init(newName, parentBroker);

	setFreqTime(3000);
}

bica::ShapePtr
getTestShape()
{
	bica::Point3DPtr p1(new bica::Point3D);
	bica::Point3DPtr p2(new bica::Point3D);

	p1->x = 0.0f;
	p1->y = 0.0f;
	p1->z = 0.0f;
	p2->x = 100.0f;
	p2->y = 0.0f;
	p2->z = 0.0f;

	bica::LinePtr line(new bica::Line);
	line->p1 = p1;
	line->p2 = p2;
	line->width = 1;

	line->color = bica::BLACK;
	line->filled = false;
	line->opacity = 255;
	line->accKey = "b";

	return line;
}

bica::ShapeList
Debug::getGrDebugRel(const Ice::Current& c)
{
	bica::ShapeList shapeListRel;

	// Add a dummy line first, if the list is empty Ice crashes. (Ice bug confirmed)
	shapeListRel.push_back(getTestShape());

	vector<Component*>::iterator it;
	for (it = debugComponents.begin(); it != debugComponents.end(); it++) {
		DebugIRel* debugableComp = dynamic_cast<DebugIRel*>(*it);
		// Check if the component is debugable
		if ( debugableComp != 0) {
			//Get the component's shape list
			ShapeList auxList = debugableComp->getGrDebugRel();
			//Insert the component's shape list into my shape list
			shapeListRel.insert(shapeListRel.end(), auxList.begin(), auxList.end());
		}
	}

	return shapeListRel;
}

bica::ShapeList
Debug::getGrDebugAbs(const Ice::Current& c)
{
	bica::ShapeList shapeListAbs;

	// Add a dummy line first, if the list is empty Ice crashes. (Ice bug confirmed)
	shapeListAbs.push_back(getTestShape());

	vector<Component*>::iterator it;
	for (it = debugComponents.begin(); it != debugComponents.end(); it++) {
		DebugIAbs* debugableComp = dynamic_cast<DebugIAbs*>(*it);
		// Check if the component is debugable
		if ( debugableComp != 0) {
			//Get the component's shape list
			ShapeList auxList = debugableComp->getGrDebugAbs();
			//Insert the component's shape list into my shape list
			shapeListAbs.insert(shapeListAbs.end(), auxList.begin(), auxList.end());
		}
	}

	return shapeListAbs;
}

bica::ShapeList
Debug::getGrDebugImg(const Ice::Current& c)
{
	bica::ShapeList shapeListImg;

	vector<Component*>::iterator it;
	for (it = debugComponents.begin(); it != debugComponents.end(); it++) {
		DebugIImg* debugableComp = dynamic_cast<DebugIImg*>(*it);
		// Check if the component is debugable
		if ( debugableComp != 0) {
			//Get the component's shape list
			ShapeList auxList = debugableComp->getGrDebugImg();
			//Insert the component's shape list into my shape list
			shapeListImg.insert(shapeListImg.end(), auxList.begin(), auxList.end());
		}
	}

	return shapeListImg;
}

void
Debug::debugOn( const string& component, const Ice::Current& c)
{
	Component *comp = Register::getInstance()->getComponent(component);

	if (comp != NULL) {
		bool exist = false;
		vector<Component*>::iterator it;
		for (it = debugComponents.begin(); it != debugComponents.end(); it++)
			if ((*it) == comp)
				exist = true;

		if (!exist) {
			debugComponents.push_back(comp);
		}
	}
}

void
Debug::debugOff( const string& component, const Ice::Current& c)
{
	Component *comp = Register::getInstance()->getComponent(component);

	if (comp != NULL) {
		vector<Component*>::iterator it;
		for (it = debugComponents.begin(); it != debugComponents.end(); it++) {
			if ((*it) == comp) {
				debugComponents.erase(it);
				break;
			}
		}
	}
}

void
Debug::step()
{
	if (isTime2Run()) {

	}
}
