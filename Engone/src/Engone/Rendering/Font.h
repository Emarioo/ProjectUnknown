#pragma once

//#if ENGONE_GLFW

#include "Texture.h"

namespace engone {

	class Font {
	public:
		Font();
		/*
		@path without the format, png and txt is automatically added.
		Check if the font's error variable is true.
		*/
		Font(const std::string& path);
		~Font();
		/*
		@path without the format, png and txt is automatically added.
		@return true if an error has occured.
		*/
		bool Init(const std::string& path);
		Texture texture;
		int charWid[256];

		int imgSize = 1024;
		int charSize = imgSize / 16;
		
		bool error = false;
	};
}
//#endif