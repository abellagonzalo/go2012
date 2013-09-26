#ifndef Debug_H
#define Debug_H

#include "Component.h"
#include "Singleton.h"
#include "CascadeScheduler.h"

class DebugIRel
{
public:
	virtual bica::ShapeList getGrDebugRel() = 0;
protected:
	bica::ShapeList	shapeListRel;
};

class DebugIAbs
{
public:
	virtual bica::ShapeList getGrDebugAbs() = 0;
protected:
	bica::ShapeList	shapeListAbs;
};

class DebugIImg
{
public:
	virtual bica::ShapeList getGrDebugImg() = 0;
protected:
	bica::ShapeList	shapeListImg;
};



class Debug : public Component, public Singleton<Debug>, public GrDebugProvider
{
public:

	Debug();
	~Debug();

	void init(const string newName, AL::ALPtr<AL::ALBroker> parentBroker);
	void step();
	virtual bica::ShapeList getGrDebugRel(const Ice::Current& c);
	virtual bica::ShapeList getGrDebugAbs(const Ice::Current& c);
	virtual bica::ShapeList getGrDebugImg(const Ice::Current& c);
	virtual void debugOn ( const string& component, const Ice::Current& c);
	virtual void debugOff ( const string& component, const Ice::Current& c);

private:
	CascadeScheduler *_sched;
	vector<Component*> debugComponents;
};

#endif
