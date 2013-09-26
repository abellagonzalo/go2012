#ifndef Speech_H_
#define Speech_H_

#include "Component.h"
#include "Singleton.h"
#include "alproxies/altexttospeechproxy.h"

class Speech: public Singleton<Speech>
{
public:
	Speech();
	virtual ~Speech();

	void init(const string newName, AL::ALPtr<AL::ALBroker> parentBroker);

	void say(string msg);
private:

	AL::ALPtr<AL::ALTextToSpeechProxy> paudio;
};

#endif /* Speech_H_ */
