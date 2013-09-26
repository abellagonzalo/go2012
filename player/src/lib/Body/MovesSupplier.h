#ifndef MovesSupplier_H
#define MovesSupplier_H

#include <string>
#include <vector>

/**
* A layer over the move files structure in the file system.
* It makes easy to access the moves and it abstracts the 
* access to them.
*/
class MovesSupplier
{
	public:
		/** Constructor */
		MovesSupplier(const std::string& path); 

		/** Destructor */
		~MovesSupplier();

		/** Returns the path of a move in a group */
		std::string getFullPath(const std::string& move, const std::string& group);

		/** Returns a vector with all available groups */
		std::vector<std::string> getGroups();

		/** Returns all moves in a group */
		std::vector<std::string> getMoves(const std::string& group);

		/** Checks if a group exists */
		bool exist(const std::string& group);

		/** Checks if a move exists in a group */
		bool exist(const std::string& move, const std::string& group);

	private:
		/** Moves default path */
		std::string _path;

		/** Returns all directories from path */
		void getDirectories(const std::string& path, std::vector<std::string>& dirs);

		/** Returns all files from a directory */
		void getFiles(const std::string& path, std::vector<std::string>& files); 

		/** Returns the path to a group of moves */
		std::string getFullPath(const std::string& group);

		/** Removes extension from a file */
		std::string removeExtension(const std::string& file);
};

#endif // MovesSupplier_H
