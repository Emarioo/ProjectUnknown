#include "gonpch.h"

#include "Font.h"

#include "../Handlers/FileHandler.h"

namespace engine {
	Font::Font() {

	}
	Font::Font(const std::string& name) {
		Data(name);
	}
	Font::~Font() {
		if (texture != nullptr) {
			texture->Unbind();
			texture->~Texture();
		}
	}
	void Font::Data(const std::string& name) {
		std::string path = "assets/fonts/" + name;
		FileReport err;
		std::vector<std::string> list = ReadTextFile(path+".txt", &err);
		if (err == FileReport::Success) {
			if (list.size() == 2) {
				charWid[0] = std::stoi(list.at(0));
				int num = std::stoi(list.at(1));
				for (int i = 1; i < 256; i++) {
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
			path += ".png";
			if (FileExist(path)) {
				texture = new Texture(path);
			} else {
				bug::out < bug::RED < "Cannot find Font  '" < path < "'\n";
			}
		} else {
			bug::out < bug::RED < "Cannot find font info '" < path < ".txt'\n";
		}
	}
}