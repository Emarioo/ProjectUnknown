#pragma once

#include <vector>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

//#include "Rendering/BufferContainer.h"
#include "ExtraFunc.h"

#include "Data/AnimData.h"
#include "Data/MeshData.h"
#include "Data/CollData.h"

enum {
	FHSuccess,
	FHNotFound,
	FHSyntax,
	FHCorrupt
};
int FHFileExist(std::string path);

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
*/
int FHLoadMesh(MeshData* da, std::string path);
/*
Parameter "path" is automaticly using .anim format
*/
int FHLoadAnim(AnimData* da, std::string path);
int FHLoadColl(CollData* da, std::string path);