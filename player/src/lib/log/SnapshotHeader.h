#ifndef _SNAPSHOTHEADER_H_
#define _SNAPSHOTHEADER_H_

#include <string>
#include <vector>
#include <iostream>
#include <cstring>
#include "Common.h"

using namespace std;

class SnapshotHeader {

public:

	static const int UNKNOWN_HEADER_ID = 99999;

	enum LogDataType {
		LOG_BOOL,
		LOG_BYTE,
		LOG_SHORT,
		LOG_INT,
		LOG_LONG,
		LOG_FLOAT,
		LOG_DOUBLE,
		LOG_STRING
	};

	SnapshotHeader(string componentName);
	~SnapshotHeader();

	int getHeaderId();
	void setHeaderId(int newHeaderId);
	void composeHeader (vector<string> labels, vector<LogDataType> types,
			vector<short> sizes, vector<unsigned long> numberOfvalues);
	vector<char> getHeader();

private:

	template <class T>
	void
	add (T value) {
		char bytes[sizeof value];
		std::copy(static_cast<const char*>(static_cast<const void*>(&value)),
				static_cast<const char*>(static_cast<const void*>(&value)) + sizeof value,
				bytes);
		for (unsigned int j = 0; j < sizeof(bytes); j++)
			_header.push_back(bytes[j]);
	}

	void
	add(string value)
	{
		char bytes[value.size() * sizeof(char)];
		std::copy(static_cast<const char*>(static_cast<const void*>(&value)),
				static_cast<const char*>(static_cast<const void*>(&value)) + sizeof value,
				bytes);
		for (unsigned int j = 0; j < sizeof(bytes); j++)
			_header.push_back(bytes[j]);
	}

	vector<char> _header;
	int _headerId;
	string _componentName;
};

#endif
