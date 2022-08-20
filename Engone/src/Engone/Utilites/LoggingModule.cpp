#include "Engone/Utilities/LoggingModule.h"

#ifdef ENGONE_PHYSICS
#include "Engone/Utilities/rp3d.h"
#endif
#ifdef ENGONE_TRACKER
#include "Engone/Utilities/Tracker.h"
#endif

namespace engone {
	// Tracker.h can't exist in LoggingModule.h because Tracker.h includes it. So variable is put here.
	//static TrackerId logReportId = "LogReport";
	// function is taken from Utilitiy.h, instead of including that i have copied the function here so that
	// the LoggingModule easily can be moved to another project.
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
	void LogReport::mayPrintTime() {
		if (!doPrintTime) return;

		int length = 11;
 		char* bytes = nullptr;
		if (m_size + length > m_maxSize) {
			if (resize((m_maxSize + length) * 2))
				bytes = m_data + m_size;
				//return nullptr;
		} else {
			bytes = m_data + m_size;
		}
 		//char* bytes = ensure(11); // can't use ensure since it calls mayPrint
		if (!bytes) return; // memory is bad

		bytes[0] = '[';
		_GetClock(bytes + 1);
		bytes[9] = ']';
		bytes[10] = ' ';
		usedSpace(11);
		doPrintTime = false;
	}
	void LogReport::printTimeNext() {
		doPrintTime = true;
	}
	void LogReport::add(const char* str, uint32_t length) {
		if (length == 0) length = strlen(str);

		//mayPrintTime();

		char* bytes = ensure(length);

		memcpy_s(bytes, m_maxSize - m_size, str, length);
		usedSpace(length);

		if (str[length - 1] == '\n')
			printTimeNext();
	}
	bool LogReport::resize(uint32_t size) {
		if (size == 0) {
			free(m_data);
			//GetTracker().subMemory(logReportId,m_maxSize);
			m_data = nullptr;
			m_size = 0;
			m_maxSize = 0;
		}
		if (!m_data) {
			char* data = (char*)malloc(size);
			if (!data) return false;
			//GetTracker().addMemory(logReportId, size);
			m_data = data;
			m_maxSize = size;
			m_size = 0;
		} else {
			char* data = (char*)realloc(m_data, size);
			if (!data) return false;
			//GetTracker().subMemory(logReportId, m_maxSize);
			//GetTracker().addMemory(logReportId, size);
			m_data = data;
			m_maxSize = size;
			if (m_size > m_maxSize) {
				m_size = m_maxSize;
			}
		}
		return true;
	}
	bool LogReport::saveReport(const char* filepath) {
		if (!m_data) return true; // return true because we successfully wrote nothing.

		if (filepath == nullptr)
			filepath = name.c_str();

		std::ofstream file(filepath, std::ios::binary);
		if (!file.is_open()) {
			return false;
		}
		file.write(m_data,m_size);
		file.close();
		return true;
	}
	void LogReport::cleanup() {
		resize(0);
	}
	char* LogReport::ensure(uint32_t length) {
		mayPrintTime();
		if (m_size+ length > m_maxSize) {
			if (!resize((m_maxSize+length)*2))
				return nullptr;
		}
		return m_data+m_size;
	}
	void LogReport::usedSpace(uint32_t length) {
		m_size += length;
	}
	namespace log {
		Filter operator|(Filter a, Filter b) {
			return (Filter)((uint32_t)a | (uint32_t)b);
		}
		Filter operator&(Filter a, Filter b) {
			return (Filter)((uint32_t)a & (uint32_t)b);
		}
		Filter operator^(Filter a, Filter b) {
			return (Filter)((uint32_t)a ^ (uint32_t)b);
		}
		Logger out;
	}
	void Logger::setColor(log::Color color) {
		if (m_consoleHandle == NULL) {
			m_consoleHandle = GetStdHandle(-11);
			if (m_consoleHandle == NULL) return;
		}
		if (m_color == color) return;
		m_color = color;
		SetConsoleTextAttribute(m_consoleHandle, color);
	}
	Logger& Logger::operator<<(const char* value) {
		LogReport& report = getSelected();
		if (reportMatch())
			report.add(value,0);
		if (consoleMatch())
			printf("%s", value);
		return *this;
	}
	Logger& Logger::operator<<(char* value) {
		return *this<<(const char*)value;
	}
	Logger& Logger::operator<<(void* value) {
		LogReport& report = getSelected();

		char* str = report.ensure(50);
		if (str) {
			int len = sprintf(str, "%p", value);
			if (consoleMatch())
				printf("%s", str);
			if (reportMatch())
				report.usedSpace(len); // is 0 char included?
		}
		return *this;
	}
	Logger& Logger::operator<<(const std::string& value) {
		LogReport& report = getSelected();
		if (reportMatch())
		report.add(value.c_str(), value.length());
		if (consoleMatch())
			printf("%s", value.c_str());
		return *this;
	}
	Logger& Logger::operator<<(int64_t value) {
		LogReport& report = getSelected();
		char* str = report.ensure(50);
		if (str) {
			int len = sprintf(str, "%ld", (long int)value);
			if(consoleMatch())
				printf("%s", str);
			if(reportMatch())
				report.usedSpace(len); // is 0 char included?
		}
		return *this;
	}
	Logger& Logger::operator<<(uint64_t value) {
		LogReport& report = getSelected();

		char* str = report.ensure(50);
		if (str) {
			int len = sprintf(str, "%lu", (unsigned long int)value);
			if (consoleMatch())
			printf("%s", str);
			if (reportMatch())
			report.usedSpace(len); // is 0 char included?
		}
		return *this;
	}
	Logger& Logger::operator<<(int32_t value) {
		LogReport& report = getSelected();

		char* str = report.ensure(50);
		if (str) {
			int len = sprintf(str, "%d", value);
			if (consoleMatch())
				printf("%s",str);
			if (reportMatch())
				report.usedSpace(len); // is 0 char included?
		}
		return *this;
	}
	Logger& Logger::operator<<(uint32_t value) {
		LogReport& report = getSelected();

		char* str = report.ensure(50);
		if (str) {
			int len = sprintf(str, "%u", value);
			if (consoleMatch())
			printf("%s", str);
			if (reportMatch())
			report.usedSpace(len); // is 0 char included?
		}
		return *this;
	}
	Logger& Logger::operator<<(uint16_t value) {
		LogReport& report = getSelected();

		char* str = report.ensure(50);
		if (str) {
			int len = sprintf(str, "%hd", value);
			if (consoleMatch())
			printf("%s", str);
			if (reportMatch())
			report.usedSpace(len); // is 0 char included?
		}
		return *this;
	}
	Logger& Logger::operator<<(uint16_t value) {
		LogReport& report = getSelected();

		char* str = report.ensure(50);
		if (str) {
			int len = sprintf(str, "%hu", value);
			if (consoleMatch())
			printf("%s", str);
			if (reportMatch())
			report.usedSpace(len); // is 0 char included?
		}
		return *this;
	}
	Logger& Logger::operator<<(char value) {
		LogReport& report = getSelected();

		char* str = report.ensure(50);
		if (str) {
			str[0] = value;
			//int len = sprintf(str, "%u", value);
			if (consoleMatch())
			printf("%c", value);
			if (reportMatch())
			report.usedSpace(1); // is 0 char included?
		}
		return *this;
	}
	Logger& Logger::operator<<(uint8_t value) {
		LogReport& report = getSelected();

		char* str = report.ensure(50);
		if (str) {
			int len = sprintf(str, "%hhu", value);
			if (consoleMatch())
			printf("%s", str);
			if (reportMatch())
			report.usedSpace(len); // is 0 char included?
		}
		return *this;
	}
	Logger& Logger::operator<<(double value) {
		LogReport& report = getSelected();

		char* str = report.ensure(50);
		if (str) {
			int len = sprintf(str, "%lf", value);
			if (consoleMatch())
				printf("%s", str);
			if (reportMatch())
				report.usedSpace(len); // is 0 char included?
		}
		return *this;
	}
	Logger& Logger::operator<<(float value) {
		LogReport& report = getSelected();

		char* str = report.ensure(50);
		if (str) {
			int len = sprintf(str, "%f", value);
			if (consoleMatch())
				printf("%s", str);
			if (reportMatch())
				report.usedSpace(len); // is 0 char included?
		}
		return *this;
	}
#ifdef ENGONE_GLM
	Logger& Logger::operator<<(const glm::vec3& value) {
		LogReport& report = getSelected();

		char* str = report.ensure(50*3);
		if (str) {
			int len = sprintf(str, "[%f %f %f]", value.x,value.y,value.z);
			if (consoleMatch())
				printf("%s", str);
			if (reportMatch())
				report.usedSpace(len); // is 0 char included?
		}
		return *this;
	}
	Logger& Logger::operator<<(const glm::vec4& value) {
		LogReport& report = getSelected();

		char* str = report.ensure(50 * 4);
		if (str) {
			int len = sprintf(str, "[%f %f %f %f]", value.x, value.y, value.z, value.w);
			if (consoleMatch())
				printf("%s", str);
			if (reportMatch())
				report.usedSpace(len); // is 0 char included?
		}
		return *this;
	}
	Logger& Logger::operator<<const glm::quat& value) {
		LogReport& report = getSelected();

		char* str = report.ensure(50 * 4);
		if (str) {
			int len = sprintf(str, "[%f %f %f %f]", value.x, value.y, value.z, value.w);
			if (consoleMatch())
				printf("%s", str);
			if (reportMatch())
				report.usedSpace(len); // is 0 char included?
		}
		return *this;
	}
	Logger& Logger::operator<<(const glm::mat4& value) {
		LogReport& report = getSelected();

		char* str = report.ensure(50 * 16);
		if (str) {
			int len = sprintf(str, "[%f %f %f %f]\n[%f %f %f %f]\n[%f %f %f %f]\n[%f %f %f %f]", 
				value[0].x, value[0].y, value[0].z, value[0].w,
				value[1].x, value[1].y, value[1].z, value[1].w,
				value[2].x, value[2].y, value[2].z, value[2].w,
				value[3].x, value[3].y, value[3].z, value[3].w
				);
			if (consoleMatch())
				printf("%s", str);
			if (reportMatch())
				report.usedSpace(len); // is 0 char included?
		}
		return *this;
	}
#endif
#ifdef ENGONE_PHYSICS
	Logger& Logger::operator<<(const rp3d::Vector3& value) {
		LogReport& report = getSelected();

		char* str = report.ensure(50 * 3);
		if (str) {
			int len = sprintf(str, "[%f %f %f]", value.x, value.y, value.z);
			if (consoleMatch())
				printf("%s", str);
			if (reportMatch())
				report.usedSpace(len); // is 0 char included?
		}
		return *this;
	}
	Logger& Logger::operator<<(const rp3d::Quaternion& value) {
		LogReport& report = getSelected();

		char* str = report.ensure(50 * 4);
		if (str) {
			int len = sprintf(str, "[%f %f %f %f]", value.x, value.y, value.z, value.w);
			if (consoleMatch())
				printf("%s", str);
			if (reportMatch())
				report.usedSpace(len); // is 0 char included?
		}
		return *this;
	}
#endif
}