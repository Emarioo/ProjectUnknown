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

std::string ReadFile(std::string path,int* err);
std::vector<std::string> ReadFileList(std::string path,int* err);

int WriteTextFile(std::string path, std::vector<std::string> text);

int FHLoadMesh(MeshData* da,std::string path);
int FHLoadAnim(AnimData* da,std::string path);
int FHLoadColl(CollData* da, std::string path);

// Deprecated
/*
int FHLoadContainer(Mesh* mesh,std::string path);
int FHLoadAnimation(AnimationData* anim,std::string path);
*/