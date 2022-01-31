#include "gonpch.h"

#include "DebugHandler.h"

#include "EventHandler.h"

#include <time.h>

namespace engone
{
	namespace log
	{
		ConsoleColorCode operator|(ConsoleColorCode c0, ConsoleColorCode c1)
		{
			return (ConsoleColorCode)((char)c0|(char)c1);
		}
		HANDLE hConsole = GetStdHandle(-11);
		logger out;
		char TIME = 24;
		logger logger::operator<<(const std::string& s)
		{
			//if (engone::CheckState(GameState::DebugLog)) {
				/*if (s.size() > 0) {
					if (s.back() == end) {
						std::cout << s;
						if (resetColor) {
							SetConsoleTextAttribute(hConsole, GRAY1);
						}
					}
					else {

					}
				}*/
			std::cout << s;
			//}
			return *this;
		}
		logger logger::operator<<(float f)
		{
			//if (engone::CheckState(GameState::DebugLog)) {
			std::cout << f;
			//}
			return *this;
		}
		logger logger::operator<<(double d)
		{
			//if (engone::CheckState(GameState::DebugLog)) {
			std::cout << d;

			//}
			return *this;
		}
		logger logger::operator<<(char c)
		{
			if (c == TIME) {
				time_t timer=time(NULL);
				struct tm info;
				localtime_s(&info,&timer);
				char buffer[9];
				strftime(buffer,9,"%H:%M:%S",&info);
				std::cout << "[" << buffer<<"]";
			}else
			//if (engone::CheckState(GameState::DebugLog)) {
				std::cout << c;
			/*if (c == end) {
				if (resetColor)
					SetConsoleTextAttribute(hConsole, GRAY1);
			}
			else {

			}*/
			//}
			//print+=""+i;
			return *this;
		}
		logger logger::operator<<(int i)
		{
			//if (engone::CheckState(GameState::DebugLog)) {
			std::cout << i;
			//print+=""+i;

		//}
			return *this;
		}
		logger logger::operator<<(unsigned int i)
		{
			//if (engone::CheckState(GameState::DebugLog)) {
			std::cout << i;
			//print+=""+i;

		//}
			return *this;
		}
		logger logger::operator<<(char* s)
		{
			//if (engone::CheckState(GameState::DebugLog)) {
			std::cout << s;

			//}
			return *this;
		}
		logger logger::operator<<(void* t)
		{
			//if (engone::CheckState(GameState::DebugLog)) {
			std::cout << t;
			//if (spaces)
			//	std::cout << " ";
		//}
			return *this;
		}
		logger logger::operator<<(glm::ivec2 v)
		{
			//if (engone::CheckState(GameState::DebugLog)) {
			std::cout << "[" << v.x << " " << v.y << "]";
			//if (spaces)
			//	std::cout << " ";
		//}
			return *this;
		}
		logger logger::operator<<(glm::vec2 v)
		{
			//if (engone::CheckState(GameState::DebugLog)) {
			std::cout << "[" << v.x << " " << v.y << "]";
			//if (spaces)
			//	std::cout << " ";
		//}
			return *this;
		}
		logger logger::operator<<(glm::vec3 v)
		{
			//if (engone::CheckState(GameState::DebugLog)) {
			std::cout << "[" << v.x << " " << v.y << " " << v.z << "]";
			//if (spaces)
			//	std::cout << " ";
		//}
			return *this;
		}
		logger logger::operator<<(glm::ivec3 v)
		{
			//if (engone::CheckState(GameState::DebugLog)) {
			std::cout << "[" << v.x << " " << v.y << " " << v.z << "]";
			//if (spaces)
			//	std::cout << " ";
		//}
			return *this;
		}
		logger logger::operator<<(glm::vec4 v)
		{
			//if (engone::CheckState(GameState::DebugLog)) {
			std::cout << "[" << v.x << " " << v.y << " " << v.z << " " << v.w << "]";
			//if (spaces)
			//	std::cout << " ";
		//}
			return *this;
		}
		logger logger::operator<<(glm::quat q)
		{
			//if (engone::CheckState(GameState::DebugLog)) {
			std::cout << "[" << q.x << " " << q.y << " " << q.z << " " << q.w << "]";
			//if (spaces)
			//	std::cout << " ";
		//}
			return *this;
		}
		logger logger::operator<<(glm::mat4 m)
		{
			//if (engone::CheckState(GameState::DebugLog)) {
			for (int i = 0; i < 4; i++) {
				std::cout << "[" << m[i].x << " " << m[i].y << " " << m[i].z << " " << m[i].w << "]" << "\n";
			}

			//if (spaces)
			//	std::cout << " ";
		//}
			return *this;
		}
		logger logger::operator<<(ConsoleColorCode t)
		{
			//if (engone::CheckState(GameState::DebugLog))
			if (hConsole != NULL)
				SetConsoleTextAttribute(hConsole, t);
			return *this;
		}
	}
	/*debug debug::operator<(engone::ShaderType t) {
		if (doPrint) {
			std::cout << (int)t;
			if (spaces)
				std::cout << " ";
		}
		return *this;
	}*/

	static bool enabled = true;
	static bool renderNew=false;
	static bool updateNew=false;

	static int selectedLine = 0;

	//LogLine::LogLine(): message("Base"), status(LogStatus::Header), opened(true) {}
	LogLine::LogLine(const std::string& msg,int indent, LogStatus status) : message(msg), indent(indent), status(status)	{}
	std::vector<LogLine> lines;
	void Logging(LogHead head, LogStatus status)
	{
		for (int i = 1; i < 6;i++) {
			if (head.strings[i].empty()) {
				std::cout << head.strings[i-1] << "\n";
				break;
			}
		}
		
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
		/*
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
			*/
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