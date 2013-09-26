#ifndef RobotLog_H
#define RobotLog_H

#include "Component.h"
#include "Singleton.h"
#include "CascadeScheduler.h"
#include "../../lib/log/ComponentSnapshot.h"
#include <logI.h>

class RobotLog : public Component, public Singleton<RobotLog>, public LogProvider
{
public:

	RobotLog();
	~RobotLog();

	void init(const string newName, AL::ALPtr<AL::ALBroker> parentBroker);
	void step();
	void updateSnapshot( vector<char> *newSnapshotData);
	long getLogTime();	//uSecs

	virtual void startLog( const string& filename, const string& comment, const Ice::Current& c );
	virtual void stopLog( const Ice::Current& c );
	virtual void logOn ( const string& component, const Ice::Current& c );
	virtual void logOff ( const string& component, const Ice::Current& c );

	template <class T> void readFromLogFile (ifstream *file, T *value) {
		file->read( reinterpret_cast<char *>(value), sizeof(*value));
	}

	void writeLogFile();
	void writeHeaderLogFile();
	pthread_mutex_t 	mutex;

private:

	static const int MAX_DATA_BATCH = 50000000; // 50MB

	typedef struct
	{
		string label;
		ComponentSnapshot::LogDataType type;
		short dataLength;
		size_t numberOfValues;
	}tLogSubSubHeader;

	typedef struct
	{
		string compName;
		vector<tLogSubSubHeader> labels;
	}tLogSubHeader;

	typedef struct
	{
		string comment;
		vector<tLogSubHeader> subHeaders;
	}tLogHeader;

	CascadeScheduler 	*_sched;
	vector<Component*> 	logComponents;
	int 				headerIdCounter;
	long				_logStartTime;
	vector<char>		_data;
	string				_filename;
	string				_comment;
	ofstream 			_vectorFile;
	bool 				_isHeaderWritten;

	// Just for testing
	void testLogFile( const string& filename );
	ifstream			_file;

	bool				isLogActivated;
	tLogHeader			_logH;


};

class LoggableI
{

	friend void RobotLog::startLog( const string& filename, const string& comment, const Ice::Current& c );
	friend void RobotLog::stopLog( const Ice::Current& c );
	friend void RobotLog::logOn ( const string& component, const Ice::Current& c );
	friend void RobotLog::writeLogFile();
	friend void RobotLog::writeHeaderLogFile();

protected:
	LoggableI();
	void beginLog();
	template <class T> void addToLog (const string& label, const ComponentSnapshot::LogDataType& type,
			const T& value) {
		if (_isTime2Log) {
			if ( !_isHeaderCompleted ) {
				_labels.push_back( label );
				_types.push_back( type );
				_sizes.push_back( sizeof (value) );
				_numberOfValues.push_back( 1 );
			}

			_componentSnapshot->add( value, _componentSnapshot->getBody() );
		}
	};

	template <class T> void addToLog (const string& label, const ComponentSnapshot::LogDataType& type,
			const vector<T>& value) {
		if (_isTime2Log) {
			if ( !_isHeaderCompleted ) {
				_labels.push_back( label );
				_types.push_back( type );
				_sizes.push_back( sizeof (value) );
				_numberOfValues.push_back( value.size() );
			}

			_componentSnapshot->add( value, _componentSnapshot->getBody() );
		}
	};

	void endLog(const string& name);

private:
	void startLog();
	void stopLog();
	vector<char> getHeader();
	void setHeaderId(int newHeaderId);

	bool 									_isTime2Log;
	RobotLog 								*_robotLog;
	ComponentSnapshot						*_componentSnapshot;
	vector<string>							_labels;
	vector<ComponentSnapshot::LogDataType> 	_types;
	vector<short>							_sizes;
	vector<size_t>		 					_numberOfValues;
	bool									_isHeaderCompleted;
};

#endif
