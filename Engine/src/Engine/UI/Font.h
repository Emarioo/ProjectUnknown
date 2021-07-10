#pragma once

#include <string>

#include "../Rendering/Texture.h"

namespace engine {

	class Font {
	public:
		Font() {}
		Font(const std::string& path);
		void Data(const std::string& path);
		Texture* texture;
		int charWid[256];

		int imgSize = 1024;
		int charSize = imgSize / 16;
	private:

	};
}