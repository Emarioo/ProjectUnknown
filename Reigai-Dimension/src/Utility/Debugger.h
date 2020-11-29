#pragma once

#include <string>
#include <iostream>

#include "Rendering/BufferContainer.h"

#include "glm/glm.hpp"

namespace bug {
	/*extern int BLACK;
	extern int OCEAN;
	extern int GREEN;
	extern int CYAN;
	extern int BLOOD;
	extern int PURPLE;
	extern int GOLD;
	extern int GRAY1;
	extern int GRAY2;
	extern int BLUE;
	extern int LIME;
	extern int AQUA;
	extern int RED;
	extern int MAGENTA;
	extern int YELLOW;
	extern int WHITE;*/
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
	
//	void on();
//	void off();
//	/*
//	Foreground
//	*/
//	void c(int f);
//	/*
//	Foreground, Background
//	*/
//	void c(int f,int b);
//	/*
//	Color once
//	*/
//	void co(int c);
	struct debug {
		debug() {}
		debug operator+(const std::string& s);
		debug operator+(glm::vec3 v);
		debug operator+(float f);
		debug operator+(int i);
		debug operator+(char c);
		debug operator+(unsigned int i);
		debug operator+(TerminalCode t);
		debug operator+(MaterialType t);
	};
	/*
	Example: bug::out + "Hello there/n"
	An addition to std::cout but + instead of << and simple color support
	*/
	extern debug out;
}