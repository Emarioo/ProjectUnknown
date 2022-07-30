//
#define ENGONE_LOGGER_IMPLEMENTATION
#include "Engone/Logger.h"
//
//namespace engone {
//	namespace log {
//		HANDLE hConsole = GetStdHandle(-11);
//		logger out;
//		logger::Time TIME;
//		void logger::setColor(Color col) {
//			if (color == col) return;
//			color = col;
//			if (hConsole==NULL) return;
//			SetConsoleTextAttribute(hConsole, color);
//		}
//		logger& logger::operator<<(const std::string& s) {
//			std::cout << s;
//
//			if (!s.empty()) {
//				if (s.back() == '\n') {
//					setColor(SILVER);
//				}
//			}
//
//			return *this;
//		}
//		logger& logger::operator<<(float f) {
//			std::cout << f;
//			return *this;
//		}
//		logger& logger::operator<<(double d) {
//			std::cout << d;
//
//			return *this;
//		}
//		logger& logger::operator<<(char c) {
//			std::cout << c;
//
//			if (c == '\n')
//				setColor(SILVER);
//
//			return *this;
//		}
//		logger& logger::operator<<(Time t) {
//			time_t timer = time(NULL);
//			struct tm info;
//			localtime_s(&info, &timer);
//			char buffer[9];
//			strftime(buffer, 9, "%H:%M:%S", &info);
//			std::cout << "[" << buffer << "]";
//
//			return *this;
//		}
//		logger& logger::operator<<(int i) {
//			std::cout << i;
//
//			return *this;
//		}
//		logger& logger::operator<<(int64_t i) {
//			std::cout << i;
//
//			return *this;
//		}
//		logger& logger::operator<<(uint64_t i) {
//			std::cout << i;
//
//			return *this;
//		}
//		logger& logger::operator<<(unsigned int i) {
//			std::cout << i;
//			return *this;
//		}
//		logger& logger::operator<<(const char* s) {
//			uint32_t len = strlen(s);
//			if (len!=0) {
//				std::cout << s;
//				if (s[len - 1] == '\n')
//					setColor(SILVER);
//			}
//
//			return *this;
//		}
//		logger& logger::operator<<(void* t) {
//			std::cout << t;
//
//			return *this;
//		}
//		logger& logger::operator<<(const glm::ivec2& v) {
//			std::cout << "[" << v.x << " " << v.y << "]";
//			return *this;
//		}
//		logger& logger::operator<<(const glm::vec2& v) {
//			std::cout << "[" << v.x << " " << v.y << "]";
//			return *this;
//		}
//		logger& logger::operator<<(const glm::vec3& v) {
//			std::cout << "[" << v.x << " " << v.y << " " << v.z << "]";
//			return *this;
//		}
//#ifndef ENGONE_NO_PHYSICS
//		logger& logger::operator<<(const rp3d::Vector3& v) {
//			std::cout << "[" << v.x << " " << v.y << " " << v.z << "]";
//			return *this;
//		}
//#endif
//		logger& logger::operator<<(const glm::ivec3& v) {
//			std::cout << "[" << v.x << " " << v.y << " " << v.z << "]";
//			return *this;
//		}
//		logger& logger::operator<<(const glm::vec4& v) {
//			std::cout << "[" << v.x << " " << v.y << " " << v.z << " " << v.w << "]";
//			return *this;
//		}
//		logger& logger::operator<<(const glm::quat& q) {
//			std::cout << "[" << q.x << " " << q.y << " " << q.z << " " << q.w << "]";
//			return *this;
//		}
//		logger& logger::operator<<(const glm::mat4& m) {
//			for (int i = 0; i < 4; i++) {
//				std::cout << "[" << m[i].x << " " << m[i].y << " " << m[i].z << " " << m[i].w << "]" << "\n";
//			}
//
//			return *this;
//		}
//		logger& logger::operator<<(Color t) {
//			setColor(t);
//			return *this;
//		}
//	}
//}