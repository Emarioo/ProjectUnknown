#pragma once

#include "ConsoleHandler.h"

namespace engone
{
	namespace log
	{
		// Will change color of out stream
		enum ConsoleColorCode : char
		{
			BLACK,
			NAVY,
			GREEN,
			CYAN,
			BLOOD,
			PURPLE,
			GOLD,
			SILVER,
			GRAY,
			BLUE,
			LIME,
			AQUA,
			RED,
			MAGENTA,
			YELLOW,
			WHITE,
			_BLACK = 0x00,
			_BLUE = 0x10,
			_GREEN = 0x20,
			_TEAL = 0x30,
			_BLOOD = 0x40,
			_PURPLE = 0x50,
			_GOLD = 0x60,
			_SILVER = 0x70,
			_GRAY = 0x80,
			_NAVY = 0x90,
			_LIME = 0xA0,
			_AQUA = 0xB0,
			_RED = 0xC0,
			_MAGENTA = 0xD0,
			_YELLOW = 0xE0,
			_WHITE = 0xF0,
		};
		ConsoleColorCode operator|(ConsoleColorCode c0, ConsoleColorCode c1);
		struct logger
		{
			logger() {}
			logger operator<<(const std::string& s);
			logger operator<<(char c);
			logger operator<<(int i);
			logger operator<<(float f);
			logger operator<<(double d);
			logger operator<<(char* a);
			logger operator<<(unsigned int i);
			logger operator<<(glm::vec2 v);
			logger operator<<(glm::vec3 v);
			logger operator<<(glm::vec4 v);
			logger operator<<(glm::ivec2 v);
			logger operator<<(glm::ivec3 v);
			logger operator<<(glm::quat q);
			logger operator<<(glm::mat4 v);
			logger operator<<(ConsoleColorCode t);
			logger operator<<(void* a);
			//debug operator<(engine::ShaderType t);
		};
		// stream like std::cout but support for glm data types
		extern logger out;
		// using this char in out stream will print the time Hour:Min:Sec
		extern char TIME;
	}

	enum class LogStatus
	{
		Info, Error, Header
	};
	struct LogHead
	{
		std::string strings[6];
	};
	class LogLine
	{
	public:
		LogLine()=default;
		LogLine(const std::string& msg, int indent, LogStatus status);
		
		bool opened = false;

		LogStatus status;

		std::string message;
		int errorCount=0;
		int infoCount=0;
		int indent = 0;
	};

	// Deprecated
	void Logging(LogHead head, LogStatus status);
	// Deprecated, Add event listener for key/console
	void DebugInit();
	// Deprecated
	void UpdateDebug(double delta);
	// Deprecated
	void RenderDebug(double lag);
}