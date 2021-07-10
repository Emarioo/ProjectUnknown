#pragma once

#include <string>

enum class LogType {
	Custom,Asset,Collision
};

class LogLine {
public:
	LogLine(const std::string& id, LogType type);
	void SetContent(const std::string& content);
	std::string id;
	std::string content;
	LogType type;
	float r=1, g=1, b=1, a=1;
};