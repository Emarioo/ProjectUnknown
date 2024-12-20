#include "Engone/Logger.h"

// Temporary
//#include "Engone/Win32Includes.h"

#include "Engone/PlatformLayer.h"
#include "Engone/Asserts.h"

#include "Engone/Util/Stream.h"

#include <time.h>
#include <string.h>

#ifdef OS_LINUX
#include <unistd.h>
#endif

namespace engone {
	namespace log {
		Logger out;
	}
	// This function is copied from Utilitiy.h to make the logger more independent.
	// str should have space of 8 characters.
	static void _GetClock(char* str) {
		time_t t;
		time(&t);
		#ifdef OS_LINUX
		tm* tmp = localtime(&t);
		#else
		tm _tm;
		tm* tmp = &_tm;
		localtime_s(tmp, &t);
		#endif
		str[2] = ':';
		str[5] = ':';

		int temp = tmp->tm_hour / 10;
		str[0] = '0' + temp;
		temp = tmp->tm_hour - 10 * temp;
		str[1] = '0' + temp;

		temp = tmp->tm_min / 10;
		str[3] = '0' + temp;
		temp = tmp->tm_min - 10 * temp;
		str[4] = '0' + temp;

		temp = tmp->tm_sec / 10;
		str[6] = '0' + temp;
		temp = tmp->tm_sec - 10 * temp;
		str[7] = '0' + temp;
	}
	void Logger::cleanup() {
		flush();
		
		for (auto& pair : m_threadInfos) {
			// pair.second.lineBuffer.resize(0);
            Free(pair.second.line_buffer_data, pair.second.line_buffer_max);
            pair.second.line_buffer_data = nullptr;
            pair.second.line_buffer_max = 0;
            pair.second.line_buffer_used = 0;
		}
		m_threadInfos.clear();
		for(auto& pair : m_logFiles){
			FileClose(pair.second);
		}
		m_logFiles.clear();
	}
	char* Logger::ThreadInfo::ensure(u32 bytes) {
		if (line_buffer_max < line_buffer_used + bytes + 1) { // +1 for \0
            int max = line_buffer_max*2 + 2*bytes + 1;
            auto new_data = (char*)Reallocate(line_buffer_data, line_buffer_max, max);
            if(!new_data)
                return nullptr;
            line_buffer_max = max;
            line_buffer_data = new_data;
			// bool yes = lineBuffer.resize(max);
			// if (!yes)
			// 	return nullptr;
		}
		return ((char*)line_buffer_data + line_buffer_used);
	}
	void Logger::ThreadInfo::use(u32 bytes) {
		line_buffer_used += bytes;
		*((char*)line_buffer_data + line_buffer_used) = 0;
		//printf("%s",lineBuffer.data+line_buffer_used);
	}
	void Logger::enableReport(bool yes){
		m_enabledReports = yes;
	}
	void Logger::enableConsole(bool yes){
		m_enabledConsole = yes;
	}
    bool Logger::isEnabledReport(){
		return m_enabledReports;
	}
	bool Logger::isEnabledConsole(){
		return m_enabledConsole;
	}
	void Logger::setMasterColor(log::Color color){
		m_masterColor = color;
	}
	void Logger::setMasterReport(const std::string path){
		m_masterReportPath = path;
	}
	void Logger::setReport(const std::string path){
		auto& info = getThreadInfo();
		info.logReport = path;
	}
	void Logger::setIndent(int indent){
		m_indent = indent;
		if(onEmptyLine){
			m_nextIndent = indent;
		}
	}
	void Logger::useThreadReports(bool yes){
		m_useThreadReports=yes;
	}
	Logger::ThreadInfo& Logger::getThreadInfo() {
		auto id = Thread::GetThisThreadId();
		auto find = m_threadInfos.find(id);
		if (find == m_threadInfos.end()) {
			m_threadInfos[id] = {};
		}
		return m_threadInfos[Thread::GetThisThreadId()];
	}
	void Logger::flush(){
		auto& info = getThreadInfo();
		if(info.line_buffer_used==0)
			return;
		*((char*)info.line_buffer_data+info.line_buffer_used) = 0;
		
		char* str = (char*)info.line_buffer_data;
		int len = info.line_buffer_used;
		// print((char*)info.lineBuffer.data, info.line_buffer_used);

		// For debugging
		// for(int i=0;i<len;i++){
		// 	char chr = str[i];
		// 	if(chr == '\n') {

		// 		int k = 232;
		// 	}
		// }

		auto outFile = engone::GetStandardOut();

		m_printMutex.lock();
		if (m_masterColor == log::NO_COLOR) {
            #ifdef OS_WINDOWS
            if(info.color == log::NO_COLOR) {
			    SetConsoleColor(log::SILVER);
            } else {
			    SetConsoleColor(info.color);
            }
            #else
			SetConsoleColor(info.color);
            #endif
        } else
			SetConsoleColor(m_masterColor);

		// fprintf(stdout,"\033[1;32m{YA}");
		// fflush(stdout);
		// int fdad = STDOUT_FILENO;
		// const char* strar = "\033[1;32mEyo!\n";
		// write(fdad,strar,strlen(strar));
		// printf("%s",str);

		// printf("\033[1,10m{yo}");
		// write()
		// engone::FileWrite(outFile, "\033[1;32m{10}",7+4);
        if(m_streamInput) {
            m_streamInput->write(str, len);
        }
		if(m_enabledConsole){
            // Do we use the indent feature?
			// if(lastPrintedChar=='\n'){
			// // 	preIndent=false;
			// 	for(int j=0;j<m_nextIndent;j++) {
			// 		engone::FileWrite(outFile, " ", 1);
			// 		// fwrite(" ",1, 1, stdout);
			// 	}
			// 	m_nextIndent = m_indent;
			// }
			int last = 0;
			for(int i=0;i<len;i++){
				if(str[i]=='\n'||i+1==len){
					engone::FileWrite(outFile, str + last, i+1-last);

					// fwrite(str+last,1,i+1-last,stdout);
					last = i+1;
					// if(str[i]=='\n' && i+1!=len){
					// 	for(int j=0;j<m_indent;j++) {
					// 		engone::FileWrite(outFile, " ", 1);
					// 		// fwrite(" ",1, 1, stdout);
					// 	}
					// }
				}
			}
			lastPrintedChar = str[len-1];
			if(lastPrintedChar=='\n') {
				onEmptyLine=true;
			}
		}
		if(m_enabledReports){
			const char* te = "[Thread 65536] ";
			char extraBuffer[11+15+1]{0};
			bool printExtra=false;
			if(printExtra){
				extraBuffer[0]='[';
				extraBuffer[9]=']';
				extraBuffer[10]=' ';
				_GetClock(extraBuffer+1);
                #ifdef OS_WINDOWS
				sprintf(extraBuffer+11,"[Thread %llu] ",(u64)Thread::GetThisThreadId());
                #else
				sprintf(extraBuffer+11,"[Thread %lu] ",(u64)Thread::GetThisThreadId());
                #endif
			}
			if(!m_masterReportPath.empty()){
				std::string path = m_rootDirectory+"/"+m_masterReportPath;
				auto find = m_logFiles.find(path);
				APIFile file={};
				if(find==m_logFiles.end()){
					file = engone::FileOpen(path,FILE_CLEAR_AND_WRITE);
					if(file)
						m_logFiles[path] = file;
				}else{
					file = find->second;
				}
				if(file){
					if(printExtra)
						engone::FileWrite(file,extraBuffer,strlen(extraBuffer));
					u64 bytes = engone::FileWrite(file,str,len);
					// TODO: check for failure
				}
			}
			if(!info.logReport.empty()){
				std::string path = m_rootDirectory+"/"+info.logReport;
				auto find = m_logFiles.find(path);
				APIFile file={};
				if(find==m_logFiles.end()){
					// file = FileOpen(path,0,FILE_CAN_CREATE);
					file = engone::FileOpen(path,FILE_CLEAR_AND_WRITE);
					if(file)
						m_logFiles[path] = file;
				}else{
					file = find->second;
				}
				if(file){
					if(printExtra)
						engone::FileWrite(file,extraBuffer,strlen(extraBuffer));
					u64 bytes = engone::FileWrite(file,str,len);
					// TODO: check for failure
				}
			}
			if(m_useThreadReports){
				std::string path = m_rootDirectory+"/thread";
				path += std::to_string((u32)Thread::GetThisThreadId())+".txt";
				auto find = m_logFiles.find(path);
				APIFile file={};
				if(find==m_logFiles.end()){
					// file = FileOpen(path,0,FILE_CAN_CREATE);
					file = engone::FileOpen(path,FILE_CLEAR_AND_WRITE);
					if(file)
						m_logFiles[path] = file;
				}else{
					file = find->second;
				}
				if(file){
					if(printExtra)
						engone::FileWrite(file,extraBuffer,strlen(extraBuffer));
					u64 bytes = engone::FileWrite(file,str,len);
					// TODO: check for failure
				}
			}
		}
		m_printMutex.unlock();
		#ifdef OS_WINDOWS
		info.color = log::SILVER; // reset color after new line
		if (m_masterColor == log::NO_COLOR)
			SetConsoleColor(log::SILVER);
		#else
		info.color = log::NO_COLOR; // reset color after new line
		if (m_masterColor == log::NO_COLOR)
			SetConsoleColor(log::NO_COLOR);
		#endif
		// TODO: write to report

		info.line_buffer_used = 0; // flush buffer
	}
	u64 Logger::getMemoryUsage(){
		u64 sum=0;
		for(auto& pair : m_threadInfos){
			sum+=pair.second.line_buffer_max;
		}
		return sum;
	}
	void Logger::flushInternal() {
		m_printMutex.lock();
		for(auto& pair : m_logFiles){
			engone::FileFlushBuffers(pair.second);
		}
		m_printMutex.unlock();
		engone::FileFlushBuffers(engone::GetStandardOut());
	}
	void Logger::print(char* str, int len, bool no_warning) {
		Assert(str);
		if (len == 0) return;
		
		auto& info = getThreadInfo();
		if(len > 0x1000 && !no_warning) {
			log::out << log::YELLOW << "Logger: Printing and ensuring a lot of bytes ("<<len<<"). You flush the bytes multiple times!\n";
		}
		// large
		// TODO: If len is really large then we do some extra flushes so
		//  we don't need to ensure megabytes of data.
		char* buf = info.ensure(len);
		if (buf) {
            written_bytes += len;
			memcpy(buf, str, len);
			info.use(len);
			onEmptyLine=false;
		} else {
			printf("[Logger] : Failed ensuring %u bytes\n", len); \
		}
		if (str[len - 1] == '\n') {
			flush();
		}
	}
	log::Color Logger::getColor() {
		auto& info = getThreadInfo();
		if (m_masterColor == log::NO_COLOR)
			return info.color;
		else
			return m_masterColor;
	}
	Logger& Logger::operator<<(log::Color value) {
		auto& inf = getThreadInfo();
		if(inf.color!=value){
			if(inf.line_buffer_used){
				flush();
			}
			inf.color = value;
		}
		return *this;
	}
	Logger& Logger::operator<<(log::Filter value) {
		getThreadInfo().filter = value;
		return *this;
	}
	Logger& Logger::operator<<(const char* value) {
		u32 len = strlen(value);

		if (len == 0) return *this;

		print((char*)value, len);
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

	// Am I crazy, lazy or smart?
#define GEN_LOG_NUM(TYPE,ENSURE,FORMAT)\
	Logger& Logger::operator<<(TYPE value) {\
		auto& info = getThreadInfo();\
		u32 ensureBytes = ENSURE;\
		char* buf = info.ensure(ensureBytes);\
		if (buf) {\
			int used = sprintf(buf,FORMAT,(TYPE)value);\
            written_bytes += used;\
			info.use(used);\
			onEmptyLine=false;\
		} else {\
			printf("[Logger] : Failed ensuring %u bytes\n", ensureBytes);\
		}\
		return *this;\
	}

	GEN_LOG_NUM(void*, 18, "%p")
	GEN_LOG_NUM(i64, 21, FORMAT_64"d")
	GEN_LOG_NUM(u64, 20, FORMAT_64"u")
	GEN_LOG_NUM(i32, 11, "%d")
	GEN_LOG_NUM(u32, 10, "%u")
	GEN_LOG_NUM(i16, 6, "%hd")
	GEN_LOG_NUM(u16, 5, "%hu")
	GEN_LOG_NUM(u8, 3, "%hu")
	// GEN_LOG_NUM(volatile long, 11, "%d")
	GEN_LOG_NUM(double, 27, "%.2lf")
	GEN_LOG_NUM(float, 20, "%.2f")

#define GEN_LOG_GEN(TYPE,ENSURE,PRINT)\
	Logger& Logger::operator<<(TYPE value) {\
		auto& info = getThreadInfo();\
		u32 ensureBytes = ENSURE;\
		char* buf = info.ensure(ensureBytes);\
		if (buf) {\
			int used = sprintf PRINT;\
            written_bytes += used;\
			info.use(used);\
			onEmptyLine=false;\
		} else {\
			printf("[Logger] : Failed ensuring %u bytes\n", ensureBytes);\
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

