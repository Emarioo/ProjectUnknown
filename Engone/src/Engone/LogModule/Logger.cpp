#include "Engone/LogModule/Logger.h"

#include "Engone/Win32Includes.h"

namespace engone {
	namespace log {
		Logger out;
	}
	void Logger::cleanup() {

		for (auto& [k, v] : m_threadInfos) {
			v.lineBuffer.resize(0);
		}
	}
	char* Logger::ThreadInfo::ensure(uint32 bytes) {
		if (lineBuffer.max < lineBuffer.used + bytes + 1) {
			// Todo: increase by max*2x instead of used+bytes?
			bool yes = lineBuffer.resize(lineBuffer.used + bytes + 1);
			if (!yes) return nullptr;
		}
		return lineBuffer.data + lineBuffer.used;
	}
	void Logger::ThreadInfo::use(uint32 bytes) {
		lineBuffer.used += bytes;
		lineBuffer.data[lineBuffer.used] = 0;
		//printf("%s",lineBuffer.data+lineBuffer.used);
	}
	Logger::ThreadInfo& Logger::getThreadInfo() {
			auto id = Thread::GetThisThreadId();
			auto find = m_threadInfos.find(id);
			if (find == m_threadInfos.end()) {
				m_threadInfos[id] = {};
;			}
			return m_threadInfos[Thread::GetThisThreadId()];
		}
	void Logger::print(char* str, int len) {
		auto& info = getThreadInfo();
		m_printMutex.lock();
		if (m_consoleHandle == NULL) {
			m_consoleHandle = GetStdHandle(-11);
			if (m_consoleHandle == NULL) return;
		}
		// Todo: don't set color if already set
		if (info.masterColor == log::NO_COLOR)
			SetConsoleTextAttribute((HANDLE)m_consoleHandle, info.color);
		else
			SetConsoleTextAttribute((HANDLE)m_consoleHandle, info.masterColor);
		printf("%s", str);
		m_printMutex.unlock();

		info.color = log::SILVER;

		// write to report
	}
	log::Color Logger::getColor() {
		auto& info = getThreadInfo();
		if (info.masterColor == log::NO_COLOR)
			return info.color;
		else
			return info.masterColor;
	}
	Logger& Logger::operator<<(log::Color value) {
		getThreadInfo().color = value;
		return *this;
	}
	Logger& Logger::operator<<(log::Filter value) {
		getThreadInfo().filter = value;
		return *this;
	}
	Logger& Logger::operator<<(const char* value) {
		uint32 len = strlen(value);

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
			print(info.lineBuffer.data, info.lineBuffer.used);
			info.lineBuffer.used = 0; // flush buffer
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

	// I am lazy (or smart?)
#define GEN_LOG_NUM(TYPE,ENSURE,FORMAT)\
	Logger& Logger::operator<<(TYPE value) {\
		auto& info = getThreadInfo();\
		uint32 ensureBytes = ENSURE;\
		char* buf = info.ensure(ensureBytes);\
		if (buf) {\
			int used = sprintf(buf,FORMAT,(TYPE)value);\
			info.use(used);\
		} else {\
			printf("[Logger] : Failed insuring %u bytes\n", ensureBytes);\
		}\
		return *this;\
	}
	GEN_LOG_NUM(void*, 18, "%p")
	GEN_LOG_NUM(int64, 21, "%lld")
	GEN_LOG_NUM(uint64, 20, "%llu")
	GEN_LOG_NUM(int32, 11, "%d")
	GEN_LOG_NUM(uint32, 10, "%u")
	GEN_LOG_NUM(int16, 6, "%hd")
	GEN_LOG_NUM(uint16, 5, "%hu")
	GEN_LOG_NUM(uint8, 3, "%hu")
	GEN_LOG_NUM(double, 27, "%lf")
	GEN_LOG_NUM(float, 20, "%.5f")

#define GEN_LOG_GEN(TYPE,ENSURE,PRINT)\
	Logger& Logger::operator<<(TYPE value) {\
		auto& info = getThreadInfo();\
		uint32 ensureBytes = ENSURE;\
		char* buf = info.ensure(ensureBytes);\
		if (buf) {\
			int used = sprintf PRINT;\
			info.use(used);\
		} else {\
			printf("[Logger] : Failed insuring %u bytes\n", ensureBytes);\
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

