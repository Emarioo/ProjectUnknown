#pragma once

#include <vector>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "Utility/ExtraFunc.h"

#include "Data/AnimData.h"
#include "Data/MeshData.h"
#include "Data/CollData.h"
#include "Data/BoneData.h"
#include "Utility/Debugger.h"

namespace fManager {
	enum {
		Success,
		NotFound,
		Syntax,
		Corrupt
	};
	int FileExist(std::string path);
	/*
	Remember to handle the returned error status
	*/
	std::string ReadFile(std::string path, int* err);
	/*
	Remember to handle the error status
	*/
	std::vector<std::string> ReadFileList(std::string path, int* err);
	/*
	Remember to handle the returned error status
	*/
	int WriteTextFile(std::string path, std::vector<std::string> text);
	/*
	Parameter "path" is automatically using .mesh format
	Prints out any error (NotFound, Corrupt)
	*/
	int LoadMesh(MeshData* da, std::string path);
	/*
	Parameter "path" is automaticly using .anim format
	Prints out any error (NotFound, Corrupt)
	*/
	int LoadAnim(AnimData* da, std::string path);
	/*
	Parameter "path" is automaticly using .coll format
	Prints out any error (NotFound, Corrupt)
	*/
	int LoadColl(CollData* da, std::string path);
	/*
	Parameter "path" is automaticly using .bone format
	Prints out any error (NotFound, Corrupt)
	*/
	int LoadBone(BoneData* da, std::string path);
}