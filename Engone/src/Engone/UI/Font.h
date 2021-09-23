#pragma once

#include "../Rendering/Texture.h"

namespace engone {

	class Font {
	public:
		Font();
		Font(const std::string& path);
		~Font();
		void Data(const std::string& path);
		Texture* texture;
		int charWid[256];

		int imgSize = 1024;
		int charSize = imgSize / 16;
	private:

	};
}