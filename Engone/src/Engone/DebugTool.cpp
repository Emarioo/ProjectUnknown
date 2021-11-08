#include "gonpch.h"

#include "DebugTool.h"

#include "EventManager.h"

namespace engone
{
	static bool enabled = true;
	static bool newMessage1=false;
	static bool newMessage2=false;

	static int selectedLine = 0;

	//LogLine::LogLine(): message("Base"), status(LogStatus::Header), opened(true) {}
	LogLine::LogLine(const std::string& msg,int indent, LogStatus status) : message(msg), indent(indent), status(status)	{}
	void LogLine::add(LogHead& head, const std::string& msg, LogStatus& status, int depth)
	{
		/*
		if (status == LogStatus::Error) errorCount++;
		else infoCount++;

		if (head.strings[depth] == nullptr) {
			for (int i = 0; i < logs.size();i++) {
				if (logs[i].message == msg) { // check if error?
					return;
				}
			}
			logs.push_back(LogLine(msg, status));
		}
		else {
			for (int i = 0; i < logs.size(); i++) {
				if (0==strcmp(head.strings[depth],logs[i].message.c_str())) {
					logs[i].add(head, msg, status, depth + 1);
					return;
				}
			}
			logs.push_back(LogLine(head.strings[depth], LogStatus::Header));
			logs.back().add(head,msg,status,depth+1);
		}
		*/
	}
	void LogLine::render(int& line, int depth)
	{
		/*
		if (depth != -1) {
			console::ConsoleColor color;
			switch (status) {
			case LogStatus::Info:
				if (opened)
					color = console::ConsoleColor::WHITE;
				else
					color = console::ConsoleColor::SILVER;
				break;
			case LogStatus::Error:
				if(opened)
					color = console::ConsoleColor::RED;
				else
					color = console::ConsoleColor::BLOOD;
				break;
			case LogStatus::Header:
				if(opened)
					color = console::ConsoleColor::YELLOW;
				else
					color = console::ConsoleColor::GOLD;
				break;
			}
			console::ConsoleColor background = console::ConsoleColor::_BLACK;
			if(line==selectedLine)
				background = console::ConsoleColor::_WHITE;

			console::ConsoleString(2 * depth, line, message, color|background);
			if (status == LogStatus::Header) {
				std::string infoMsg = std::to_string(infoCount);
				console::ConsoleString(2 * depth + message.length() + 1, line, std::to_string(infoCount), opened?console::ConsoleColor::WHITE:console::ConsoleColor::SILVER);
				console::ConsoleString(2 * depth + message.length() + 1 + infoMsg.length() + 1, line, std::to_string(errorCount), opened ? console::ConsoleColor::RED : console::ConsoleColor::BLOOD);
			}
			line++;
		}
		if (opened) {
			for (int i = 0; i < logs.size(); i++) {
				logs[i].render(line, depth + 1);
			}
		}
		*/
	}
	std::vector<LogLine> lines;
	void Logging(LogHead head, LogStatus status)
	{
		newMessage1 = true;
		newMessage2 = true;
		
		int indent = 0;
		for (int i = 0; i < lines.size(); i++) {
			if (lines[i].indent < indent) {
				lines.insert(lines.begin() + i, { head.strings[indent],indent, status });
				indent++;
				if (head.strings[indent].empty())
					return;
			}
			else if (lines[i].indent == indent &&
				lines[i].message == head.strings[indent]) {
				/*if (status == LogStatus::Error) lines[i].errorCount++;
				else lines[i].infoCount++;
				*/
				indent++;
				if (head.strings[indent].empty())
					return;
			}/*
			else if (i == lines.size() - 1) {
				lines.push_back({ head.strings[indent], indent, status });

				indent++;
				if (head.strings[indent].empty())
					return;
			}*/
		}
		while(!head.strings[indent].empty()){
			lines.push_back({ head.strings[indent], indent, status });
			indent++;
		}
	}
	void DebugInit()
	{
		AddListener(new Listener(EventType::Key|EventType::Console, [](Event& e) {
			if (e.key==VK_UP) {
				selectedLine--;
				if (selectedLine < 0) {
					selectedLine = 0;
				}
			}
			else if (e.key == VK_DOWN) {
				selectedLine++;
				if (selectedLine > 50) {
					selectedLine = 49;
				}
			}
			else if (e.key == VK_LEFT) {

			}
			else if (e.key == VK_RIGHT) {
				
			}

			return false;
			}));
	}
	void UpdateDebug(double delta)
	{
		if (!enabled)
			return;
		if (!newMessage1)
			return;

		newMessage1=false;
	}
	void RenderDebug(double lag)
	{
		if (!enabled)
			return;
		if (!newMessage2)
			return;

		newMessage2=false;
		
		for(int i=0;i<lines.size();i++){
			LogStatus status = lines[i].status;
			bool opened = lines[i].opened;
			int indent = lines[i].indent;

			console::ConsoleColor color;
			switch (status) {
			case LogStatus::Info:
				if (opened)
					color = console::ConsoleColor::WHITE;
				else
					color = console::ConsoleColor::SILVER;
				break;
			case LogStatus::Error:
				if (opened)
					color = console::ConsoleColor::RED;
				else
					color = console::ConsoleColor::BLOOD;
				break;
			case LogStatus::Header:
				if (opened)
					color = console::ConsoleColor::YELLOW;
				else
					color = console::ConsoleColor::GOLD;
				break;
			}
			console::ConsoleColor background = console::ConsoleColor::_BLACK;
			if (i == selectedLine)
				background = console::ConsoleColor::_WHITE;

			console::ConsoleString(2 * indent, i, lines[i].message, color | background);
			if (status == LogStatus::Header) {
				std::string infoMsg = std::to_string(lines[i].infoCount);
				console::ConsoleString(2 * indent +lines[i].message.length() + 1, i, std::to_string(lines[i].infoCount), opened ? console::ConsoleColor::WHITE : console::ConsoleColor::SILVER);
				console::ConsoleString(2 * indent + lines[i].message.length() + 1 + infoMsg.length() + 1, i, std::to_string(lines[i].errorCount), opened ? console::ConsoleColor::RED : console::ConsoleColor::BLOOD);
			}
		}
	}
}