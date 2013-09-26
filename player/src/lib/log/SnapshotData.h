#ifndef _SNAPSHOTDATA_H_
#define _SNAPSHOTDATA_H_

#include <string>
#include <vector>
#include <iostream>
#include <cstring>
#include "Common.h"
#include "SnapshotHeader.h"

using namespace std;

class SnapshotData {

public:

	static const long int UNKNOWN_TIME = -99999;

	SnapshotData(SnapshotHeader *myHeader);
	~SnapshotData();

	template <class T>
	void
	add (T value) {
		char bytes[sizeof value];
		std::copy(static_cast<const char*>(static_cast<const void*>(&value)),
				static_cast<const char*>(static_cast<const void*>(&value)) + sizeof value,
				bytes);
		for (unsigned int j = 0; j < sizeof(bytes); j++)
			_body.push_back(bytes[j]);
	}

	template <class T>
	void
	add (vector<T> value) {
		if ( _header->getHeaderId() == SnapshotHeader::UNKNOWN_HEADER_ID) {
			cerr << begin_red << "SnapshotData::add() Unknown header Id\n" << end_color;
			return;
		}

		add( UNKNOWN_TIME );
		add( _header->getHeaderId() );

		for (unsigned int i = 0; i < value.size(); i++) {
			char bytes[sizeof value[i]];
			std::copy(static_cast<const char*>(static_cast<const void*>(&value[i])),
					static_cast<const char*>(static_cast<const void*>(&value[i])) + sizeof value[i],
					bytes);
			for (unsigned int j = 0; j < sizeof(bytes); j++)
				_body.push_back(bytes[j]);
		}
	}

	void
	add(vector<string> value)
	{
		if ( _header->getHeaderId() == SnapshotHeader::UNKNOWN_HEADER_ID) {
			cerr << begin_red << "SnapshotData::add() Unknown header Id\n" << end_color;
			return;
		}

		add( UNKNOWN_TIME );
		add( _header->getHeaderId() );

		for (unsigned int i = 0; i < value.size(); i++) {
			_body.push_back(value[i].size() * sizeof(char));
			char bytes[value.size() * sizeof(char)];
			std::copy(static_cast<const char*>(static_cast<const void*>(&value)),
					static_cast<const char*>(static_cast<const void*>(&value)) + sizeof value,
					bytes);
			for (unsigned int j = 0; j < sizeof(bytes); j++)
				_body.push_back(bytes[j]);
		}
	}

	vector<char> getSnapshotBody();
	void clear();

private:
	SnapshotHeader *_header;
	vector<char> _body;
};

#endif
