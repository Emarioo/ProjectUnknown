#pragma once

// these are not necessary in programs where you have precompiled header
// but incase you don't and throw this header inside, it will still work.
#include <stdint.h>
#include <string>

#ifndef ENGONE_NO_PHYSICS
#include "Engone/Utilities/rp3d.h"
#endif
namespace engone {
	namespace log 	{
		// Will change color of out stream
		enum Color : uint8_t {
			BLACK	 = 0x00,
			NAVY	 = 0x01,
			GREEN	 = 0x02,
			CYAN	 = 0x03,
			BLOOD	 = 0x04,
			PURPLE	 = 0x05,
			GOLD	 = 0x06,
			SILVER	 = 0x07,
			GRAY	 = 0x08,
			BLUE	 = 0x09,
			LIME	 = 0x0A,
			AQUA	 = 0x0B,
			RED		 = 0x0C,
			MAGENTA	 = 0x0D,
			YELLOW	 = 0x0E,
			WHITE	 = 0x0F,
			_BLACK	 = 0x00,
			_BLUE	 = 0x10,
			_GREEN	 = 0x20,
			_TEAL	 = 0x30,
			_BLOOD	 = 0x40,
			_PURPLE	 = 0x50,
			_GOLD	 = 0x60,
			_SILVER  = 0x70,
			_GRAY	 = 0x80,
			_NAVY	 = 0x90,
			_LIME	 = 0xA0,
			_AQUA	 = 0xB0,
			_RED	 = 0xC0,
			_MAGENTA = 0xD0,
			_YELLOW  = 0xE0,
			_WHITE	 = 0xF0
		};
		struct logger
		{
			struct Time {};
			logger() {}
			logger& operator<<(const std::string& s);
			logger& operator<<(char c);
			logger& operator<<(int i);
			logger& operator<<(int64_t i);
			logger& operator<<(uint64_t i);
			logger& operator<<(float f);
			logger& operator<<(double d);
			logger& operator<<(const char* a);
			logger& operator<<(unsigned int i);
#ifndef ENGONE_NO_PHYSICS
			logger& operator<<(const rp3d::Vector3& v);
#endif
#ifndef ENGONE_NO_GLM
			logger& operator<<(const glm::vec2& v);
			logger& operator<<(const glm::vec3& v);
			logger& operator<<(const glm::vec4& v);
			logger& operator<<(const glm::ivec2& v);
			logger& operator<<(const glm::ivec3& v);
			logger& operator<<(const glm::quat& q);
			logger& operator<<(const glm::mat4& v);
#endif
			logger& operator<<(const Color t);
			logger& operator<<(Time t);
			logger& operator<<(void* a);

			Color getColor() const { return color; }
		private:
			Color color=SILVER;
			void setColor(Color color);
		};
		// stream like std::cout but support for glm data types and color
		extern logger out;
		// using this char in out stream will print the time Hour:Min:Sec
		extern logger::Time TIME;

#ifdef ENGONE_LOGGER_IMPLEMENTATION
		HANDLE hConsole = GetStdHandle(-11);
		logger out;
		logger::Time TIME;
		void logger::setColor(Color col) {
			if (color == col) return;
			color = col;
			if (hConsole == NULL) return;
			SetConsoleTextAttribute(hConsole, color);
		}
		logger& logger::operator<<(const std::string& s) {
			std::cout << s;

			if (!s.empty()) {
				if (s.back() == '\n') {
					setColor(SILVER);
				}
			}

			return *this;
		}
		logger& logger::operator<<(float f) {
			std::cout << f;
			return *this;
		}
		logger& logger::operator<<(double d) {
			std::cout << d;

			return *this;
		}
		logger& logger::operator<<(char c) {
			std::cout << c;

			if (c == '\n')
				setColor(SILVER);

			return *this;
		}
		logger& logger::operator<<(int i) {
			std::cout << i;

			return *this;
		}
		logger& logger::operator<<(int64_t i) {
			std::cout << i;

			return *this;
		}
		logger& logger::operator<<(uint64_t i) {
			std::cout << i;

			return *this;
		}
		logger& logger::operator<<(unsigned int i) {
			std::cout << i;
			return *this;
		}
		logger& logger::operator<<(const char* s) {
			uint32_t len = strlen(s);
			if (len != 0) {
				std::cout << s;
				if (s[len - 1] == '\n')
					setColor(SILVER);
			}

			return *this;
		}
		logger& logger::operator<<(void* t) {
			std::cout << t;

			return *this;
		}
#ifndef ENGONE_NO_PHYSICS
		logger& logger::operator<<(const rp3d::Vector3& v) {
			std::cout << "[" << v.x << " " << v.y << " " << v.z << "]";
			return *this;
		}
#endif
#ifndef ENGONE_NO_GLM
		logger& logger::operator<<(const glm::ivec2& v) {
			std::cout << "[" << v.x << " " << v.y << "]";
			return *this;
		}
		logger& logger::operator<<(const glm::vec2& v) {
			std::cout << "[" << v.x << " " << v.y << "]";
			return *this;
		}
		logger& logger::operator<<(const glm::vec3& v) {
			std::cout << "[" << v.x << " " << v.y << " " << v.z << "]";
			return *this;
		}
		logger& logger::operator<<(const glm::ivec3& v) {
			std::cout << "[" << v.x << " " << v.y << " " << v.z << "]";
			return *this;
		}
		logger& logger::operator<<(const glm::vec4& v) {
			std::cout << "[" << v.x << " " << v.y << " " << v.z << " " << v.w << "]";
			return *this;
		}
		logger& logger::operator<<(const glm::quat& q) {
			std::cout << "[" << q.x << " " << q.y << " " << q.z << " " << q.w << "]";
			return *this;
		}
		logger& logger::operator<<(const glm::mat4& m) {
			for (int i = 0; i < 4; i++) {
				std::cout << "[" << m[i].x << " " << m[i].y << " " << m[i].z << " " << m[i].w << "]" << "\n";
			}

			return *this;
		}
#endif
		logger& logger::operator<<(Color t) {
			setColor(t);
			return *this;
		}
		logger& logger::operator<<(Time t) {
			time_t timer = time(NULL);
			struct tm info;
			localtime_s(&info, &timer);
			char buffer[9];
			strftime(buffer, 9, "%H:%M:%S", &info);
			std::cout << "[" << buffer << "]";

			return *this;
		}
#endif
	}
}