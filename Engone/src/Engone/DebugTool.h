#pragma once

#include "ConsoleHandler.h"

namespace engone
{
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

	void Logging(LogHead head, LogStatus status);

	void DebugInit();
	void UpdateDebug(double delta);
	void RenderDebug(double lag);
}