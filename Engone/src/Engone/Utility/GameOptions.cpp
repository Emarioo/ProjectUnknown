#include "gonpch.h"

#include "GameOptions.h"

namespace engone {

	std::map<std::string, int> intOptions;
	std::map<std::string, float> floatOptions;
	int GetOptioni(std::string s) {
		return intOptions[s];
	}

	float GetOptionf(std::string s) {
		return floatOptions[s];
	}
	void AddOptioni(std::string s, int i) {
		intOptions[s] = i;
	}
	void AddOptionf(std::string s, float f) {
		floatOptions[s] = f;
	}
	void ReadOptions() {
		std::vector<std::string> list;
		FileReport err = ReadTextFile("data/gameoptions.txt", list);
		if (err == FileReport::NotFound)
			return;
		for (std::string s : list) {
			std::vector<std::string> set = SplitString(s, " ");
			if (set.size() > 1) {
				//std::cout << set[0] << " " << set[1] << std::endl;
				if (set[0].at(0) == 'f') {
					AddOptionf(set[0].substr(1), std::stof(set[1]));
				} else {
					AddOptioni(set[0].substr(1), std::stoi(set[1]));
				}
			}
		}
	}
}