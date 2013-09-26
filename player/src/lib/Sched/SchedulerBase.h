#ifndef SchedulerBase_H
#define SchedulerBase_H

#include <string>

class SchedulerBase
{
public:

	virtual void init() = 0;
	virtual void run(std::string component = "Default") = 0;
	virtual void stop(std::string component = "Default") = 0;
};

#endif
