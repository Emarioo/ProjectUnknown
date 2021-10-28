#include "gonpch.h"

#if gone
#include "../Handlers/FileHandler.h"

//#if ENGONE_GLFW

#include "Font.h"

namespace engone {
	Font::Font() {}
	Font::Font(const std::string& name) {
		Init(name);
	}
	Font::~Font() {
		/*
		if (texture != nullptr) {
			texture->Unbind();
			texture->~Texture();
		}
		*/
	}
	bool Font::Init(const std::string& path) {
		std::vector<std::string> list;
		FileReport err = ReadTextFile(path + ".txt", list);

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
			if (!texture.Init(path+".png")) {
				return false; // no error
			}
		}
		error = true;
		return true;
	}
}
#endif