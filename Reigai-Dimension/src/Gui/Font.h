#pragma once

#include <string>

#include "Rendering/Texture.h"


class Font {
public:
	Font() {}
	Font(std::string path);
	void Data(std::string path);
	Texture* texture;
	int charWid[256];
	
	int imgSize=1024;
	int charSize = imgSize / 16;
private:

};