#ifndef BodyCache_H
#define BodyCache_H

#include "alproxies/almotionproxy.h"
#include <map>

/**
* Intermediate cache for Body Component where moves are 
* stored to access them quickly.
*/
class BodyCache
{
	public:
		/** Constructor */
		BodyCache();

		/** Destructor */
		~BodyCache();

		/** Returns move names vector */
		const std::vector<std::string>& getNames(const std::string& move, const std::string& group);

		/** Returns move times values */
		const AL::ALValue& getTimes(const std::string& move, const std::string& group);

		/** Returns move keys values */
		const AL::ALValue& getKeys(const std::string& move, const std::string& group);

		/** Inserts a new move in cache */
		void insert(const std::string& move,
					const std::string& group,
					const std::vector<std::string>& names,
					const AL::ALValue& times,
					const AL::ALValue& keys);

		/** Erase move of a group from cache */
		void erase(const std::string& move, const std::string& group);

		/** Returns true if move is in cache */
		bool has(const std::string& move, const std::string& group);
		
	private:
		/** Names map */
		std::map<std::string, std::vector<std::string> > namesMap;

		/** Times map */
		std::map<std::string, AL::ALValue> timesMap;

		/** Keys map */
		std::map<std::string, AL::ALValue> keysMap;

		/** Method to create the key */
		std::string key(const std::string& move, const std::string& group);
};

#endif // BodyCache_H
