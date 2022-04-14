
#include "FileHandler.h"

namespace engone {

	bool FindFile(const std::string& path) {
		struct stat buffer;
		return (stat(path.c_str(), &buffer) == 0);
	}
	void GetFiles(const std::string& path, std::vector<std::string>& list) {
		for (const auto& entry : std::filesystem::directory_iterator(path)) {
			list.push_back(entry.path().generic_string());
		}
		/*
		for (int i = 0; i < list.size();i++) {
			std::cout << list.at(i) << std::endl;
		}*/
	}
	FileReport ReadTextFile(const std::string& path, std::vector<std::string>& list)
	{
		std::ifstream file(path);

		if (!file) {
			return FileReport::NotFound;
		}

		std::string line;
		while (getline(file, line))
		{
			list.push_back(line);
		}
		file.close();
		return FileReport::Success;
	}
	FileReport WriteTextFile(const std::string& path, std::vector<std::string>& list) {
		std::ofstream file(path);
		std::vector<std::string> out;
		if (!file) {
			return FileReport::NotFound;
		}
		for (int i = 0; i < list.size(); i++) {
			if (i != 0)
				file.write("\n", 1);
			file.write(&(list[i][0]), list[i].length());
		} 
		file.close();
		return FileReport::Success;
	}
}