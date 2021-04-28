#pragma once

#include <string>
#include <iostream>

#include "Rendering/Shader.h"

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
	
	/*
	FileManager : LoadMesh, LoadAnimation, LoadCollider, LoadArmature
	*/
	bool is(const std::string& name);
	/*
	FileManager : LoadMesh, LoadAnimation, LoadCollider, LoadArmature
	*/
	void set(const std::string& name, bool on);
	void Enable(bool state);
	void ENABLE_DEBUG_OVERRIDE();

	struct debug {
		bool spaces = false;
		debug() {}
		debug operator<(const std::string& s);
		debug operator<(glm::ivec2 v);
		debug operator<(glm::vec2 v);
		debug operator<(glm::ivec3 v);
		debug operator<(glm::vec3 v);
		debug operator<(glm::vec4 v);
		debug operator<(glm::quat q);
		debug operator<(glm::mat4 v);
		debug operator<(float f);
		debug operator<(double d);
		debug operator<(int i);
		debug operator<(char c);
		debug operator<(unsigned int i);
		debug operator<(TerminalCode t);
		//debug operator<(engine::ShaderType t);
		debug operator<(char* a);
		debug operator<(void* a);
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