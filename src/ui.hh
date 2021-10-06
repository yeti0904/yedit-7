#pragma once
#include "win.hh"
void rectangle(int y1, int x1, int y2, int x2);
ui_window newWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h, string title);
void printOnWindow(ui_window &win, string buf);
void changeTitleWindow(ui_window &win, string title);
void clearWindow(ui_window &win);
void changeTitleWindow(ui_window &win, string title);
void moveWindow(ui_window &win, uint16_t x, uint16_t y);
void resizeWindow(ui_window &win, uint16_t w, uint16_t h);
void renderWindow(ui_window win);