#include <string>
#include <fstream>
#include <dirent.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
using std::string;
using std::ofstream;
using std::ifstream;

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

bool fexists(string fname) {
	ofstream file;
	file.open(fname);
	if (file.is_open()) {
		file.close();
		return true;
	}
	else
		return false;
}

bool o_fexists(string fname) {
	return (access(fname.c_str(), F_OK) == 0);
}

string fread(string fname) {
	ifstream fhnd;
	string line;
	string ret;
	fhnd.open(fname);
	while (getline(fhnd, line)) {
		ret += line + '\n';
	}
	fhnd.close();
	return ret;
}

void fcreate(string fname) {
	ofstream {fname};
}