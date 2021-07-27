#include "DebugHandler.h"

#include "../Handlers/UIHandler.h"

#include "DebugPanel.h"

#include "GameState.h"

#include <Windows.h>

#include <vector>

namespace engine {
	DebugPanel* debugPanel;
	void SetupDebugPanel() {
		debugPanel = new DebugPanel("Debug Panel");
		debugPanel->conX.Left(0.f)->conY.Center(0.f)->conW.Center(.4f)->conH.Center(2.f);
		engine::AddBase(debugPanel);
	}
}
namespace bug {

	std::vector<LogLine> logReport;

	void UpdateLogReport() {

	}
	LogLine* AddLogLine(const std::string& id, LogType type) {
		logReport.push_back(LogLine(id, type));
		return &logReport.back();
	}
	void SetLogContent(const std::string& id, const std::string& content) {
		for (LogLine& l : logReport) {
			if (l.id == id) {
				l.SetContent(content);
			}
		}
	}
	void RenderDebug() {

	}

	char end = '\n';
	debug out;
	debugs outs;
	HANDLE hConsole = GetStdHandle(-11);
	
	std::vector<std::string> debugVarOn;
	bool is(const std::string& name) {
		if (engine::CheckState(GameState::DebugLog)) {
			for (auto s : debugVarOn) {
				if (name == s) {
					return true;
				}
			}
		}
		return false;
	}
	void set(const std::string& name, bool on) {
		if (engine::CheckState(GameState::DebugLog)) {
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
	bool resetColor = true;
	/*
	Enabled in Engine.cpp if debug mode
	*/
	debug debug::operator<(const std::string& s) {
		if (engine::CheckState(GameState::DebugLog)) {
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
		if (engine::CheckState(GameState::DebugLog)) {
			std::cout << "[" << v.x << " " << v.y << "]";
			if (spaces)
				std::cout << " ";
		}
		return *this;
	}
	debug debug::operator<(glm::vec2 v) {
		if (engine::CheckState(GameState::DebugLog)) {
			std::cout << "[" << v.x << " " << v.y << "]";
			if (spaces)
				std::cout << " ";
		}
		return *this;
	}
	debug debug::operator<(glm::vec3 v) {
		if (engine::CheckState(GameState::DebugLog)) {
			std::cout << "[" << v.x << " " << v.y << " " << v.z << "]";
			if (spaces)
				std::cout << " ";
		}
		return *this;
	}
	debug debug::operator<(glm::ivec3 v) {
		if (engine::CheckState(GameState::DebugLog)) {
			std::cout << "[" << v.x << " " << v.y << " " << v.z << "]";
			if (spaces)
				std::cout << " ";
		}
		return *this;
	}
	debug debug::operator<(glm::vec4 v) {
		if (engine::CheckState(GameState::DebugLog)) {
			std::cout << "[" << v.x << " " << v.y << " " << v.z <<" "<< v.w << "]";
			if (spaces)
				std::cout << " ";
		}
		return *this;
	}
	debug debug::operator<(glm::quat q) {
		if (engine::CheckState(GameState::DebugLog)) {
			std::cout << "[" << q.w << " " << q.x << " " << q.y << " " << q.z << "]";
			if (spaces)
				std::cout << " ";
		}
		return *this;
	}
	debug debug::operator<(glm::mat4 m) {
		if (engine::CheckState(GameState::DebugLog)) {
			for (int i = 0; i < 4;i++) {
				std::cout << "[" << m[i].x << " " << m[i].y << " " << m[i].z << " " << m[i].w << "]"<<bug::end;
			}

			if (spaces)
				std::cout << " ";
		}
		return *this;
	}
	debug debug::operator<(float f) {
		if (engine::CheckState(GameState::DebugLog)) {
			std::cout << f;
			if (spaces)
				std::cout << " ";
		}
		return *this;
	}
	debug debug::operator<(double d) {
		if (engine::CheckState(GameState::DebugLog)) {
			std::cout << d;
			if (spaces)
				std::cout << " ";
		}
		return *this;
	}
	debug debug::operator<(char c) {
		if (engine::CheckState(GameState::DebugLog)) {
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
		if (engine::CheckState(GameState::DebugLog)) {
			std::cout << i;
			//print+=""+i;
			if (spaces)
				std::cout << " ";
		}
		return *this;
	}
	debug debug::operator<(unsigned int i) {
		if (engine::CheckState(GameState::DebugLog)) {
			std::cout << i;
			//print+=""+i;
			if (spaces)
				std::cout << " ";
		}
		return *this;
	}
	debug debug::operator<(TerminalCode t) {
		if (engine::CheckState(GameState::DebugLog))
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
		if (engine::CheckState(GameState::DebugLog)) {
			std::cout << s;
			if (spaces)
				std::cout << " ";
		}
		return *this;
	}
	debug debug::operator<(void* t) {
		if (engine::CheckState(GameState::DebugLog)) {
			std::cout << t;
			if (spaces)
				std::cout << " ";
		}
		return *this;
	}
}