#include "Debugger.h"

#include <Windows.h>

namespace bug {
	
	char end = '\n';
	debug out;
	debugs outs;
	HANDLE hConsole = GetStdHandle(-11);
	
	bool resetColor = true;
	bool doPrint = true;
	void on() {
		doPrint = true;
	}
	void off() {
		doPrint = false;
	}
	void set(bool b) {
		doPrint = b;
	}
	debug debug::operator<(const std::string& s) {
		if (doPrint) {
			if (s.size() > 0){
				if (s.back() == end) {
					std::cout << s;
					//std::cout << print << s;
					//print.clear();
					if (resetColor) {
						SetConsoleTextAttribute(hConsole, GRAY1);
					}
				} else {
					//print+=s;
					std::cout << s;
					if (spaces)
						std::cout << " ";
				}
			}
		}
		return *this;
	}
	debug debug::operator<(glm::ivec2 v) {
		if (doPrint) {
			std::cout << "[" << v.x << " " << v.y << "]";
			if (spaces)
				std::cout << " ";
		}
		return *this;
	}
	debug debug::operator<(glm::vec2 v) {
		if (doPrint) {
			std::cout << "[" << v.x << " " << v.y << "]";
			if (spaces)
				std::cout << " ";
		}
		return *this;
	}
	debug debug::operator<(glm::vec3 v) {
		if (doPrint) {
			std::cout << "[" << v.x << " " << v.y << " " << v.z << "]";
			if (spaces)
				std::cout << " ";
		}
		return *this;
	}
	debug debug::operator<(glm::ivec3 v) {
		if (doPrint) {
			std::cout << "[" << v.x << " " << v.y << " " << v.z << "]";
			if (spaces)
				std::cout << " ";
		}
		return *this;
	}
	debug debug::operator<(glm::vec4 v) {
		if (doPrint) {
			std::cout << "[" << v.x << " " << v.y << " " << v.z <<" "<< v.w << "]";
			if (spaces)
				std::cout << " ";
		}
		return *this;
	}
	debug debug::operator<(glm::mat4 m) {
		if (doPrint) {
			for (int i = 0; i < 4;i++) {
				std::cout << "[" << m[i].x << " " << m[i].y << " " << m[i].z << " " << m[i].w << "]"<<bug::end;
			}

			if (spaces)
				std::cout << " ";
		}
		return *this;
	}
	debug debug::operator<(float f) {
		if (doPrint) {
			std::cout << f;
			if (spaces)
				std::cout << " ";
		}
		return *this;
	}
	debug debug::operator<(double d) {
		if (doPrint) {
			std::cout << d;
			if (spaces)
				std::cout << " ";
		}
		return *this;
	}
	debug debug::operator<(char c) {
		if (doPrint) {
			std::cout << c;
			if (c == end) {
				if(resetColor)
					SetConsoleTextAttribute(hConsole, GRAY1);
			} else {
				if (spaces)
					std::cout << " ";
			}
		}
		//print+=""+i;
		return *this;
	}
	debug debug::operator<(int i) {
		if (doPrint) {
			std::cout << i;
			//print+=""+i;
			if (spaces)
				std::cout << " ";
		}
		return *this;
	}
	debug debug::operator<(unsigned int i) {
		if (doPrint) {
			std::cout << i;
			//print+=""+i;
			if (spaces)
				std::cout << " ";
		}
		return *this;
	}
	debug debug::operator<(TerminalCode t) {
		if (doPrint)
			SetConsoleTextAttribute(hConsole, t);
		return *this;
	}
	/*debug debug::operator<(engine::ShaderType t) {
		if (doPrint) {
			std::cout << (int)t;
			if (spaces)
				std::cout << " ";
		}
		return *this;
	}*/
	debug debug::operator<(char* s) {
		if (doPrint) {
			std::cout << s;
			if (spaces)
				std::cout << " ";
		}
		return *this;
	}
	debug debug::operator<(void* t) {
		if (doPrint) {
			std::cout << t;
			if (spaces)
				std::cout << " ";
		}
		return *this;
	}
}