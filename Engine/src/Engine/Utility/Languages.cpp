#include "Languages.h"

#include "../Handlers/FileHandler.h"

#include <map>
#include <vector>

namespace engine {

	std::map<std::string, std::map<std::string, std::string>> langs;
	std::string currentLang = "english";
	void LLoadLang(std::string path) {
		FileReport err;
		std::vector<std::string> list = ReadTextFile("assets/languages/" + path + ".txt", &err);
		if (err == FileReport::Success) {
			langs[path] = std::map<std::string, std::string>();
			for (std::string s : list) {
				std::vector<std::string> split = SplitString(s, "=");
				langs[path][split[0]] = split[1];
			}
		}
	}
	void LSetLang(std::string lan) {
		currentLang = lan;
	}
	std::string LGetString(std::string str) {
		return langs[currentLang][str];
	}
}