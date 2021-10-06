#pragma once
#include <cstdint>
#include <string>
using std::string;

struct ui_window {
	string title;
	uint16_t x,y,w,h;
	string content;
};