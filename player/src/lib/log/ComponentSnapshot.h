#ifndef _ComponentSnapshot_H_
#define _ComponentSnapshot_H_

#include <string>
#include <vector>
#include <iostream>
#include <cstring>
#include "Common.h"

using namespace std;

class ComponentSnapshot {

public:

	static const int UNKNOWN_HEADER_ID = 99999;
	static const long int UNKNOWN_TIME = -99999;

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

	ComponentSnapshot();
	~ComponentSnapshot();

	int getHeaderId();
	void setHeaderId( int newHeaderId );
	void composeHeader (const string name, const vector<string>& labels, const vector<LogDataType>& types,
			const vector<short>& sizes, const vector<size_t>& numberOfvalues);
	vector<char>* getHeader();
	vector<char>* getBody();

	template <class T> void add (const T& value, vector<char> *dst)
	{
		char bytes[sizeof value];
		std::copy(static_cast<const char*>(static_cast<const void*>(&value)),
				static_cast<const char*>(static_cast<const void*>(&value)) + sizeof value,
				bytes);
		for (unsigned int j = 0; j < sizeof(bytes); j++)
			dst->push_back(bytes[j]);
	}

	template <class T> void	add (const vector<T>& value, vector<char> *dst)
	{
		for (size_t i = 0; i < value.size(); i++) {
			add( value[i], dst );
		}
	}

	void add (const string& value, vector<char> *dst)
	{
		add( value.size(), dst );
		for (unsigned i = 0; i < value.size(); i++) {
			char bytes[sizeof(char)];
			std::copy(static_cast<const char*>(static_cast<const void*>(&value[i])),
					static_cast<const char*>(static_cast<const void*>(&value[i])) + 1,
					bytes);
			for (unsigned int j = 0; j < sizeof(bytes); j++)
				dst->push_back(bytes[j]);
		}
	}

	void add (const vector<string>& value, vector<char> *dst)
	{
		for (size_t i = 0; i < value.size(); i++) {
			add( value[i], dst );
		}
	}

	void reset();

private:

	int _headerId;
	vector<char> _header;
	vector<char> _body;
};

#endif
