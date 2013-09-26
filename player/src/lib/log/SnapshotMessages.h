#ifndef _SnapshotMessages_H_
#define _SnapshotMessages_H_

#include <string>
#include <vector>

using namespace std;

template <class T>
void
add (T value, vector<char> buffer) {
	char bytes[sizeof value];
	std::copy(static_cast<const char*>(static_cast<const void*>(&value)),
			static_cast<const char*>(static_cast<const void*>(&value)) + sizeof value,
			bytes);
	for (unsigned int j = 0; j < sizeof(bytes); j++)
		buffer.push_back(bytes[j]);
}

template <class T>
void
add (vector<T> value, vector<char> buffer) {
	for (unsigned int i = 0; i < value.size(); i++)
		add( value[i], buffer );
}

void
add(string value, vector<char> buffer)
{
	buffer.push_back(value.size() * sizeof(char));
	char bytes[value.size() * sizeof(char)];
	std::copy(static_cast<const char*>(static_cast<const void*>(&value)),
			static_cast<const char*>(static_cast<const void*>(&value)) + sizeof value,
			bytes);
	for (unsigned int j = 0; j < sizeof(bytes); j++)
		buffer.push_back(bytes[j]);
}

void
add(vector<string> value, vector<char> buffer)
{
	for (unsigned int i = 0; i < value.size(); i++)
		add( value[i], buffer );
}

#endif
