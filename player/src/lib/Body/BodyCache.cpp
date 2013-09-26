#include "BodyCache.h"
#include <stdexcept>

BodyCache::BodyCache()
{
}

BodyCache::~BodyCache()
{
}

const std::vector<std::string>& 
BodyCache::getNames(const std::string& move, const std::string& group)
{
	if (has(move,group)) return namesMap[key(move,group)];
	else throw std::invalid_argument("Move " + move + "doesn't exist in group " + group);
}

const AL::ALValue& 
BodyCache::getTimes(const std::string& move, const std::string& group)
{
	if (has(move,group)) return timesMap[key(move,group)];
	else throw std::invalid_argument("Move " + move + "doesn't exist in group " + group);
}

const AL::ALValue& 
BodyCache::getKeys(const std::string& move, const std::string& group)
{
	if (has(move,group)) return keysMap[key(move,group)];
	else throw std::invalid_argument("Move " + move + "doesn't exist in group " + group);
}

void 
BodyCache::insert(const std::string& move,
					const std::string& group,
					const std::vector<std::string>& names,
					const AL::ALValue& times,
					const AL::ALValue& keys)
{
	namesMap[key(move,group)] = names;
	timesMap[key(move,group)] = times;
	keysMap[key(move,group)] = keys;
}

void 
BodyCache::erase(const std::string& move, const std::string& group)
{
	namesMap.erase(key(move,group));
	timesMap.erase(key(move,group));
	keysMap.erase(key(move,group));
}

bool 
BodyCache::has(const std::string& move, const std::string& group)
{ 
	std::map<std::string, std::vector<std::string> >::iterator namesIt = namesMap.find(key(move,group));
	std::map<std::string, AL::ALValue>::iterator timesIt = timesMap.find(key(move,group));
	std::map<std::string, AL::ALValue>::iterator keysIt = keysMap.find(key(move,group));

	return (namesIt!=namesMap.end() && timesIt!=timesMap.end() && keysIt!=keysMap.end());
}

std::string 
BodyCache::key(const std::string& move, const std::string& group)
{
	return group + move;
}

