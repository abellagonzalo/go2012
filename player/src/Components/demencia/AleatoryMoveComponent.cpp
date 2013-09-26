#include "AleatoryMoveComponent.h"

AleatoryMoveComponent::AleatoryMoveComponent()
{
	body = Body::getInstance();
	srand(time(NULL));
	setConfFile("/home/nao/bica/conf/demencia/aleatoryMove.cfg");
}

AleatoryMoveComponent::~AleatoryMoveComponent()
{

}

void AleatoryMoveComponent::init(const string newName, AL::ALPtr<AL::ALBroker> parentBroker) {
	Component::init(newName, parentBroker);

	if (loadConfFile()) {
		cout << begin_blue << "[AleatoryMoveComponent] Loaded " << moves.size() << " moves" << end_color << endl;
	} else {
		cout << begin_red << "[AleatoryMoveComponent] Problem loading configuration file " << confFilePath << end_color << endl;
	} 
}

void 
AleatoryMoveComponent::step()
{
	//if (isTime2Run()) {
		body->step();
	//}
}

void 
AleatoryMoveComponent::execute()
{
	int randNum = rand() % moves.size();
	std::list<string>::iterator it = moves.begin();
	std::advance(it, randNum);
	body->selKick(*it);
	cerr << "Hacemos el movimiento " << *it << "con el numero " << randNum << endl; 
}

bool 
AleatoryMoveComponent::isAvailable(string move)
{
	std::list<string>::iterator it;
	it = std::find(moves.begin(), moves.end(), move);
	return it!=moves.end();
}

string
AleatoryMoveComponent::createKeyWithNumber(string key, int number)
{
	ostringstream keyStream;
	keyStream << "move"  << number;
	return keyStream.str();
}

bool 
AleatoryMoveComponent::loadConfFile()
{
	Dictionary dictionary;
	dictionary.createDictionary();

	moves.clear();

	if (!dictionary.loadFromFile(confFilePath.c_str())) {
		dictionary.freeDictionary();
		return false;
	}

	int i = 0;
	bool continueWhile = true;
	while (continueWhile) {

		// Create key
		string keyString = createKeyWithNumber("move", i++);
		char *key = new char[keyString.length()];
		strcpy(key, keyString.c_str());

		// Get value from key
		char *value;		
		if (continueWhile = dictionary.getValueString(key, &value)) {
			string move(value);
			moves.push_back(move);
		}

		// Clean memory
		delete []key;
	}

	dictionary.freeDictionary();

	return true;
}

bool 
AleatoryMoveComponent::saveConfFile()
{
	Dictionary dictionary;
	dictionary.createDictionary();

	int i = 0;
	for (list<string>::iterator move=moves.begin(); move!=moves.end(); move++) {

		// Create Key
		string key = createKeyWithNumber("move", i++);

		// Set key & value
		dictionary.setValue(key.c_str(), move->c_str());
	}

	bool returnValue = dictionary.saveToFile(confFilePath.c_str());
	dictionary.freeDictionary();

	return returnValue;
}

