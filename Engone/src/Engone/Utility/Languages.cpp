#include "gonpch.h"

#include "Languages.h"

#include "../Handlers/FileHandler.h"

namespace engone {

	std::map<std::string, std::map<std::string, std::string>> langs;
	std::string currentLang = "english";
	void LLoadLang(std::string path) {
		std::vector<std::string> list;
		FileReport err = ReadTextFile("assets/languages/" + path + ".txt", list);
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