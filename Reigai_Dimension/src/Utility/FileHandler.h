#pragma once

#include <vector>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "Components/Mesh.h"
#include "Rendering/BufferContainer.h"
#include "ExtraFunc.h"
#include "Components/AnimationData.h"

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

int FHLoadMesh(Mesh* mes,std::string path);
int FHLoadAnim(AnimationData* anim,std::string path);

// Deprecated
/*
int FHLoadContainer(Mesh* mesh,std::string path);
int FHLoadAnimation(AnimationData* anim,std::string path);
*/