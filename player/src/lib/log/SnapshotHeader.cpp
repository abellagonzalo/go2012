#include "SnapshotHeader.h"

SnapshotHeader::SnapshotHeader (string componentName)
{
	_componentName = componentName;
	_headerId = UNKNOWN_HEADER_ID;
}

SnapshotHeader::~SnapshotHeader()
{
	_header.clear();
}

int
SnapshotHeader::getHeaderId()
{
	return _headerId;
}

void
SnapshotHeader::setHeaderId(int newHeaderId)
{
	_headerId = newHeaderId;
}

void
SnapshotHeader::composeHeader (vector<string> labels, vector<LogDataType> types,
		vector<short> sizes, vector<unsigned long> numberOfValues)
{
	if (((labels.size() == types.size()) == sizes.size()) == numberOfValues.size()) {

		// Calculate the size of the header in bytes
		int headerSize = sizeof(_headerId) + sizeof(unsigned int) + _componentName.size() * sizeof(char) +
				sizeof(int);
		for (unsigned int i = 0; i < labels.size(); i++) {
			headerSize += sizeof(unsigned int) + labels[i].size() * sizeof(char) + sizeof(LogDataType) + sizeof(short) +
					sizeof(unsigned int);
		}

		// Fill the header
		_header.resize( headerSize );

		add( _headerId );
		add( _componentName.size() );
		add( _componentName );
		add( labels.size() );

		for (unsigned int i = 0; i < labels.size(); i++) {
			add ( labels[i].size() * sizeof(char) );
			add ( labels[i] );
			add ( types[i] );
			add ( sizes[i] );
			add ( numberOfValues[i] );
		}
	} else {
		//ToDo: use some value to avoid future exceptions returning void
		cerr << begin_red << "SnapshotHeader::composeHeader() Number of labels (" << labels.size() <<
				") and types (" << types.size() << ") does not mach\n" << end_color;
		return;
	}
}

vector<char>
SnapshotHeader::getHeader()
{
	return _header;
}
