#pragma once
#include <cstdint>
#include <string>
#include <ncurses.h>
#include "ui.hh"
using std::string;

class ui_window {
	private:
	string title;
	uint16_t x, y;
	string content;

	public:
	ui_window() {};
	uint16_t w, h;
	void create(uint16_t ax, uint16_t ay, uint16_t aw, uint16_t ah, string atitle) {
		x     = ax;
		y     = ay;
		w     = aw;
		h     = ah;
		title = atitle;
	}

	void print(string buf) {
		content += buf;
	}

	void clear() {
		content = "";
	}

	void changeTitle(string ttitle) {
		title = ttitle;
	}

	void Move(uint16_t ax, uint16_t ay) {
		x = ax;
		y = ay;
	}

	void resize(uint16_t aw, uint16_t ah) {
		w = aw;
		h = ah;
	}

	void render() {
		attron(COLOR_PAIR(7));
		for (uint16_t i = y; i<h+y; ++i) {
			for (uint16_t j = x; j<w+x; ++j) {
				mvaddch(i, j, ' ');
			}
		}
		move(y+1, x+w);
		/*attroff(COLOR_PAIR(7));
		attron(COLOR_PAIR(9));
		for (uint16_t i = y; y<=y+h; ++i) {
			mvaddch(i+1, x+w, ' ');
		}
		attroff(COLOR_PAIR(9));
		attron(COLOR_PAIR(7));*/
		// render content
		move(y + 1, x + 1);
		uint16_t cols, lines;
		cols  = 0;
		lines = 0;
		for (size_t i = 0; i<content.length(); ++i) {
			switch (content[i]) {
				case 10: {
					cols = 0;
					++ lines;
					move(y + 1 + lines, x + 1);
					break;
				}
				case 9: {
					cols += 4;
					move(y + 1 + lines, x + 1 + cols);
					break;
				}
				default: {
					if (x+cols<x+w)
						addch(content[i]);
					break;
				}
			}
		}
		rectangle(y, x, y+h, x+w);
		move(y, x + 1);
		printw(title.c_str());
		move(y, (x+w)-3);
		addch('[');
		attron(COLOR_PAIR(8));
		addch(ACS_DIAMOND);
		attroff(COLOR_PAIR(8));
		attron(COLOR_PAIR(7));
		addch(']');
		attroff(COLOR_PAIR(7));
	}
};
