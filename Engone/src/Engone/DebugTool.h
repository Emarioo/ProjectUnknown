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
		const char* strings[5];
	};
	class LogLine
	{
	public:
		LogLine();
		LogLine(const std::string& msg, LogStatus status);
		
		bool opened = false;

		LogStatus status;

		std::string message;
		int errorCount=0;
		int infoCount=0;

		std::vector<LogLine> logs;

		void add(LogHead& head, const std::string& msg, LogStatus& status, int depth=0);
		
		void render(int& line,int depth);
	};

	/*
	AssetManger 52 30
		ModelAsset 10 0
			Can't find thing
		MaterialAsset 20 10
			
	
	*/
	void Logging(LogHead head, const std::string& message, LogStatus status);

	void UpdateDebug(double delta);
	void RenderDebug(double lag);
}