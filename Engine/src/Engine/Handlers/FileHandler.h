#pragma once

#include <vector>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "Utility/Utilities.h"

#include "Components/Animation.h"
#include "Components/Mesh.h"
#include "Components/Collider.h"
#include "Components/Armature.h"
#include "Components/Model.h"
#include "Utility/Debugger.h"

namespace engine {
	enum FileReport {
		Success,
		NotFound,
		Syntax,
		Corrupt
	};
	int FileExist(const std::string& path);
	/*
	Remember to handle the returned error status
	File path without txt required
	*/
	std::string ReadFile(std::string path, int* err);
	/*
	Remember to handle the error status
	File path without txt required
	*/
	std::vector<std::string> ReadFileList(const std::string& path, int* err);
	/*
	Remember to handle the returned error status
	File path without txt required
	*/
	int WriteTextFile(const std::string& path, std::vector<std::string> text);
	/*
	File path is converted to "assets/materials/"+path+".material"
	Returned object's hasError is true if any errors occur
	*/
	void LoadMaterial(Material* data, const std::string& path);
	/*
	File path is converted to "assets/meshes/"+path+".mesh"
	Returned object's hasError is true if any errors occur
	*/
	void LoadMesh(Mesh* data, const std::string& path);
	/*
	File path is converted to "assets/animations/"+path+".animation"
	Returned object's hasError is true if any errors occur
	*/
	void LoadAnimation(Animation* data, const std::string& path);
	/*
	File path is converted to "assets/colliders/"+path+".collider"
	Returned object's hasError is true if any errors occur
	*/
	void LoadCollider(Collider* data, const std::string& path);
	/*
	File path is converted to "assets/armatures/"+path+".armature"
	Returned object's hasError is true if any errors occur
	*/
	void LoadArmature(Armature* data, const std::string& path);
	/*
	File path is converted to "assets/colliders/"+path+".model"
	Returned object's hasError is true if any errors occur
	*/
	void LoadModel(Model* data, const std::string& path);
}