#include "Engone/LogModule/Logger.h"

//#include "Engone/Win32Includes.h"

#include "Engone/PlatformLayer.h"

#include <time.h>

namespace engone {
	namespace log {
		Logger out;
	}
	// This function is copied from Utilitiy.h to make the logger more independent.
	// str should have space of 8 characters.
	static void _GetClock(char* str) {
		time_t t;
		time(&t);
		tm tm;
		localtime_s(&tm, &t);

		str[2] = ':';
		str[5] = ':';

		int temp = tm.tm_hour / 10;
		str[0] = '0' + temp;
		temp = tm.tm_hour - 10 * temp;
		str[1] = '0' + temp;

		temp = tm.tm_min / 10;
		str[3] = '0' + temp;
		temp = tm.tm_min - 10 * temp;
		str[4] = '0' + temp;

		temp = tm.tm_sec / 10;
		str[6] = '0' + temp;
		temp = tm.tm_sec - 10 * temp;
		str[7] = '0' + temp;
	}
	void Logger::cleanup() {
		flush();
		
		for (auto& pair : m_threadInfos) {
			
			pair.second.lineBuffer.resize(0);
		}
		m_threadInfos.clear();
		for(auto& pair : m_logFiles){
			FileClose(pair.second);
		}
		m_logFiles.clear();
	}
	char* Logger::ThreadInfo::ensure(u32 bytes) {
		if (lineBuffer.max < lineBuffer.used + bytes + 1) {
			// Todo: increase by max*2x instead of used+bytes?
			bool yes = lineBuffer.resize(lineBuffer.used + bytes + 1);
			if (!yes) return nullptr;
		}
		return ((char*)lineBuffer.data + lineBuffer.used);
	}
	void Logger::ThreadInfo::use(u32 bytes) {
		lineBuffer.used += bytes;
		*((char*)lineBuffer.data + lineBuffer.used) = 0;
		//printf("%s",lineBuffer.data+lineBuffer.used);
	}
	void Logger::enableReport(bool yes){
		if(realLogger) realLogger->enableReport(yes);
		else m_enabledReports = yes;
	}
	void Logger::enableConsole(bool yes){
		if(realLogger) realLogger->enableConsole(yes);
		else m_enabledConsole = yes;
	}
	void Logger::setMasterColor(log::Color color){
		if(realLogger) realLogger->setMasterColor(color);
		else m_masterColor = color;
	}
	void Logger::setMasterReport(const std::string path){
		if(realLogger) realLogger->setMasterReport(path);
		else m_masterReportPath = path;
	}
	void Logger::setReport(const std::string path){
		if(realLogger) realLogger->setReport(path);
		else{
			auto& info = getThreadInfo();
			info.logReport = path;
		}
	}
	void Logger::useThreadReports(bool yes){
		if(realLogger) realLogger->useThreadReports(yes);
		else m_useThreadReports=yes;
	}
	void Logger::setRealLogger(Logger* logger){
		realLogger = logger;
	}
	Logger::ThreadInfo& Logger::getThreadInfo() {
		if(realLogger) return realLogger->getThreadInfo();
		
		auto id = Thread::GetThisThreadId();
		auto find = m_threadInfos.find(id);
		if (find == m_threadInfos.end()) {
			m_threadInfos[id] = {};
		}
		return m_threadInfos[Thread::GetThisThreadId()];
	}
	void Logger::flush(){
		if(realLogger) realLogger->flush();
		else {
			auto& info = getThreadInfo();
			if(info.lineBuffer.used==0)
				return;
			print((char*)info.lineBuffer.data, info.lineBuffer.used);
			info.lineBuffer.used = 0; // flush buffer
		}
	}
	void Logger::print(char* str, int len) {
		if(realLogger) realLogger->print(str,len);
		else{
			auto& info = getThreadInfo();
			m_printMutex.lock();
			if (m_masterColor == log::NO_COLOR)
				SetConsoleColor(info.color);
			else
				SetConsoleColor(m_masterColor);

			if(m_enabledConsole){
				printf("%s", str);
			}
			if(m_enabledReports){
				const char* te = "[Thread 65536] ";
				char extraBuffer[11+15+1]{0};
				extraBuffer[0]='[';
				extraBuffer[9]=']';
				extraBuffer[10]=' ';
				_GetClock(extraBuffer+1);
				sprintf(extraBuffer+11,"[Thread %u] ",Thread::GetThisThreadId());
				if(!m_masterReportPath.empty()){
					std::string path = m_rootDirectory+"/"+m_masterReportPath;
					auto find = m_logFiles.find(path);
					APIFile* file=nullptr;
					if(find==m_logFiles.end()){
						file = FileOpen(path,0,FILE_CAN_CREATE);
						if(file)
							m_logFiles[path] = file;
					}else{
						file = find->second;
					}
					if(file){
						FileWrite(file,extraBuffer,strlen(extraBuffer));
						u64 bytes = FileWrite(file,str,len);
						// Todo: check for failure
					}
				}
				if(!info.logReport.empty()){
					std::string path = m_rootDirectory+"/"+info.logReport;
					auto find = m_logFiles.find(path);
					APIFile* file=nullptr;
					if(find==m_logFiles.end()){
						file = FileOpen(path,0,FILE_CAN_CREATE);
						if(file)
							m_logFiles[path] = file;
					}else{
						file = find->second;
					}
					if(file){
						FileWrite(file,extraBuffer,strlen(extraBuffer));
						u64 bytes = FileWrite(file,str,len);
						// Todo: check for failure
					}
				}
				if(m_useThreadReports){
					std::string path = m_rootDirectory+"/thread";
					path += std::to_string((u32)Thread::GetThisThreadId())+".txt";
					auto find = m_logFiles.find(path);
					APIFile* file=nullptr;
					if(find==m_logFiles.end()){
						file = FileOpen(path,0,FILE_CAN_CREATE);
						if(file)
							m_logFiles[path] = file;
					}else{
						file = find->second;
					}
					if(file){
						FileWrite(file,extraBuffer,strlen(extraBuffer));
						u64 bytes = FileWrite(file,str,len);
						// Todo: check for failure
					}
				}
			}
			m_printMutex.unlock();

			info.color = log::SILVER; // reset color after new line
			if (m_masterColor == log::NO_COLOR)
				SetConsoleColor(log::SILVER);

			// Todo: write to report
		}
	}
	log::Color Logger::getColor() {
		if(realLogger) return realLogger->getColor();
		auto& info = getThreadInfo();
		if (m_masterColor == log::NO_COLOR)
			return info.color;
		else
			return m_masterColor;
	}
	Logger& Logger::operator<<(log::Color value) {
		if(realLogger) *realLogger << value;
		else getThreadInfo().color = value;
		return *this;
	}
	Logger& Logger::operator<<(log::Filter value) {
		if(realLogger) *realLogger << value;
		else getThreadInfo().filter = value;
		return *this;
	}
	Logger& Logger::operator<<(const char* value) {
		if(realLogger) *realLogger<<value;
		else {
			u32 len = strlen(value);

			if (len == 0) return *this;

			auto& info = getThreadInfo();

			char* buf = info.ensure(len);
			if (buf) {
				memcpy(buf, value, len);
				info.use(len);
			} else {
				printf("[Logger] : Failed insuring %u bytes\n", len); \
			}
			if (value[len - 1] == '\n') {
			flush();
			// print((char*)info.lineBuffer.data, info.lineBuffer.used);
			// info.lineBuffer.used = 0; // flush buffer
		}
		}
		return *this;
	}
	Logger& Logger::operator<<(char* value) {
		return *this << (const char*)value;
	}
	Logger& Logger::operator<<(char value) {
		char tmp[]{ value,0 };
		return *this << (const char*)&tmp;
	}
	Logger& Logger::operator<<(const std::string& value) {
		return *this << value.c_str();
	}

	// Am I lazy or smart?
#define GEN_LOG_NUM(TYPE,ENSURE,FORMAT)\
	Logger& Logger::operator<<(TYPE value) {\
		if(realLogger) *realLogger << value;\
		else {\
			auto& info = getThreadInfo();\
			u32 ensureBytes = ENSURE;\
			char* buf = info.ensure(ensureBytes);\
			if (buf) {\
				int used = sprintf(buf,FORMAT,(TYPE)value);\
				info.use(used);\
			} else {\
				printf("[Logger] : Failed insuring %u bytes\n", ensureBytes);\
			}\
		}\
		return *this;\
	}
	GEN_LOG_NUM(void*, 18, "%p")
	GEN_LOG_NUM(i64, 21, "%lld")
	GEN_LOG_NUM(u64, 20, "%llu")
	GEN_LOG_NUM(i32, 11, "%d")
	GEN_LOG_NUM(u32, 10, "%u")
	GEN_LOG_NUM(i16, 6, "%hd")
	GEN_LOG_NUM(u16, 5, "%hu")
	GEN_LOG_NUM(u8, 3, "%hu")
	GEN_LOG_NUM(double, 27, "%lf")
	GEN_LOG_NUM(float, 20, "%.5f")

#define GEN_LOG_GEN(TYPE,ENSURE,PRINT)\
	Logger& Logger::operator<<(TYPE value) {\
		if(realLogger) *realLogger<<value;\
		else {\
			auto& info = getThreadInfo();\
			u32 ensureBytes = ENSURE;\
			char* buf = info.ensure(ensureBytes);\
			if (buf) {\
				int used = sprintf PRINT;\
				info.use(used);\
			} else {\
				printf("[Logger] : Failed insuring %u bytes\n", ensureBytes);\
			}\
		}\
		return *this;\
	}
#ifdef ENGONE_GLM
	GEN_LOG_GEN(const glm::vec3&,60,(buf,"[%f, %f, %f]",value.x,value.y,value.z))
	GEN_LOG_GEN(const glm::vec4&,80,(buf,"[%f, %f, %f, %f]",value.x,value.y,value.z,value.w))
	GEN_LOG_GEN(const glm::quat&,60,(buf,"[%f, %f, %f, %f]", value.x, value.y, value.z, value.w))
	GEN_LOG_GEN(const glm::mat4&,60,(buf,
		"[%f %f %f %f]\n[%f %f %f %f]\n[%f %f %f %f]\n[%f %f %f %f]",
		value[0].x, value[0].y, value[0].z, value[0].w,
		value[1].x, value[1].y, value[1].z, value[1].w,
		value[2].x, value[2].y, value[2].z, value[2].w,
		value[3].x, value[3].y, value[3].z, value[3].w))
#endif // ENGONE_GLM

#ifdef ENGONE_PHYSICS
	GEN_LOG_GEN(const rp3d::Vector3&, 60, (buf, "[%f, %f, %f]", value.x, value.y, value.z))
	GEN_LOG_GEN(const rp3d::Quaternion&, 60, (buf, "[%f, %f, %f, %f]", value.x, value.y, value.z, value.w))
#endif // ENGONE_PHYSICS
}

