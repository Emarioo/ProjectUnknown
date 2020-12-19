#pragma once

#include <string>
#include <iostream>

#include "Rendering/BufferContainer.h"

#include "glm/glm.hpp"

namespace bug {
	enum TerminalCode {
		BLACK,
		OCEAN,
		GREEN,
		CYAN,
		BLOOD,
		PURPLE,
		GOLD,
		GRAY1,
		GRAY2,
		BLUE,
		LIME,
		AQUA,
		RED,
		MAGENTA,
		YELLOW,
		WHITE
	};
	extern char end;
	
	void on();
	void off();
	void set(bool);

	struct debug {
		bool spaces = false;
		debug() {}
		debug operator<(const std::string& s);
		debug operator<(glm::vec3 v);
		debug operator<(float f);
		debug operator<(double d);
		debug operator<(int i);
		debug operator<(char c);
		debug operator<(unsigned int i);
		debug operator<(TerminalCode t);
		debug operator<(MaterialType t);
	};
	struct debugs : public debug {
		debugs() {
			spaces = true;
		}
	};
	/*
	Example: bug::out + "Hello there/n"
	An addition to std::cout but + instead of << and simple color support
	*/
	extern debug out;
	extern debugs outs;
}