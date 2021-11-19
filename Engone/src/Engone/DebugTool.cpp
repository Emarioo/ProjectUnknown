#include "gonpch.h"

#include "DebugTool.h"

#include "EventManager.h"

namespace engone
{
	static bool enabled = true;
	static bool renderNew=false;
	static bool updateNew=false;

	static int selectedLine = 0;

	//LogLine::LogLine(): message("Base"), status(LogStatus::Header), opened(true) {}
	LogLine::LogLine(const std::string& msg,int indent, LogStatus status) : message(msg), indent(indent), status(status)	{}
	std::vector<LogLine> lines;
	void Logging(LogHead head, LogStatus status)
	{
		renderNew = true;
		updateNew = true;
		
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
			}
		}
		while(!head.strings[indent].empty()){
			lines.push_back({ head.strings[indent], indent, status });
			indent++;
		}
	}
	void DebugInit()
	{
		AddListener(new Listener(EventType::Key|EventType::Console, [](Event& e) {
			if (e.action == 1) {
				if (e.key == VK_UP) {
					int newSelected = 0;
					int closedIndent = 9999;
					for (int i = 0; i < lines.size(); i++) {
						if (lines[i].indent > closedIndent)
							continue;

						if (lines[i].opened) {
							closedIndent = 9990;
						}
						else {
							closedIndent = lines[i].indent;
						}
						if (selectedLine == i) {
							selectedLine = newSelected;
							break;
						}
						newSelected = i;
					}
				}
				else if (e.key == VK_DOWN) {
					int closedIndent = 9999;
					for (int i = selectedLine; i < lines.size();i++) {
						if (lines[i].indent > closedIndent)
							continue;

						if (lines[i].opened) {
							closedIndent = 9990;
						}
						else {
							closedIndent = lines[i].indent;
						}
						if (selectedLine != i) {
							selectedLine = i;
							break;
						}
					}
				}
				else if (e.key == VK_LEFT) {

					if (!lines[selectedLine].opened) {
						for (int i = selectedLine; i >= 0; i--) {
							//console::ConsoleString(50, i, std::to_string(lines[]));
							if (lines[selectedLine].indent-1==lines[i].indent) {
								lines[i].opened = false;
								selectedLine = i;
								break;
							}
						}
					}else
						lines[selectedLine].opened = false;
					
				}
				else if (e.key == VK_RIGHT) {
					lines[selectedLine].opened = true;
				}
			}
			renderNew = true;
			updateNew = true;

			return EventType::None;
			}));
	}
	void UpdateDebug(double delta)
	{
		if (!enabled)
			return;
		if (!updateNew)
			return;

		updateNew =false;
	}
	void RenderDebug(double lag)
	{
		if (!enabled)
			return;
		if (!renderNew)
			return;

		using namespace console;

		Clear(Color::_BLACK);

		int off=2;

		renderNew =false;
		int closedIndent=9999;
		int line = 0;
		for (int i = 0; i < lines.size(); i++) {
			LogStatus status = lines[i].status;
			bool opened = lines[i].opened;
			int indent = lines[i].indent;

			if (indent > closedIndent)
				continue;

			if (opened) {
				closedIndent = 999;
			}
			else {
				closedIndent = indent;
			}

			Color color;
			switch (status) {
			case LogStatus::Info:
				if (opened)
					color = Color::WHITE;
				else
					color = Color::SILVER;
				break;
			case LogStatus::Error:
				if (opened)
					color = Color::RED;
				else
					color = Color::BLOOD;
				break;
			case LogStatus::Header:
				if (opened)
					color = Color::YELLOW;
				else
					color = Color::GOLD;
				break;
			}
			if (i==selectedLine)
				Print(2*indent, line, "X", Color::YELLOW);
			
			Print(off+2 * indent, line, lines[i].message, color);
			if (status == LogStatus::Header) {
				std::string infoMsg = std::to_string(lines[i].infoCount);
				Print(off+2 * indent +lines[i].message.length() + 1, line, std::to_string(lines[i].infoCount), opened ? Color::WHITE : Color::SILVER);
				Print(off+2 * indent + lines[i].message.length() + 1 + infoMsg.length() + 1, line, std::to_string(lines[i].errorCount), opened ? Color::RED : Color::BLOOD);
			}
			line++;
		}
	}
}