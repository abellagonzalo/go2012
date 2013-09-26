#include "MovesSupplier.h"
#include <fstream>
#include <iostream>
#include <dirent.h>
#include <stdexcept>
#include <algorithm>


MovesSupplier::MovesSupplier(const std::string& path) : _path(path) 
{ 
	DIR *dp;
	if ((dp = opendir(_path.c_str())) == NULL) {
		// TODO: getionar errores
		// throw std::invalid_argument(_path + " is an invalid path.");
		std::cerr << _path << " is an invalid path." << std::endl;
	}
	closedir(dp);
}; 

MovesSupplier::~MovesSupplier() 
{
}

std::string 
MovesSupplier::getFullPath(const std::string& move, const std::string& group)
{
	std::vector<std::string> files;
	std::string groupPath(getFullPath(group));
	getFiles(groupPath, files);

	for (int i=0; i<files.size(); i++) {
		if (move == removeExtension(files[i])) {
			return groupPath + "/" + files[i];
		}
	}
}

std::vector<std::string> 
MovesSupplier::getGroups()
{
	std::vector<std::string> dirs;
	getDirectories(_path, dirs);
	return dirs;
}


std::vector<std::string> 
MovesSupplier::getMoves(const std::string& group)
{
	std::vector<std::string> files;
	std::string groupPath(getFullPath(group));
	getFiles(groupPath, files);

	std::vector<std::string> moves;
	for (int i=0; i<files.size(); i++) {
		moves.push_back(removeExtension(files[i]));
	}

	return moves;
}


bool 
MovesSupplier::exist(const std::string& group)
{
	std::vector<std::string> groups = getGroups();
	std::vector<std::string>::iterator it;
	it = std::find(groups.begin(), groups.end(), group);
	return (it != groups.end());
}


bool 
MovesSupplier::exist(const std::string& move, const std::string& group)
{
	std::vector<std::string> moves = getMoves(group);
	std::vector<std::string>::iterator it;
	it = std::find(moves.begin(), moves.end(), move);
	return (it != moves.end());
}

void
MovesSupplier::getDirectories(const std::string& path, std::vector<std::string>& dirs)
{
	dirs.clear();

	DIR *dp;
	if ((dp = opendir(path.c_str())) == NULL) {
		// TODO: como gestionar los errores
		//throw std::runtime_error(_path + " is an invalid path.");
		std::cerr << "MovesSupplier: Invalid path " << path << std::endl;
		return;
	}

	struct dirent *dirp;
	while ((dirp = readdir(dp)) != NULL) {
		std::string grp(dirp->d_name);

		// Skip ".", ".." and hidden directories
		if (grp[0]=='.') continue;

		if (dirp->d_type == DT_DIR) {
			dirs.push_back(grp);
		}
	}

	closedir(dp);	
}

void 
MovesSupplier::getFiles(const std::string& path, std::vector<std::string>& files)
{
	files.clear();

	DIR *dp;
	if ((dp = opendir(path.c_str())) == NULL) {
		// TODO: como gestionar los errores
		//throw std::runtime_error(groupPath + " is an invalid path.");
		std::cerr << "MovesSupplier: Invalid path " << path << std::endl;
		return;
	}

	struct dirent *dirp;
	while ((dirp = readdir(dp)) != NULL) {
		if (dirp->d_type == DT_REG) {
			std::string file(dirp->d_name);

			// Skip hidden files
			if (file[0]=='.') continue;

			files.push_back(file);
		}
	}

	closedir(dp);
}

std::string 
MovesSupplier::getFullPath(const std::string& group)
{
	return _path + group;
}

std::string 
MovesSupplier::removeExtension(const std::string& file)
{
	return file.substr(0, file.find_last_of("."));
}
