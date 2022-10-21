#ifdef ENGONE_LOGGER  // ENGONE_LOGGER

#include "Engone/Utilities/LoggingModule.h"

#ifdef ENGONE_TRACKER
// tracker is required when doing implementation
#include "Engone/Utilities/Tracker.h"
#endif
namespace engone {
	// Tracker.h can't exist in LoggingModule.h because Tracker.h includes it. So variable is put here.
	//#ifdef ENGONE_TRACKER
	//static TrackerId logReportId = "LogReport";
	//#endif
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
		Filter operator~(Filter a) {
			return (Filter)(~(uint32_t)a);
		}
		Filter operator|=(Filter& a, Filter b) {
			a = a | b;
			return a;
		}
		Filter operator&=(Filter& a, Filter b) {
			a = a & b;
			return a;
		}
		Filter operator^=(Filter& a, Filter b) {
			a = a ^ b;
			return a;
		}
		Logger out;
	}
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
		if (!doPrintTime || m_disableTime) return;

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
	//void LogReport::add(const char* str, uint32_t length) {
	//	if (length == 0) length = strlen(str);

	//	char* bytes = ensure(length);

	//	memcpy_s(bytes, m_maxSize - m_size, str, length);
	//	usedSpace(length);

	//	if (str[length - 1] == '\n')
	//		printTimeNext();
	//}
	bool LogReport::resize(uint32_t size) {
		//m_mutex.lock();
		bool succ = true;
		if (size == 0) {
			free(m_data);
			//#ifdef ENGONE_TRACKER
					//GetTracker().subMemory(logReportId,m_maxSize);
			//#endif
			m_data = nullptr;
			m_size = 0;
			m_maxSize = 0;
		}
		if (!m_data) {
			char* data = (char*)malloc(size);
			if (data) {
				//#ifdef ENGONE_TRACKER
						//GetTracker().addMemory(logReportId, size);
				//#endif
				m_data = data;
				m_maxSize = size;
				m_size = 0;
			} else {
				succ = false;
			}
		} else {
			char* data = (char*)realloc(m_data, size);
			if (data) {
				//#ifdef ENGONE_TRACKER
						//GetTracker().subMemory(logReportId, m_maxSize);
						//GetTracker().addMemory(logReportId, size);
				//#endif
				m_data = data;
				m_maxSize = size;
				if (m_size > m_maxSize) {
					m_size = m_maxSize;
				}
			} else {
				succ = false;
			}
		}
		//m_mutex.unlock();
		return succ;
	}
	bool LogReport::saveReport(const char* filepath) {
		bool succ = true;
		//m_mutex.lock();
		if (m_data) {
			if (filepath == nullptr)
				filepath = name.c_str();

			std::ofstream file(filepath, std::ios::binary); // could use fopen.
			if (file.is_open()) {
				file.write(m_data, m_size);
				file.close();
			} else {
				succ = false;
			}
		}
		//m_mutex.unlock();
		return succ;
	}
	void LogReport::cleanup() {
		//m_mutex.lock();
		resize(0);
		//m_mutex.unlock();
	}
	char* LogReport::ensure(uint32_t length) {
		//m_mutex.lock();
		char* out = m_data + m_size;
		mayPrintTime();
		if (m_size + length > m_maxSize) {
			if (!resize((m_maxSize + length) * 2))
				return nullptr;
		}
		out = m_data + m_size;
		//m_mutex.unlock();
		return out;
	}
	void LogReport::usedSpace(uint32_t length) {
		m_size += length;
	}
	void Logger::setColor(log::Color color) {
		//bool b = m_mutex.try_lock();
		if (m_consoleHandle == NULL) {
			m_consoleHandle = GetStdHandle(-11);
			if (m_consoleHandle == NULL) return;
		}
		if (m_color == color) return;
		m_color = color;
		SetConsoleTextAttribute(m_consoleHandle, color);
		//if(b)
		//	m_mutex.unlock();
	}
	void Logger::disableTime(bool yes) {
		//m_mutex.lock();
		for (int i = 0; i < MAX_REPORTS; i++) {
			LogReport& report = m_reports[i];
			report.disableTime(yes);
		}
		//m_mutex.unlock();
	}
	void Logger::lock() {
		if (mutexPerLine) {
			if (m_threadId != std::this_thread::get_id()) {
				m_mutex.lock();
				m_threadId = std::this_thread::get_id();
			}
		} else {
			m_mutex.lock();
		}
	}
	void Logger::unlock(bool newLine) {
		if (mutexPerLine) {
			if (newLine) {
				m_threadId = {};
				m_mutex.unlock();
			}
		} else {
			m_mutex.unlock();
		}
	}
	void Logger::print(char* str, int len) {
		//m_mutex.lock();
		LogReport& report = getSelected();
		if (consoleMatch())
			printf("%s", str);
		if (reportMatch())
			report.usedSpace(len); // is 0 char included?
		//m_mutex.unlock();
	}
	Logger& Logger::operator<<(const char* value) {
		int len = strlen(value);
		if (len != 0) {
			lock();

			LogReport& report = getSelected();
			char* str = report.ensure(len + 1);
			memcpy_s(str, len, value, len);
			str[len] = 0;
			print(str, len);
			//if (reportMatch())
			//	report.add(value, 0);
			//if (consoleMatch())
			//	printf("%s", value);
			bool newLine = value[len - 1] == '\n';
			if (newLine) {
				setColor(log::SILVER);
				report.printTimeNext();
			}
			unlock(newLine);
		}
		return *this;
	}
	Logger& Logger::operator<<(char* value) {
		return *this << (const char*)value;
	}
	Logger& Logger::operator<<(void* value) {
		lock();
		LogReport& report = getSelected();
		char* str = report.ensure(50);
		if (str) {
			int len = sprintf(str, "%p", value);
			print(str, len);
			//if (consoleMatch())
			//	printf("%s", str);
			//if (reportMatch())
			//	report.usedSpace(len); // is 0 char included?
		}
		unlock();
		return *this;
	}
	Logger& Logger::operator<<(const std::string& value) {
		int len = value.length();
		if (len != 0) {
			lock();

			LogReport& report = getSelected();

			char* str = report.ensure(len + 1);
			memcpy_s(str, len, value.c_str(), len);
			str[len] = 0;
			print(str, len);
			//if (reportMatch())
			//	report.add(value.c_str(), value.length());
			//if (consoleMatch())
			//	printf("%s", value.c_str());
			bool newLine = value.back() == '\n';
			if (newLine) {
				setColor(log::SILVER);
				report.printTimeNext();
			}
			unlock(newLine);
		}
		return *this;
	}
	//Logger& Logger::operator<<(std::string value) {
	//	return *this<<(const std::string&)value;
	//}
	Logger& Logger::operator<<(int64_t value) {
		lock();
		LogReport& report = getSelected();
		char* str = report.ensure(50);
		if (str) {
			int len = sprintf(str, "%ld", (long int)value);
			print(str, len);
			//if (consoleMatch())
			//	printf("%s", str);
			//if (reportMatch())
			//	report.usedSpace(len); // is 0 char included?
		}
		unlock();
		return *this;
	}
	Logger& Logger::operator<<(uint64_t value) {
		lock();
		LogReport& report = getSelected();
		char* str = report.ensure(50);
		if (str) {
			int len = sprintf(str, "%lu", (unsigned long int)value);
			print(str, len);
			//if (consoleMatch())
			//	printf("%s", str);
			//if (reportMatch())
			//	report.usedSpace(len); // is 0 char included?
		}
		unlock();
		return *this;
	}
	Logger& Logger::operator<<(int32_t value) {
		lock();
		LogReport& report = getSelected();
		char* str = report.ensure(50);
		if (str) {
			int len = sprintf(str, "%d", value);
			print(str, len);
			//if (consoleMatch())
			//	printf("%s", str);
			//if (reportMatch())
			//	report.usedSpace(len); // is 0 char included?
		}
		unlock();
		return *this;
	}
	Logger& Logger::operator<<(uint32_t value) {
		lock();
		LogReport& report = getSelected();
		char* str = report.ensure(50);
		if (str) {
			int len = sprintf(str, "%u", value);
			print(str, len);
			//if (consoleMatch())
			//	printf("%s", str);
			//if (reportMatch())
			//	report.usedSpace(len); // is 0 char included?
		}
		unlock();
		return *this;
	}
	Logger& Logger::operator<<(int16_t value) {
		lock();
		LogReport& report = getSelected();
		char* str = report.ensure(50);
		if (str) {
			int len = sprintf(str, "%hd", value);
			print(str, len);
			//if (consoleMatch())
			//	printf("%s", str);
			//if (reportMatch())
			//	report.usedSpace(len); // is 0 char included?
		}
		unlock();
		return *this;
	}
	Logger& Logger::operator<<(uint16_t value) {
		lock();
		LogReport& report = getSelected();
		char* str = report.ensure(50);
		if (str) {
			int len = sprintf(str, "%hu", value);
			print(str, len);
			//if (consoleMatch())
			//	printf("%s", str);
			//if (reportMatch())
			//	report.usedSpace(len); // is 0 char included?
		}
		unlock();
		return *this;
	}
	Logger& Logger::operator<<(char value) {
		lock();
		LogReport& report = getSelected();
		char* str = report.ensure(5);
		if (str) {
			str[0] = value;
			print(str, 1);
			//int len = sprintf(str, "%u", value);
			if (value == '\n') {
				setColor(log::SILVER);
				report.printTimeNext();
			}
		}
		unlock(value == '\n');
		return *this;
	}
	Logger& Logger::operator<<(uint8_t value) {
		lock();
		LogReport& report = getSelected();
		char* str = report.ensure(50);
		if (str) {
			int len = sprintf(str, "%hhu", value);
			print(str, len);
			//if (consoleMatch())
			//	printf("%s", str);
			//if (reportMatch())
			//	report.usedSpace(len); // is 0 char included?
		}
		unlock();
		return *this;
	}
	Logger& Logger::operator<<(double value) {
		lock();
		LogReport& report = getSelected();
		char* str = report.ensure(50);
		if (str) {
			int len = sprintf(str, "%lf", value);
			print(str, len);
			//if (consoleMatch())
			//	printf("%s", str);
			//if (reportMatch())
			//	report.usedSpace(len); // is 0 char included?
		}
		unlock();
		return *this;
	}
	Logger& Logger::operator<<(float value) {
		lock();
		LogReport& report = getSelected();
		char* str = report.ensure(50);
		if (str) {
			int len = sprintf(str, "%f", value);
			print(str, len);
			//if (consoleMatch())
			//	printf("%s", str);
			//if (reportMatch())
			//	report.usedSpace(len); // is 0 char included?
		}
		unlock();
		return *this;
	}
#ifdef ENGONE_GLM
	Logger& Logger::operator<<(const glm::vec3& value) {
		lock();
		LogReport& report = getSelected();
		char* str = report.ensure(50 * 3);
		if (str) {
			int len = sprintf(str, "[%f %f %f]", value.x, value.y, value.z);
			print(str, len);
			//if (consoleMatch())
			//	printf("%s", str);
			//if (reportMatch())
			//	report.usedSpace(len); // is 0 char included?
		}
		unlock();
		return *this;
	}
	Logger& Logger::operator<<(const glm::vec4& value) {
		lock();
		LogReport& report = getSelected();
		char* str = report.ensure(50 * 4);
		if (str) {
			int len = sprintf(str, "[%f %f %f %f]", value.x, value.y, value.z, value.w);
			print(str, len);
			//if (consoleMatch())
			//	printf("%s", str);
			//if (reportMatch())
			//	report.usedSpace(len); // is 0 char included?
		}
		unlock();
		return *this;
	}
	Logger& Logger::operator<<(const glm::quat& value) {
		lock();
		LogReport& report = getSelected();
		char* str = report.ensure(50 * 4);
		if (str) {
			int len = sprintf(str, "[%f %f %f %f]", value.x, value.y, value.z, value.w);
			print(str, len);
			//if (consoleMatch())
			//	printf("%s", str);
			//if (reportMatch())
			//	report.usedSpace(len); // is 0 char included?
		}
		unlock();
		return *this;
	}
	Logger& Logger::operator<<(const glm::mat4& value) {
		lock();
		LogReport& report = getSelected();
		char* str = report.ensure(50 * 16);
		if (str) {
			int len = sprintf(str, "[%f %f %f %f]\n[%f %f %f %f]\n[%f %f %f %f]\n[%f %f %f %f]",
				value[0].x, value[0].y, value[0].z, value[0].w,
				value[1].x, value[1].y, value[1].z, value[1].w,
				value[2].x, value[2].y, value[2].z, value[2].w,
				value[3].x, value[3].y, value[3].z, value[3].w
			);
			print(str, len);
			//if (consoleMatch())
			//	printf("%s", str);
			//if (reportMatch())
			//	report.usedSpace(len); // is 0 char included?
		}
		unlock();
		return *this;
	}
#endif // ENGONE_GLM
#ifdef ENGONE_PHYSICS
	Logger& Logger::operator<<(const rp3d::Vector3& value) {
		lock();
		LogReport& report = getSelected();
		char* str = report.ensure(50 * 3);
		if (str) {
			int len = sprintf(str, "[%f %f %f]", value.x, value.y, value.z);
			print(str, len);
			//if (consoleMatch())
			//	printf("%s", str);
			//if (reportMatch())
			//	report.usedSpace(len); // is 0 char included?
		}
		unlock();
		return *this;
	}
	Logger& Logger::operator<<(const rp3d::Quaternion& value) {
		lock();
		LogReport& report = getSelected();
		char* str = report.ensure(50 * 4);
		if (str) {
			int len = sprintf(str, "[%f %f %f %f]", value.x, value.y, value.z, value.w);
			print(str, len);
			//if (consoleMatch())
			//	printf("%s", str);
			//if (reportMatch())
			//	report.usedSpace(len); // is 0 char included?
		}
		unlock();
		return *this;
	}
#endif // ENGONE_PHYSICS
}
#endif // ENGONE_LOGGER