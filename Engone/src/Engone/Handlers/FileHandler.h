#pragma once

#include "../Utility/Utilities.h"

namespace engone {
	enum class FileReport {
		Success,
		NotFound,
		Syntax,
		Corrupt
	};
	bool FindFile(const std::string& path);

	/*
	fill 'list' with files in 'path' directory
	*/
	void GetFiles(const std::string& path, std::vector<std::string>& list);
	
	/*
	Remember to handle the returned error status
	File path without txt required
	std::string ReadFile(std::string path, FileReport* err);*/

	/*
	Remember to handle the error status
	Full file path
	*/
	FileReport ReadTextFile(const std::string& path, std::vector<std::string>& list);
	/*
	Remember to handle the returned error status
	Full file path
	*/
	FileReport WriteTextFile(const std::string& path, std::vector<std::string>& text);
}