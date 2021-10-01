#include <ncurses.h>
#include <unistd.h>
#include <vector>
#include <string>
#include <cstdint>
#include <cstring>
#include "constants.hh"
using std::vector;
using std::string;

#define ctrl(x)    ((x) & 0x1f)

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

int main(int argc, const char* argv[]) {
	string fname = "Unnamed";
	string fbuf = "";
	uint16_t maxx, maxy;
	bool run = true;
	bool renderCurs;
	uint64_t curp = 0;
	uint16_t curx = 0, cury = 0;
	uint16_t in;
	string instr = "";
	uint64_t scrollY = 0;
	uint64_t lines, cols;

	initscr();
	start_color();
	cbreak();
	use_default_colors();
	nodelay(stdscr, true);
	keypad(stdscr, true);
	curs_set(0);

	init_pair(1, COLOR_BLACK, COLOR_WHITE); // inverted theme
	init_pair(2, COLOR_WHITE, COLOR_BLUE);
	init_pair(3, COLOR_BLUE, COLOR_WHITE);

	while (run) {
		attroff(A_ALTCHARSET);
		maxx = getmaxx(stdscr);
		maxy = getmaxy(stdscr);
		// render
		move(0, 0);
		attron(COLOR_PAIR(1));
		printw(APP_NAME); // titlebar
		printw(" | ");
		for (uint16_t i = 0; i<maxx-strlen(APP_NAME) - 3; ++i) {
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
		for (uint64_t i = 0; i<=fbuf.length(); ++i) {
			if (fbuf[i] == 10) {
				++ lines;
				cols = 0;
			}
			else {
				++ cols;
			}
			if ((lines >= scrollY) && (lines-scrollY < maxy)) {
				if (i == curp) {
					attron(COLOR_PAIR(3));
				}
				else {
					attroff(COLOR_PAIR(3));
					attron(COLOR_PAIR(2));
				}
				if ((cols < maxx-1) && (i != fbuf.length())) {
					if (fbuf[i] == 10) {
						move((lines-scrollY)+2, 1);
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
		refresh();
		usleep(1000000/MAX_FPS);
		in = getch();
		switch (in) {
			default: {
				if ((in == 10) || (in >= 32 && in <= 126)) {
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
			case KEY_BACKSPACE: {
				if (curp-1 != -1) {
					fbuf.erase(curp-1, 1);
					-- curp;
				}
			}
		}
	}
}
