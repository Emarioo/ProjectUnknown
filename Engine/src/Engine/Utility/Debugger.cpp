#include "Debugger.h"

#include <Windows.h>

#if PR_DEBUG == 1
#define DEBUG_MODE() true
#else
#define DEBUG_MODE() false
#endif


namespace bug {
	
	char end = '\n';
	debug out;
	debugs outs;
	HANDLE hConsole = GetStdHandle(-11);
	
	std::vector<std::string> debugVarOn;
	bool is(const std::string& name) {
		if (DEBUG_MODE()) {
			for (auto s : debugVarOn) {
				if (name == s) {
					return true;
				}
			}
		}
		return false;
	}
	void set(const std::string& name, bool on) {
		if (DEBUG_MODE()) {
			if (on) {
				for (auto s : debugVarOn) {
					if (name == s) {
						return;
					}
				}
				debugVarOn.push_back(name);
			} else {
				for (int i = 0; i < debugVarOn.size(); i++) {
					if (debugVarOn[i] == name) {
						debugVarOn.erase(debugVarOn.begin() + i);
						break;
					}
				}
			}
		}
	}
	bool debug_override = false;
	void ENABLE_DEBUG_OVERRIDE(){
		debug_override = true;
	}
	bool resetColor = true;
	/*
	Enabled in Engine.cpp if debug mode
	*/
	bool doPrint = false;
	void Enable(bool state) {
		if(DEBUG_MODE()||debug_override)
			doPrint = state;
	}
	debug debug::operator<(const std::string& s) {
		if (doPrint) {
			if (s.size() > 0){
				if (s.back() == end) {
					std::cout << s;
					if (resetColor) {
						SetConsoleTextAttribute(hConsole, GRAY1);
					}
				} else {
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
	debug debug::operator<(glm::quat q) {
		if (doPrint) {
			std::cout << "[" << q.w << " " << q.x << " " << q.y << " " << q.z << "]";
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