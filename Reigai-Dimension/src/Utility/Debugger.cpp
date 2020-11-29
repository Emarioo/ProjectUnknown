#include "Debugger.h"

#include <Windows.h>

namespace bug {
	/*
	int BLACK=0;
	int OCEAN=1;
	int GREEN=2;
	int CYAN=3;
	int BLOOD=4;
	int PURPLE=5;
	int GOLD=6;
	int GRAY1=7;
	int GRAY2=8;
	int BLUE=9;
	int LIME=10;
	int AQUA=11;
	int RED=12;
	int MAGENTA=13;
	int YELLOW=14;
	int WHITE=15;
	*/
	char end = '\n';
	debug out;
	HANDLE hConsole = GetStdHandle(-11);
	//std::string print;
	bool reset = true;
	bool doPrint = true;
	void on() {
		doPrint = true;
	}
	void off() {
		doPrint = false;
	}/*
	void c(int f) {
		SetConsoleTextAttribute(hConsole, f);
	}
	void c(int f,int b) {
		SetConsoleTextAttribute(hConsole, f+b*16);
	}
	void co(int f) {
		c(f);
		once = true;
	}
	void co(int f, int b) {
		c(f,b);
		once = true;
	}*/
	debug debug::operator+(const std::string& s) {
		if (doPrint) {
			if (s.back() == '\n') {
				std::cout << s;
				//std::cout << print << s;
				//print.clear();
				if (reset) {
					SetConsoleTextAttribute(hConsole, GRAY1);
				}
			} else {
				//print+=s;
				std::cout << s;
			}
		}
		return out;
	}
	debug debug::operator+(glm::vec3 v) {
		if (doPrint) {
			std::cout << "[" << v.x << " " << v.y << " " << v.z << "]";
			/*
			print += "[";
			print += v.x;
			print += " ";
			print += v.y;
			print += " ";
			print += v.z;
			print += "]";
			*/
		}
		return out;
	}
	debug debug::operator+(float f) {
		if (doPrint)
			std::cout << f;
			//print += f;
		return out;
	}
	debug debug::operator+(char c) {
		if (doPrint) {
			std::cout << c;
			if (c == end&&reset) {
				SetConsoleTextAttribute(hConsole, GRAY1);
			}
		}
		//print+=""+i;
		return out;
	}
	debug debug::operator+(int i) {
		if (doPrint) {
			std::cout << i;
			//print+=""+i;
		}
		return out;
	}
	debug debug::operator+(unsigned int i) {
		if (doPrint) {
			std::cout << i;
			//print+=""+i;
		}
		return out;
	}
	debug debug::operator+(TerminalCode t) {
		if (doPrint)
			SetConsoleTextAttribute(hConsole, t);
		return out;
	}
	debug debug::operator+(MaterialType t) {
		if (doPrint)
			std::cout << (int)t;
		return out;
	}
}