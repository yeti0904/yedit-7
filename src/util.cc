#include <string>
#include <ctime>
#include <cstring>
using std::string;

string currentTime() {
	time_t     now = time(0);
	struct tm  tstruct;
	char       buf[80];
	tstruct = *localtime(&now);
	strftime(buf, sizeof(buf), "%X", &tstruct);
	return buf;
}

bool strIsNum(string str) { // made by reinhold
	for (const char& ch : str)
		if (ch < '0' && ch > '9')
			return false;
	return true;
}