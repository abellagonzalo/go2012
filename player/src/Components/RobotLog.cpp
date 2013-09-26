#include "RobotLog.h"

//ToDo: Get the real component name
LoggableI::LoggableI()
{
	_isTime2Log = false;
	_robotLog = RobotLog::getInstance();
	_componentSnapshot = new ComponentSnapshot();
	_isHeaderCompleted = false;
}

void
LoggableI::startLog()
{
	_isTime2Log = true;
	_isHeaderCompleted = false;
}

void
LoggableI::stopLog()
{
	_isTime2Log = false;
}

void
LoggableI::beginLog()
{
	pthread_mutex_lock(&(_robotLog->mutex));
	if (_isTime2Log) {
		_componentSnapshot->add( _robotLog->getLogTime(), _componentSnapshot->getBody() );
		//cerr << "beginLog() HeaderId (" << _componentSnapshot->getHeaderId() << ")\n";
		_componentSnapshot->add( _componentSnapshot->getHeaderId(), _componentSnapshot->getBody() );
	}
}

void
LoggableI::endLog(const string& name)
{
	if (_isTime2Log) {
		if ( !_isHeaderCompleted ) {
			_componentSnapshot->composeHeader( name, _labels, _types, _sizes, _numberOfValues);
			_labels.clear();
			_types.clear();
			_sizes.clear();
			_numberOfValues.clear();
			_isHeaderCompleted = true;
		}

		_robotLog->updateSnapshot( _componentSnapshot->getBody() );
		_componentSnapshot->reset();
	}
	pthread_mutex_unlock(&(_robotLog->mutex));
}

void
LoggableI::setHeaderId(int newHeaderId)
{
	_componentSnapshot->setHeaderId( newHeaderId );
}

RobotLog::RobotLog()
{
	_sched = CascadeScheduler::getInstance();
	headerIdCounter = 0;
	isLogActivated = false;
	_isHeaderWritten = false;
	pthread_mutex_init(&mutex, NULL);
}

RobotLog::~RobotLog()
{

}

void
RobotLog::init(const string newName, AL::ALPtr<AL::ALBroker> parentBroker)
{
	Component::init(newName, parentBroker);
	setFreqTime(1000);
}

void
RobotLog::startLog( const string& filename, const string& comment, const Ice::Current& c )
{
	int counter = 0;

	pthread_mutex_lock(&mutex);

	isLogActivated = true;
	_data.clear();

	vector<Component*>::iterator it;
	for (it = logComponents.begin(); it != logComponents.end(); it++) {
		LoggableI* loggableComp = dynamic_cast<LoggableI*>(*it);
		// Check if the component is loggable
		if ( loggableComp != 0) {
			// Activate the log in the component
			loggableComp->setHeaderId( counter );
			counter++;
			loggableComp->startLog();
		}
	}

	_filename = filename;
	_comment = comment;

	// Get the current time
	_logStartTime = getCurrentTime();

	_vectorFile.open( _filename.c_str(), ios::out | ios::binary);

	cout << begin_yellow << "Log started (" << _filename.c_str() << ")...\n" << end_color;

	pthread_mutex_unlock(&mutex);
}

void
RobotLog::stopLog( const Ice::Current& c )
{
	pthread_mutex_lock(&mutex);

	if ( (!_data.empty()) && (isLogActivated) ) {
		if (!_isHeaderWritten) {
			cerr << "writeHeaderLogFile()";
			writeHeaderLogFile();
		}
		writeLogFile();
		cerr << "writeLogFile()\n";
		_vectorFile.close();
	}

	isLogActivated = false;
	_isHeaderWritten = false;


	//cerr << "antes de parar todos los log de los componentes\n";
	vector<Component*>::iterator it;
	for (it = logComponents.begin(); it != logComponents.end(); it++) {
		LoggableI* loggableComp = dynamic_cast<LoggableI*>(*it);
		// Check if the component is loggable
		if ( loggableComp != 0) {
			// Activate the log in the component
			loggableComp->stopLog();
		}
	}
	//cerr << "depues de parar todos los log de los componentes\n";


	// Copy to text the binary log file for checking all the fields
	//testLogFile( _filename );

	cout << begin_yellow << "Log finished...\n" << end_color;
	pthread_mutex_unlock(&mutex);
}

void
RobotLog::logOn ( const string& component, const Ice::Current& c )
{
	Component *comp = Register::getInstance()->getComponent(component);

	if (comp != NULL) {
		bool exist = false;
		vector<Component*>::iterator it;
		for (it = logComponents.begin(); it != logComponents.end(); it++)
			if ((*it) == comp) {
				exist = true;
				break;
			}

		if (!exist) {
			LoggableI* loggableComp = dynamic_cast<LoggableI*>(comp);
			// Check if the component is loggable
			if ( loggableComp != 0) {
				logComponents.push_back(comp);
				//loggableComp->setHeaderId( headerIdCounter );
				//cerr << "Componente de log añadido (" << headerIdCounter << ")\n";
				headerIdCounter++;
			}
		}
	}
}

void
RobotLog::logOff ( const string& component, const Ice::Current& c )
{
	Component *comp = Register::getInstance()->getComponent(component);

	if (comp != NULL) {
		vector<Component*>::iterator it;
		for (it = logComponents.begin(); it != logComponents.end(); it++) {
			if ((*it) == comp) {
				logComponents.erase(it);
				break;
			}
		}
	}
}

void
RobotLog::writeHeaderLogFile()
{
	vector<char> myHeader;
	int counter = 0;

	// Add the log comment length
	size_t commentLength = _comment.size();
	char bytes1[sizeof(commentLength)];
	std::copy(static_cast<const char*>(static_cast<const void*>(&commentLength)),
			static_cast<const char*>(static_cast<const void*>(&commentLength)) + sizeof commentLength,
			bytes1);
	for (unsigned int j = 0; j < sizeof(bytes1); j++)
		myHeader.push_back(bytes1[j]);

	// Add the log comment
	for (unsigned i = 0; i < _comment.size(); i++) {
		char bytes[sizeof(char)];
		std::copy(static_cast<const char*>(static_cast<const void*>(&_comment[i])),
				static_cast<const char*>(static_cast<const void*>(&_comment[i])) + 1,
				bytes);
		for (unsigned int j = 0; j < sizeof(bytes); j++)
			myHeader.push_back(bytes[j]);
	}

	//Iterator to the beginning of subHeaders
	vector<char> headers;

	// Get all the headers
	vector<Component*>::iterator it;
	for (it = logComponents.begin(); it != logComponents.end(); it++) {
		LoggableI* loggableComp = dynamic_cast<LoggableI*>(*it);
		// Check if the component is loggable
		if ( loggableComp != 0) {
			// Insert at the beginning the log header of the component
			headers.insert( headers.end(), loggableComp->_componentSnapshot->getHeader()->begin(),
					loggableComp->_componentSnapshot->getHeader()->end() );
			counter++;
		}
	}

	// Add number of headers
	char bytes2[sizeof counter];
	std::copy(static_cast<const char*>(static_cast<const void*>(&counter)),
			static_cast<const char*>(static_cast<const void*>(&counter)) + sizeof counter,
			bytes2);
	for (unsigned int j = 0; j < sizeof(bytes2); j++)
		myHeader.push_back(bytes2[j]);

	myHeader.insert( myHeader.end(), headers.begin(), headers.end());

	_vectorFile.write((char*) &myHeader[0], myHeader.size());

	cout << begin_yellow << "Header written in log...\n" << end_color;

	myHeader.clear();
	headers.clear();
}

void
RobotLog::writeLogFile()
{
	_vectorFile.write((char*) &_data[0], _data.size());
	cout << begin_yellow << "Data written in log...\n" << end_color;

	_data.clear();
}


void
RobotLog::step()
{
	if ( (isTime2Run()) && (isLogActivated) ) {
		if ( _data.size() > MAX_DATA_BATCH ) {

			if (!_isHeaderWritten) {
				writeHeaderLogFile();
				_isHeaderWritten = true;
			}
			writeLogFile();
			cerr << "RobotLog::step()\n";
		}
	}
}

void
RobotLog::updateSnapshot( vector<char> *newSnapshotData)
{
	_data.insert( _data.end(), newSnapshotData->begin(), newSnapshotData->end() );
}

long
RobotLog::getLogTime()
{
	return getCurrentTime() - _logStartTime; //uSecs
}

void
RobotLog::testLogFile( const string& filename )
{
	// Header
	size_t commentLength;
	char c;

	// Comment
	string comment;

	int numHeaders;

	string newFilename = filename + ".txt";
	ofstream ofs(newFilename.c_str());

	_file.open( filename.c_str(), ios::in | ios::binary );

	cerr << "Nombre de fichero: (" << filename << ")\n";

	// CommentLength
	_file.read( reinterpret_cast<char *>(&commentLength), sizeof(size_t));
	ofs << commentLength << " ";

	cerr << "Longitud comentario: (" << commentLength << ")\n";

	// Comment
	for (unsigned i = 0; i < commentLength; i++) {
		_file.read( reinterpret_cast<char *>(&c), sizeof(char));
		comment += c;
	}
	_logH.comment = comment;
	ofs << comment << " ";

	cerr << "Comentario: (" << comment << ")\n";

	// NumHeaders
	_file.read( reinterpret_cast<char *>(&numHeaders), sizeof(int));
	ofs << numHeaders << " ";

	cerr << "Numero de cabeceras: (" << numHeaders << ")\n";

	cerr << "Leyendo log de " << filename << ": |" << commentLength << "|" << comment << "|" << numHeaders << "|";

	_logH.subHeaders.resize( numHeaders );
	cerr << "Longitud vector de cabeceras: (" << _logH.subHeaders.size() << ")\n";

	// Headers
	for (int i = 0; i < numHeaders; i++ ) {

		cerr << "========\n == Componente (" << i << ") ==\n ========\n";

		//tLogSubHeader subHeader;

		// HeaderId
		int headerId;
		_file.read( reinterpret_cast<char *>(&headerId), sizeof(int));
		ofs << headerId << " ";

		cerr << "Header id: (" << headerId << ")\n";

		// ComponentNameLength
		size_t length;
		_file.read( reinterpret_cast<char *>(&length), sizeof(size_t));
		ofs << length << " ";

		cerr << "ComponentNameLength: (" << length << ")\n";

		// ComponentName
		string componentName;
		for (unsigned j = 0; j < length; j++) {
			_file.read( reinterpret_cast<char *>(&c), sizeof(char));
			componentName += c;
		}
		//subHeader.compName = componentName;
		_logH.subHeaders.at(i).compName = componentName;
		ofs << componentName << " ";

		cerr << "componentName: (" << componentName << ")\n";

		// NumLabels
		int numLabs;
		_file.read( reinterpret_cast<char *>(&numLabs), sizeof(int));
		ofs << numLabs << " ";

		cerr << "Num labels: (" << numLabs << ")\n";

		cout << headerId << "|" << length << "|" << componentName << "|" << numLabs << "|";

		// Resize subheader vector
		_logH.subHeaders.at(i).labels.resize( numLabs );

		for (int nlabels = 0; nlabels < numLabs; nlabels++ ) {

			cerr << "\t========\n\t == Etiqueta (" << nlabels << ") ==\n\t ========\n";

			//tLogSubSubHeader ssHeader;

			// LabelLength
			size_t labelLength;
			_file.read( reinterpret_cast<char *>(&labelLength), sizeof(size_t));
			ofs << labelLength << " ";

			cerr << "LabelLength: (" << labelLength << ")\n";

			// Label
			string label;
			for (unsigned k = 0; k < labelLength; k++) {
				_file.read( reinterpret_cast<char *>(&c), sizeof(char));
				label += c;
			}
			//ssHeader.label = label;
			_logH.subHeaders.at(i).labels.at(nlabels).label = label;


			ofs << label << " ";

			cerr << "Label: (" << label << ")\n";

			// DataType
			ComponentSnapshot::LogDataType dataType;
			_file.read( reinterpret_cast<char *>(&dataType), sizeof(dataType));
			//ssHeader.type = dataType;
			_logH.subHeaders.at(i).labels.at(nlabels).type = dataType;

			ofs << dataType << " ";

			cerr << "DataType: (" << dataType << ")\n";

			// DataValueLength
			short dataSize;
			_file.read( reinterpret_cast<char *>(&dataSize), sizeof(short));
			//ssHeader.dataLength = dataSize;
			_logH.subHeaders.at(i).labels.at(nlabels).dataLength = dataSize;

			ofs << dataSize << " ";

			cerr << "DataSize: (" << dataSize << ")\n";

			// NumberOfValues
			size_t values;
			_file.read( reinterpret_cast<char *>(&values), sizeof(values));
			//ssHeader.numberOfValues = values;
			_logH.subHeaders.at(i).labels.at(nlabels).numberOfValues = values;

			ofs << values << " ";

			cerr << "Number of values: (" << values << ")\n";

			//subHeader.labels.push_back( ssHeader);

			cerr << labelLength << "|" << label << "|" << dataType << "|" << dataSize << "|" << values << "|\n";
		}

		//cerr << "Antes de insertar sub-cabecera (" << headerId << ")\n";
		//_logH.subHeaders[headerId - 1] = subHeader;
		//_logH.subHeaders.push_back(subHeader);
		//cerr << "Despuéss de insertar sub-cabecera (" << headerId << ")\n";
	}

	cerr << endl;
	ofs << endl;

	cerr << "Cuerpo\n";
	// Time
	unsigned long time;

	while ( _file.read( reinterpret_cast<char *>(&time), sizeof(time)) ){

		/*_file.read( reinterpret_cast<char *>(&total), sizeof(total));
		cout << "|" << total << "|\n";
		ofs << total << "\n";*/



		ofs << time << " ";

		cerr << "Time: (" << time << ")\n";

		// HeaderId
		int headerId;
		_file.read( reinterpret_cast<char *>(&headerId), sizeof(headerId));
		ofs << headerId << " ";

		cerr << "HeaderId: (" << headerId << ")\n";

		size_t numLabels = _logH.subHeaders.at( headerId ).labels.size();

		cerr << "NumLabels: (" << numLabels << ")\n";

		cerr << "|" << time << "|" << headerId << "|\n";

		for (unsigned i = 0; i < numLabels; i++ ) {
			// SubSubHeader
			tLogSubSubHeader ssHeader = _logH.subHeaders.at( headerId ).labels.at(i);
			//ComponentSnapshot::LogDataType dataType = ssHeader.type;
			int dataType = ssHeader.type;
			unsigned long nValues = ssHeader.numberOfValues;

			cerr << "DataType: (" << dataType << ")\n";
			cerr << "nValues: (" << nValues << ")\n";

			for (unsigned long j = 0; j < nValues; j++ ) {
				switch (dataType) {
				case ComponentSnapshot::LOG_BOOL:
					bool v1;
					_file.read( reinterpret_cast<char *>(&v1), sizeof(v1));
					ofs << v1 << " ";
					cerr << v1 << "|";
					break;
				case ComponentSnapshot::LOG_BYTE:
					char v2;
					_file.read( reinterpret_cast<char *>(&v2), sizeof(v2));
					ofs << v2 << " ";
					cerr << v2 << "|";
					break;
				case ComponentSnapshot::LOG_SHORT:
					short v3;
					_file.read( reinterpret_cast<char *>(&v3), sizeof(v3));
					ofs << v3 << " ";
					cerr << v3 << "|";
					break;
				case ComponentSnapshot::LOG_INT:
					int v4;
					_file.read( reinterpret_cast<char *>(&v4), sizeof(v4));
					ofs << v4 << " ";
					cerr << v4 << "|";
					break;
				case ComponentSnapshot::LOG_LONG:
					long v5;
					_file.read( reinterpret_cast<char *>(&v5), sizeof(v5));
					ofs << v5 << " ";
					cerr << v5 << "|";
					break;
				case ComponentSnapshot::LOG_FLOAT:
					float v6;
					_file.read( reinterpret_cast<char *>(&v6), sizeof(v6));
					ofs << v6 << " ";
					cerr << v6 << "|";
					break;
				case ComponentSnapshot::LOG_DOUBLE:
					double v7;
					_file.read( reinterpret_cast<char *>(&v7), sizeof(v7));
					ofs << v7 << " ";
					cerr << v7 << "|";
					break;
				case ComponentSnapshot::LOG_STRING:
					size_t v8l;
					char cc;
					string v8;

					_file.read( reinterpret_cast<char *>(&v8l), sizeof(size_t));
					ofs << v8l << " ";
					for (unsigned k = 0; k < v8l; k++) {
						_file.read( reinterpret_cast<char *>(&cc), sizeof(char));
						v8 += cc;
					}

					ofs << v8 << " ";
					cerr << v8 << "|";

					break;
				}
			}
		}
		cerr << endl;
		ofs << endl;

	}

	_file.close();
	ofs.close();

	cerr << "Cierra el fichero\n";
}
