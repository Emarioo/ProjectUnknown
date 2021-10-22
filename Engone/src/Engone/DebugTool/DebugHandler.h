#pragma once

#include "LogLine.h"

namespace engone {
	void SetupDebugPanel();
}
namespace bug {
	/*
	Update the logReport.txt
	*/
	void UpdateLogReport();

	LogLine* AddLogLine(const std::string& id, LogType type);
	void SetLogContent(const std::string& id, const std::string& content);

	/* DEBUG CMDS
		FileManager
		LoadMaterial
		LoadMesh All Weights Triangles Vectors Colors Normals
		LoadAnimation All Frames
		LoadCollider All Vectors Quads
		LoadArmature All Bones Matrix
		LoadModel All Mesh Matrix
	*/
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
	
	bool is(const std::string& name);
	
	void set(const std::string& name, bool on);

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