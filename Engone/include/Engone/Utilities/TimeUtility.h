

#ifndef ENGONE_TIMEUTIL_GUARD
#define ENGONE_TIMEUTIL_GUARD

//#define TIMER(str) Timer str = Timer(#str,__LINE__*strlen(__FILE__))

#ifdef ENGONE_LOGGER
#include "Engone/Logger.h"
#endif

namespace engone {

	// Time since epoch in seconds, microsecond precision
	double GetSystemTime();
	// the time of your computer clock, i think 
	std::string GetClock();
	// optimized version of GetClock
	// str must be at least 8 bytes in size.
	void GetClock(char* str);

	enum FormatTimePeriod : uint8_t {
		FormatTimeNS = 1, // nanoseconds
		FormatTimeUS = 2, // microseconds
		FormatTimeMS = 4, // milliseconds 
		FormatTimeS = 8, // seconds
		FormatTimeM = 16, // minutes
		FormatTimeH = 32, // hours
		FormatTimeD = 64, // days
		FormatTimeW = 128, // weeks
	};
	typedef int FormatTimePeriods;
	// if compact is true, result will be 1d 5h 1m 13s, false is 1 day 5 hours 1 minutes 13 seconds.
	// flags are used to determine what literals you want. day, hour, minute, second is default.
	// the smallest literal will be what time should be in. with default. time is in seconds
	// If you want precision, this function is best.
	// If you only use the day flag when the time is smaller than a day, an empty string will be returned.
	std::string FormatTime(uint64_t time, bool compact = false, FormatTimePeriods flags = FormatTimeD | FormatTimeH | FormatTimeM | FormatTimeS);
	// if compact is true, result will be 1d 5h 1m 13s, false is 1 day 5 hours 1 minutes 13 seconds.
	// flags are used to determine what literals you want. day, hour, minute, second is default.
	// If you want precision, use this function but with uin64_t in nanoseconds instead. Doubles are a bit special.
	// If you only use the day flag when the time is smaller than a day, an empty string will be returned.
	std::string FormatTime(double seconds, bool compact = false, FormatTimePeriods flags = FormatTimeD | FormatTimeH | FormatTimeM | FormatTimeS);

	// instead std::this_thread::sleep_for, microsecond precision
	void Sleep(double seconds);

#ifdef ENGONE_LOGGER
	// this class is based on engone logger.
	// it doesn't work without it. You could use std::cout instead
	// but i dont want to.
	class Timer {
	public:
		Timer(const char* name) : name(name), startTime(GetSystemTime()) {}
		~Timer() {
			stop();
		}
		void stop() {
			if (startTime == 0) return;
			double diff = GetSystemTime() - startTime;
			startTime = 0;
			if (name)
				log::out << name;
			else
				log::out << "Time";
			log::out << ": " << diff << "\n";
		}
#endif

	private:
		const char* name = nullptr;
		double startTime = 0;
	};
}
#endif // ENGONE_TIMEUTIL_GUARD

#ifdef ENGONE_TIMEUTIL_IMPL
#undef ENGONE_TIMEUTIL_IMPL

namespace engone {
	double GetSystemTime() {
		return (double)std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count() / 1000000.0;
		//std::cout << std::chrono::system_clock::now().time_since_epoch().count() <<" "<< (std::chrono::system_clock::now().time_since_epoch().count() / 10000000) << "\n";
		//return (double)(std::chrono::system_clock::now().time_since_epoch().count() / 10000000);
	}
	std::string GetClock() {
		time_t t;
		time(&t);
		tm tm;
		localtime_s(&tm, &t);
		std::string str = std::to_string(tm.tm_hour) + ":" + std::to_string(tm.tm_min) + ":" + std::to_string(tm.tm_sec);
		return str;
	}
	void GetClock(char* str) {
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
	std::string FormatTime(double seconds, bool compact, FormatTimePeriods flags) {
		bool small = false;
		for (int i = 0; i < 3; i++) {
			if (flags & (1 << i))
				small = true;
			if (small) seconds *= 1000;
		}
		return FormatTime((uint64_t)round(seconds), compact, flags);
	}
	std::string FormatTime(uint64_t time, bool compact, FormatTimePeriods flags) {
		if (time == 0)
			return "0s";
		// what is the max amount of characters?
		const int outSize = 130;
		char out[outSize]{};
		int write = 0;

		uint64_t num[8]{};
		uint64_t divs[8]{ 1,1,1,1,1,1,1,1 };
		uint64_t divLeaps[8]{ 1000,1000,1000,60,60,24,7,1 };
		const char* lit[8]{ "nanosecond","microsecond","millisecond","second","minute","hour","day","week" };

		uint64_t rest = time;

		bool smallest = false;
		for (int i = 0; i < 7; i++) {
			if (flags & (1 << i))
				smallest = true;
			if (smallest)
				divs[i + 1] *= divs[i] * divLeaps[i];
		}

		for (int i = 7; i >= 0; i--) {
			if (0 == (flags & (1 << i))) continue;
			num[i] = rest / divs[i];
			if (num[i] == 0) continue;
			rest -= num[i] * divs[i];

			if (write != 0) out[write++] = ' ';
			if(compact)
				write += snprintf(out + write, outSize - write, "%u", num[i]);
			else
				write += snprintf(out + write, outSize - write, "%u ", num[i]); // adds a space
			if (compact) {
				out[write++] = lit[i][0];
				if (i < 3)
					out[write++] = 's';
			}
			else {
				memcpy(out + write, lit[i], strlen(lit[i]));
				write += strlen(lit[i]);
				if (num[i] != 1)
					out[write++] = 's';
			}
		}
		return out;
	}
	void Sleep(double seconds) {
		std::this_thread::sleep_for((std::chrono::microseconds)((uint64_t)(seconds * 1000000)));
	}
}

#endif