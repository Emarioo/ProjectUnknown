#include "Font.h"

#include "Utility/FileManager.h"

Font::Font(std::string path) {
	Data(path);
}
void Font::Data(std::string path) {
	int err = 0;
	std::vector<std::string> list = fManager::ReadFileList(path, &err);
	if (err == fManager::Success) {
		if (list.size() == 1) {
			int num = std::stoi(list.at(0));
			for (int i = 0; i < 256;i++) {
				charWid[i] = num;
			}
		} else {
			int i = 0;
			for (std::string s : list) {
				std::vector<std::string> list2 = SplitString(s, ",");
				for (std::string s2 : list2) {
					charWid[i] = std::stoi(s2);
					i++;
				}
			}
		}
		if (fManager::FileExist(path + ".png")) {
			texture = new Texture(path);
		} else {
			std::cout << "[Font] Not Found: " << path << ".png" << std::endl;
		}
	} else {
		for (int i = 0; i < 256; i++) {
			charWid[i] = 40;
		}
		std::cout << "[Font] Not Found: " << path << ".txt" << std::endl;
	}
}