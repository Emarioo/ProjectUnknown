
/* Error/log system
	Option to write all logs to a file (with a date)
	Choose multiple places to write those logs.
	Choose to keep backups of all logs (like the start data of the program)
	The .txt log acts as a kind of crashreport.
	The game doesn't have extensive errors. Stacktrace like in Java doesn't quite work the same in C++.
	Info, Warning, Error levels. More ones?

	log::out is the Master Logger class. Or maybe out is an actor of the master. Full functionality comes from GetLogger()	
*/

// glm is included in precompiled header


#ifdef ENGONE_LOGGER
#ifndef ENGONE_LOGGER_GUARD
#define ENGONE_LOGGER_GUARD

// INCLUDES (when not using precompiled header)
// #define _WIN32_WINNT 0x0501
// #define WIN32_LEAN_AND_MEAN
// #include <windows.h>
// #include <mutex>
// #include <fstream>
// #include <stdio.h>

#ifdef ENGONE_PHYSICS
#include "Engone/Utilities/rp3d.h"
#endif
namespace engone {
	// cannot be zero initialized with malloc and memset
	class LogReport {
	public:
		LogReport() = default;
		~LogReport() { cleanup(); }

		LogReport(const LogReport&) = delete;
		LogReport& operator =(const LogReport&) = delete;

		//void add(const std::string& str);
		//void add(const char* str, uint32_t length);
		// will make sure the pointer has free space you can add to.
		// 
		// use usedSpace afterwards.
		// mayPrintTime will be called
		char* ensure(uint32_t length);
		// increments size.
		void usedSpace(uint32_t length);

		// will fire when printTimeNext has been called before.
		// call printTimeNext again to fire this function again.
		void mayPrintTime();
		void printTimeNext();

		void disableTime(bool yes) {
			m_disableTime = yes;
		}

		// returns false if failed
		bool saveReport(const char* filepath);

		//void lock() { m_mutex.lock(); }
		//void unlock() { m_mutex.unlock(); }

		void cleanup();
	private:
		bool resize(uint32_t size);
		bool doPrintTime = true;
		bool m_disableTime = false;

		//std::mutex m_mutex;

		char* m_data = nullptr; // char* because i don't like void*
		uint32_t m_maxSize = 0;
		uint32_t m_size = 0;
		std::string name = "logreport.txt";
	};
	namespace log {
		// Will change color of out stream
		enum Color : uint8_t {
			BLACK = 0x00,
			NAVY = 0x01,
			GREEN = 0x02,
			CYAN = 0x03,
			BLOOD = 0x04,
			PURPLE = 0x05,
			GOLD = 0x06,
			SILVER = 0x07,
			GRAY = 0x08,
			BLUE = 0x09,
			LIME = 0x0A,
			AQUA = 0x0B,
			RED = 0x0C,
			MAGENTA = 0x0D,
			YELLOW = 0x0E,
			WHITE = 0x0F,
			_BLACK = 0x00,
			_BLUE = 0x10,
			_GREEN = 0x20,
			_TEAL = 0x30,
			_BLOOD = 0x40,
			_PURPLE = 0x50,
			_GOLD = 0x60,
			_SILVER = 0x70,
			_GRAY = 0x80,
			_NAVY = 0x90,
			_LIME = 0xA0,
			_AQUA = 0xB0,
			_RED = 0xC0,
			_MAGENTA = 0xD0,
			_YELLOW = 0xE0,
			_WHITE = 0xF0
		};
		enum Filter : uint32_t {
			FilterAll = -2, // full bit mask except 1
			FilterNone = 0,
			Disable=1,
			Info=2,
			Warning=4,
			Error=8,
		};
		Filter operator|(Filter a, Filter b);
		Filter operator&(Filter a, Filter b);
		Filter operator^(Filter a, Filter b);
		Filter operator~(Filter a);
		Filter operator|=(Filter& a, Filter b);
		Filter operator&=(Filter& a, Filter b);
		Filter operator^=(Filter& a, Filter b);
		//Filter operator~(Filter a) {
		//	return (Filter)(~(uint32_t)a);
		//}
	}
	// Make sure class isn't copied.
	// cannot be zero initialized (with malloc, memset)
	// filters are compared with AND-bitwise operator.

	// Major flaw is that filters work poorly with multithreading.
	// Filters are scrapped untill I have a good solution
	class Logger {
	public:
		Logger() = default;

		Logger(const Logger&) = delete;
		Logger& operator =(const Logger&) = delete;

		void setConsoleFilter(log::Filter filter) {
			m_mutex.lock();
			m_consoleFilter = filter;
			m_mutex.unlock();
		}
		void setReportFilter(log::Filter filter) {
			m_mutex.lock();
			m_reportFilter = filter;
			m_mutex.unlock();
		}
		// reference to report filter. setting it's value to log::Info will cause report to only catch info level reports.
		log::Filter reportFilter() const {
			return m_reportFilter;
		}
		// reference to filter.
		log::Filter consoleFilter() const {
			return m_consoleFilter;
		}

		void print(char* str, int len);

		void disableTime(bool yes);
		// NOTE: not using templates because they obscure things and you need to build the project to know if a data type is defined.
		//   Using templates would mean less code but I am not that desperate.
		Logger& operator<<(const char* value);
		Logger& operator<<(char* value); // treated as a string
		Logger& operator<<(void* value);

		Logger& operator<<(const std::string& value);
		//Logger& operator<<(std::string value);

		Logger& operator<<(int64_t value);
		Logger& operator<<(uint64_t value);
		Logger& operator<<(int32_t value);
		Logger& operator<<(uint32_t value);
		Logger& operator<<(int16_t value);
		Logger& operator<<(uint16_t value);
		Logger& operator<<(char value); // interpreted as a character not a number.
		Logger& operator<<(uint8_t value);

		Logger& operator<<(double value);
		Logger& operator<<(float value);

#ifdef ENGONE_GLM
		Logger& operator<<(const glm::vec3& value);
		Logger& operator<<(const glm::vec4& value);
		Logger& operator<<(const glm::quat& value);
		Logger& operator<<(const glm::mat4& value);
#endif
#ifdef ENGONE_PHYSICS
		Logger& operator<<(const rp3d::Vector3& value);
		Logger& operator<<(const rp3d::Quaternion& value);
#endif

		//set the current filter.
		Logger& operator<<(log::Filter value) {
			m_currentFilter = value;
			return *this;
		}
		Logger& operator<<(log::Color value) {
			setColor(value);
			return *this;
		}

		void lock();
		// newLine is used for mutexPerLine
		void unlock(bool newLine=false);

		LogReport& getSelected() {
			return getReport(m_select);
		}
		// does not deal with out of bounds. max is MAX_REPORTS
		LogReport& getReport(uint32_t index) {
			return m_reports[index];
		}
		// out of bounds will selected the first report.
		void selectReport(uint32_t index) {
			if (index > MAX_REPORTS)
				m_select = 0;
			else
				m_select = index;
		}
		log::Color getColor() const {
			return m_color;
		}
		// Mutex is locked per \n instead of per << operator which looks better
		// True by default.
		void setMutexPerLine(bool yes) {
			m_mutex.lock();
			mutexPerLine = yes;
			m_mutex.unlock();
		}
	private:
		inline bool reportMatch() {
			//return m_reportFilter & m_currentFilter && (m_reportFilter & log::Disable)==0;
			return true;
		}
		inline bool consoleMatch() {
			//return m_consoleFilter & m_currentFilter && (m_consoleFilter & log::Disable) == 0;
			return true;
		}

		void setColor(log::Color color);
		HANDLE m_consoleHandle = NULL; // used to change colors

		std::mutex m_mutex;
		std::thread::id m_threadId{}; // owner of mutex
		bool mutexPerLine=true;

		log::Color m_color = log::SILVER;
		log::Filter m_currentFilter = log::FilterAll;
		log::Filter m_reportFilter = log::FilterAll;
		log::Filter m_consoleFilter = log::FilterAll;
		uint32_t m_select = 0;
		static const int MAX_REPORTS = 5; // probably never gonna need more than one.
		LogReport m_reports[MAX_REPORTS];
	};
	namespace log {
		extern Logger out;
	}
}
#endif
#ifdef ENGONE_LOGGER_IMPL
#undef ENGONE_LOGGER_IMPL
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
		if (!doPrintTime||m_disableTime) return;

		int length = 11;
		char* bytes = nullptr;
		if (m_size + length > m_maxSize) {
			if (resize((m_maxSize + length) * 2))
				bytes = m_data + m_size;
			//return nullptr;
		}
		else {
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
		bool succ=true;
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

			std::ofstream file(filepath, std::ios::binary);
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
		for (int i = 0; i < MAX_REPORTS;i++) {
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
		}else{
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
			char* str = report.ensure(len+1);
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
		unlock(value=='\n');
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
#endif // ENGONE_LOGGER_IMPL
#endif // ENGONE_LOGGER