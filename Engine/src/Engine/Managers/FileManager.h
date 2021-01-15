#pragma once

#include <vector>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "Utility/Utilities.h"

#include "Data/AnimData.h"
#include "Data/MeshData.h"
#include "Data/CollData.h"
#include "Data/BoneData.h"
#include "Utility/Debugger.h"

namespace engine {
	enum FileReport {
		Success,
		NotFound,
		Syntax,
		Corrupt
	};
	int FileExist(std::string path);
	/*
	Remember to handle the returned error status
	File path without txt required
	*/
	std::string ReadFile(std::string path, int* err);
	/*
	Remember to handle the error status
	File path without txt required
	*/
	std::vector<std::string> ReadFileList(std::string path, int* err);
	/*
	Remember to handle the returned error status
File path without txt required
	*/
	int WriteTextFile(std::string path, std::vector<std::string> text);
	/*
	File path is converted to "assets/meshes/"+path+".mesh"
	Can return NotFound, Corrupt
	*/
	int LoadMesh(MeshData* da, std::string path);
	/*
	File path is converted to "assets/animations/"+path+".anim"
	Can return NotFound, Corrupt
	*/
	int LoadAnim(AnimData* da, std::string path);
	/*
	File path is converted to "assets/colliders/"+path+".coll"
	Can return NotFound, Corrupt
	*/
	int LoadColl(CollData* da, std::string path);
	/*
	File path is converted to "assets/bones/"+path+".bone"
	Can return NotFound, Corrupt
	*/
	int LoadBone(BoneData* da, std::string path);
}