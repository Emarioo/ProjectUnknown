
#include "Logger.h"

namespace engone
{
	namespace log
	{
		ConsoleColorCode operator|(ConsoleColorCode c0, ConsoleColorCode c1)
		{
			return (ConsoleColorCode)((char)c0|(char)c1);
		}
		HANDLE hConsole = GetStdHandle(-11);
		logger out;
		char TIME = 24;
		logger logger::operator<<(const std::string& s)
		{
			std::cout << s;

			if (!s.empty()) {
				if (s.back() == '\n' && hConsole != NULL)
					SetConsoleTextAttribute(hConsole, ConsoleColorCode::SILVER);
			}

			return *this;
		}
		logger logger::operator<<(float f)
		{
			std::cout << f;
			return *this;
		}
		logger logger::operator<<(double d)
		{
			std::cout << d;

			return *this;
		}
		logger logger::operator<<(char c)
		{
			if (c == TIME) {
				time_t timer=time(NULL);
				struct tm info;
				localtime_s(&info,&timer);
				char buffer[9];
				strftime(buffer,9,"%H:%M:%S",&info);
				std::cout << "[" << buffer<<"]";
			}else
				std::cout << c;

			if(c=='\n'&& hConsole != NULL)
				SetConsoleTextAttribute(hConsole, ConsoleColorCode::SILVER);

			return *this;
		}
		logger logger::operator<<(int i)
		{
			std::cout << i;

			return *this;
		}
		logger logger::operator<<(int64_t i) {
			std::cout << i;

			return *this;
		}
		logger logger::operator<<(uint64_t i) {
			std::cout << i;

			return *this;
		}
		logger logger::operator<<(unsigned int i)
		{
			std::cout << i;
			return *this;
		}
		logger logger::operator<<(char* s)
		{
			std::cout << s;

			return *this;
		}
		logger logger::operator<<(void* t)
		{
			std::cout << t;
			
			return *this;
		}
		logger logger::operator<<(glm::ivec2 v)
		{
			std::cout << "[" << v.x << " " << v.y << "]";
			return *this;
		}
		logger logger::operator<<(glm::vec2 v)
		{
			std::cout << "[" << v.x << " " << v.y << "]";
			
			return *this;
		}
		logger logger::operator<<(glm::vec3 v)
		{
			std::cout << "[" << v.x << " " << v.y << " " << v.z << "]";
			return *this;
		}
		logger logger::operator<<(glm::ivec3 v)
		{
			std::cout << "[" << v.x << " " << v.y << " " << v.z << "]";
			return *this;
		}
		logger logger::operator<<(glm::vec4 v)
		{
			std::cout << "[" << v.x << " " << v.y << " " << v.z << " " << v.w << "]";
			return *this;
		}
		logger logger::operator<<(glm::quat q)
		{
			std::cout << "[" << q.x << " " << q.y << " " << q.z << " " << q.w << "]";
			return *this;
		}
		logger logger::operator<<(glm::mat4 m)
		{
			for (int i = 0; i < 4; i++) {
				std::cout << "[" << m[i].x << " " << m[i].y << " " << m[i].z << " " << m[i].w << "]" << "\n";
			}

			return *this;
		}
		logger logger::operator<<(ConsoleColorCode t)
		{
			if (hConsole != NULL)
				SetConsoleTextAttribute(hConsole, t);
			return *this;
		}
	}
	/*debug debug::operator<(engone::ShaderType t) {
		if (doPrint) {
			std::cout << (int)t;
			if (spaces)
				std::cout << " ";
		}
		return *this;
	}*/
}