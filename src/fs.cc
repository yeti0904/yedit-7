#include <string>
#include <dirent.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
using std::string;

bool pexists(string pname) {
	DIR* dir = opendir(pname.c_str());
	if (dir) {
		closedir(dir);
		return true;
	} else if (ENOENT == errno) {
		return false;
	} else {
		return false;
	}
}

void pcreate(string pname) {
	struct stat st = {0};
	if (stat(pname.c_str(), &st) == -1) {
	    mkdir(pname.c_str(), 0700);
	}
}