#include <string>
#include <ncurses.h>
#include "win.hh"
using std::string;

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

ui_window newWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h, string title) {
	return {title, x, y, w, h, ""};
}

void printOnWindow(ui_window &win, string buf) {
	win.content += buf;
}

void clearWindow(ui_window &win) {
	win.content = "";
}

void changeTitleWindow(ui_window &win, string title) {
	win.title = title;
}

void moveWindow(ui_window &win, uint16_t x, uint16_t y) {
	win.x = x;
	win.y = y;
}

void resizeWindow(ui_window &win, uint16_t w, uint16_t h) {
	win.w = w;
	win.h = h;
}

void renderWindow(ui_window win) {
	attron(COLOR_PAIR(7));
	for (uint16_t i = win.y; i<win.h+win.y; ++i) {
		for (uint16_t j = win.x; j<win.w+win.x; ++j) {
			move(i, j);
			addch(' ');
		}
	}
	// render content
	move(win.y + 1, win.x + 1);
	uint16_t cols, lines;
	cols  = 0;
	lines = 0;
	for (size_t i = 0; i<win.content.length(); ++i) {
		switch (win.content[i]) {
			case 10: {
				cols = 0;
				++ lines;
				move(win.y + 1 + lines, win.x + 1);
				break;
			}
			case 9: {
				cols += 4;
				move(win.y + 1 + lines, win.x + 1 + cols);
				break;
			}
			default: {
				if (win.x+cols<win.x+win.w)
					addch(win.content[i]);
				break;
			}
		}
	}
	rectangle(win.y, win.x, win.y+win.h, win.x+win.w);
	move(win.y, win.x + 1);
	printw(win.title.c_str());
	move(win.y, (win.x+win.w)-3);
	addch('[');
	attron(COLOR_PAIR(8));
	addch('X');
	attroff(COLOR_PAIR(8));
	attron(COLOR_PAIR(7));
	addch(']');
	attroff(COLOR_PAIR(7));
}