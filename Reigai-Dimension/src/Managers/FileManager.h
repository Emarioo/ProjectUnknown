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
	Full path required
	*/
	std::string ReadFile(std::string path, int* err);
	/*
	Remember to handle the error status
	Full path required
	*/
	std::vector<std::string> ReadFileList(std::string path, int* err);
	/*
	Remember to handle the returned error status
	Full path required
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