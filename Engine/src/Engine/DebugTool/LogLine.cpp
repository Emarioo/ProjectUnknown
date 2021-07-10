#include "LogLine.h"

LogLine::LogLine(const std::string& id, LogType type) 
: id(id),type(type) {

}
void LogLine::SetContent(const std::string& content) {
	this->content = content;
}