#ifdef _WIN32
	#error yedit does not support windows
#endif
#include <ncurses.h>
#include <unistd.h>
#include <vector>
#include <string>
#include <fstream>
#include <filesystem>
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <ctime>
#include "constants.hh"
#include "editmode.hh"
#include "fs.hh"
using std::vector;
using std::string;
using std::ofstream;
using std::ifstream;
using std::endl;

#define ctrl(x)    ((x) & 0x1f)

bool   alert = false;
string alertContent;
int16_t alertDuration;

bool   boxin = false;
string boxinTitle;

void rectangle(int y1, int x1, int y2, int x2) {
	mvhline(y1, x1, 0, x2-x1);
	mvhline(y2, x1, 0, x2-x1);
	mvvline(y1, x1, 0, y2-y1);
	mvvline(y1, x2, 0, y2-y1);
	mvaddch(y1, x1, ACS_ULCORNER);
	mvaddch(y2, x1, ACS_LLCORNER);
	mvaddch(y1, x2, ACS_URCORNER);
	mvaddch(y2, x2, ACS_LRCORNER);
}

std::string currentTime() {
	time_t     now = time(0);
	struct tm  tstruct;
	char       buf[80];
	tstruct = *localtime(&now);
	strftime(buf, sizeof(buf), "%X", &tstruct);
	return buf;
}

void showAlert(string alertc) {
	alert = true;
	alertContent = alertc;
	alertDuration = 3000;
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

uint64_t countLines(string buf) {
	uint64_t ret = 0;
	for (uint64_t i = 0; i<buf.length(); ++i) {
		if (buf[i] == 10)
			++ ret;
	}
	return ret;
}

bool strIsNum(string str) { // made by reinhold
    for (const char& ch : str)
        if (ch < '0' && ch > '9')
            return false;
    return true;
}

uint8_t strToColour(string buf) {
	if (buf == "black") {
		return COLOR_BLACK;
	}
	else if (buf == "red") {
		return COLOR_RED;
	}
	else if (buf == "green") {
		return COLOR_GREEN;
	}
	else if (buf == "yellow") {
		return COLOR_YELLOW;
	}
	else if (buf == "blue") {
		return COLOR_BLUE;
	}
	else if (buf == "magenta") {
		return COLOR_MAGENTA;
	}
	else if (buf == "cyan") {
		return COLOR_CYAN;
	}
	else if (buf == "white") {
		return COLOR_WHITE;
	}
	else {
		return COLOR_WHITE;
	}
}

int main(int argc, const char* argv[]) {
	string   fname = "Unnamed";
	char     fnamec;
	string   fbuf = "";
	uint16_t maxx, maxy;
	bool     run = true;
	bool     renderCurs;
	uint64_t curp = 0;
	uint16_t curx = 0, cury = 0;
	uint16_t in;
	string   instr = "";
	uint64_t scrollY = 0;
	uint64_t lines, cols;
	ofstream ofile;
	editMode emode = mode_txt;
	uint8_t  tabWidth = 4;
	bool     syntaxHighlighting = false;
	bool     inString;
	string   temp;
	bool     renderedCursor;

	vector <string> args;
	for (uint16_t i = 0; i<argc; ++i) {
		args.push_back(argv[i]);
	}

	// process args
	for (uint16_t i = 1; i<args.size(); ++i) {
		if (args[i][0] != '-') {
			fname = args[i];
			fbuf  = fread(fname);
		}
		else {
			if (args[i] == "--version") {
				printf(APP_NAME "\n");
				return 0;
			}
		}
	}

	initscr();
	start_color();
	raw();
	use_default_colors();
	keypad(stdscr, true);
	curs_set(0);

	if (!has_colors()) {
		printw("yedit requires a terminal that supports colour\npress any key to continue");
		refresh();
		getch();
		endwin();
		return 0;
	}

	nodelay(stdscr, true);

	// theme
	uint8_t editor_back   = COLOR_BLUE;
	uint8_t editor_fore   = COLOR_WHITE;
	uint8_t titlebar_back = COLOR_WHITE;
	uint8_t titlebar_fore = COLOR_BLACK;
	uint8_t alert_fore    = COLOR_BLACK;
	uint8_t alert_back    = COLOR_GREEN;
	uint8_t h_int         = COLOR_CYAN;
	uint8_t h_str         = COLOR_GREEN;
	

	init_pair(1, titlebar_fore, titlebar_back);
	init_pair(2, editor_fore, editor_back);
	init_pair(3, editor_back, editor_fore);
	init_pair(4, alert_fore, alert_back);
	init_pair(5, h_int, editor_back);
	init_pair(6, h_str, editor_back);

	showAlert("Welcome to YEDIT.");

	while (run) {
		attroff(A_ALTCHARSET);
		maxx = getmaxx(stdscr);
		maxy = getmaxy(stdscr);
		// render
		move(0, 0);
		attron(COLOR_PAIR(1));
		// titlebar
		for (uint16_t i = 0; i<maxx - 3; ++i) {
			addch(' ');
		}
		attroff(COLOR_PAIR(1));
		// render editor area
		move(1, 0);
		attron(COLOR_PAIR(2));
		for (uint16_t i = 0; i<maxx; ++i) {
			for (uint16_t j = 1; j<maxy; ++j) {
				addch(' ');
			}
		}
		move(2, 1);
		// render editor text
		lines = 0;
		cols  = 0;
		attron(COLOR_PAIR(2));
		renderCurs = false;
		inString = false;
		for (uint64_t i = 0; i<=fbuf.length(); ++i) {
			if (fbuf[i] == 10) {
				++ lines;
				cols = 0;
			}
			else
				++ cols;
			if ((lines >= scrollY) && (lines-scrollY < maxy)) {
				if (i == curp) {
					attroff(COLOR_PAIR(5));
					attroff(COLOR_PAIR(6));
					attron(COLOR_PAIR(3));
					if ((fbuf[i] == '"') || (fbuf[i] == '\''))
						inString = false;
				}
				else {
					attroff(COLOR_PAIR(3));
					attron(COLOR_PAIR(2));
					if (syntaxHighlighting) {
						if (((fbuf[i] == '"') || (fbuf[i] == '\'')) && (fbuf[i-1] != '\\')) {
							inString = !inString;
						}
						temp = "" + fbuf[i];
						if (!inString && ((fbuf[i] >= '0') && (fbuf[i] <= '9'))) {
							attron(COLOR_PAIR(5));
						}
						else if (inString) {
							attron(COLOR_PAIR(6));
						}
						else {
							attroff(COLOR_PAIR(5));
							attroff(COLOR_PAIR(6));
							attron(COLOR_PAIR(2));
						}
					}
				}
				if ((cols < maxx-1) && (i != fbuf.length())) {
					if (fbuf[i] == 10)
						move((lines-scrollY)+2, 1);
					else if (fbuf[i] == 9) {
						for (uint8_t i = 0; i<tabWidth; ++i) {
							addch(' ');
							attroff(COLOR_PAIR(3));
							attron(COLOR_PAIR(2));
						}
						cols += tabWidth-1;
					}
					else
						addch(fbuf[i]);
				}
				if ((cols < maxx-1) && (i == fbuf.length())) {
					addch(' ');
				}
			}
		}
		attroff(COLOR_PAIR(3));
		attron(COLOR_PAIR(2));
		rectangle(1, 0, maxy - 1, maxx - 1);
		move(1, 2);
		printw("%s", fname.c_str());
		move(0, maxx-currentTime().length());
		printw("%s", currentTime().c_str());
		if (alert) {
			move((maxy-1)/2, ((maxx-1)/2)-currentTime().length() - 1);
			attron(COLOR_PAIR(4));
			printw("[ %s ]", alertContent.c_str());
			alertDuration -= 1000/MAX_FPS;
			if (alertDuration <= 0)
				alert = false;
		}
		move(0, 0);
		attroff(COLOR_PAIR(2));
		attron(COLOR_PAIR(1));
		printw(APP_NAME " | ");
		attroff(COLOR_PAIR(1));
		refresh();
		usleep(1000000/MAX_FPS);
		in = getch();
		switch (in) {
			default: {
				if ((in == 10) || (in == 9) || (in >= 32 && in <= 126)) {
					fbuf.insert(curp, string(1, in));
					++ curp;
				};
					
				break;
			}
			case KEY_LEFT: {
				if (curp - 1 != -1) {
					-- curp;
				}
				break;
			}
			case KEY_RIGHT: {
				if (curp + 1 <= fbuf.length()) {
					++ curp;
				}
				break;
			}
			case KEY_UP: {
				if (scrollY-1 != -1)
					-- scrollY;
				break;
			}
			case KEY_DOWN: {
				if (scrollY+1 <= countLines(fbuf))
					++ scrollY;
				break;
			}
			case 127: case KEY_BACKSPACE: {
				if (curp-1 != -1) {
					fbuf.erase(curp-1, 1);
					-- curp;
				}
				break;
			}
			case ctrl('s'): {
				ofile.open(fname);
				if (true) {
					ofile << fbuf.c_str();
					ofile.close();
					showAlert("Saved buffer to " +fname);
				}
				else
					showAlert("Error saving file");
				break;
			}
			case ctrl('q'): {
				run = false;
				break;
			}
			case ctrl('h'): {
				syntaxHighlighting = !syntaxHighlighting;
				if (syntaxHighlighting)
					showAlert("Enabled syntax highlighting");
				else
					showAlert("Disabled syntax highlighting");
				break;
			}
		}
	}
	endwin();
	return 0;
}
