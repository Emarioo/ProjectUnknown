#pragma once

#include <string>

namespace engine {

	void LLoadLang(std::string path);
	void LSetLang(std::string lan);
	std::string LGetString(std::string str);

}