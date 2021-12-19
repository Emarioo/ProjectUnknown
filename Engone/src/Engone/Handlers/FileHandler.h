#pragma once

#include "../Utility/Utilities.h"
#include "../DebugHandler.h"

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
#if gone
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
#endif
}