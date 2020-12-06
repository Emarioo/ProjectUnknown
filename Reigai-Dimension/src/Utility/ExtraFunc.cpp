#include "ExtraFunc.h"
#include <iostream>

std::vector<std::string> SplitString(std::string text, std::string delim) {
	std::vector<std::string> out;
	unsigned int at = 0;
	while ((at = text.find(delim)) != std::string::npos) {
		std::string push = text.substr(0, at);
		if (push.size() > 0) {
			out.push_back(push);
		}
		text.erase(0, at + delim.length());
	}
	out.push_back(text);
	return out;
}
char SanitizedChars[64]{
	'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z',
	'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
	'0','1','2','3','4','5','6','7','8','9','_','-'
};
std::string SanitizeString(std::string s) {
	std::string out;
	for (int i = 0; i < s.size();i++) {
		for (int j = 0; j < 64; j++) {
			if (s.at(i) == SanitizedChars[j]) {
				out += s.at(i);
				break;
			}
		}
	}
	return out;
}
float lerp(float a, float b, float c) {
	return (1 - c)*a + c * b;
}
float inverseLerp(float min, float max, float x) {
	return (x-min)/(max-min);
}
float distance(float x,float y,float x1,float y1) {
	return sqrt(pow(x1-x,2)+pow(y1-y,2));
}
void insert(float* ar,int off,int len,float* data) { // carefull with going overboard
	std::cout << "INSERT ";
	for (int i = 0; i < len;i++) {
		ar[off + i] = data[i];
		std::cout << off+i<<"_"<<ar[off+ i] << "   ";
	}
	std::cout << std::endl;
}
void printv(int len,float* val) {
	for (int i = 0; i < len;i++) {
		std::cout << val[i]<<" ";
	}
	std::cout << std::endl;
}