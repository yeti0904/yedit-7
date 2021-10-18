#pragma once
#include <cstdint>
#include <string>
#include <ncurses.h>
#include "ui.hh"
using std::string;

class ui_textbar {
	private:
	uint16_t x, y;
	uint16_t curp, scrollx;

	public:
	ui_textbar() {};
	uint16_t w;
	string content;
	void create(uint16_t ax, uint16_t ay, uint16_t aw) {
		x       = ax;
		y       = ay;
		w       = aw;
		content = "";
		curp    = 0;
		scrollx = 0;
	}

	void Move(uint16_t ax, uint16_t ay) {
		x = ax;
		y = ay;
	}

	void resize(uint16_t aw) {
		w = aw;
	}

	void input(uint16_t ain) {
		switch(ain) {
			case KEY_LEFT: {
				if (curp - 1 != -1) {
					-- curp;
				}

				if (scrollx > curp) {
					-- scrollx;
				}
				break;
			}
			case KEY_RIGHT: {
				if (curp + 1 <= content.length()) {
					++ curp;
				}

				if (scrollx + w - 1 < curp) {
					++ scrollx;
				}
				break;
			}
			case 127: case KEY_BACKSPACE: {
				if (curp-1 != -1) {
					content.erase(curp-1, 1);
					-- curp;

					if (scrollx > curp) {
						-- scrollx;
					}
				}
				break;
			}
			default: {
				if (ain >= 32 && ain <= 126) {
					content.insert(curp, string(1, ain));
					++ curp;

					if (scrollx + w - 1 < curp) {
						++ scrollx;
					}
				}
			}
		}
	}

	void render() {
		attron(COLOR_PAIR(2));
		mvprintw(y, x, content.substr(scrollx, w).c_str());
		if (content.length() - scrollx < w) {
			printw(string(w - (content.length() - scrollx), ' ').c_str());
		}
		attron(A_REVERSE);
		mvaddch(y, x + curp - scrollx, curp >= content.length()? ' ' : content[curp]);
		attroff(A_REVERSE);
		attroff(COLOR_PAIR(2));

		/*move(y+1, x+w);
		attroff(COLOR_PAIR(7));
		attron(COLOR_PAIR(9));
		for (uint16_t i = y; y<=y+h; ++i) {
			mvaddch(i+1, x+w, ' ');
		}
		attroff(COLOR_PAIR(9));
		attron(COLOR_PAIR(7));
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
		attroff(COLOR_PAIR(7));*/
	}
};
