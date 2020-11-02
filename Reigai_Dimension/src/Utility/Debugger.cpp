#include "Debugger.h"

bool DebuggerPrint;
void DebugOn() {
	DebuggerPrint = true;
}
void DebugOff() {
	DebuggerPrint = false;
}
void print(std::string s) {
	if(DebuggerPrint)
		std::cout << s;
}
void print(float s) {
	if (DebuggerPrint)
		std::cout << s;
}
void print(int s) {
	if (DebuggerPrint)
		std::cout << s;
}
void printl(std::string s,float f) {
	if (DebuggerPrint)
		std::cout << s<<f <<std::endl;
}
void printl() {
	if(DebuggerPrint)
		std::cout << std::endl;
}