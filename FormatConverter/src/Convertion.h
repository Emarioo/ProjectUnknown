#pragma once

#include <iostream>
#include <string>
#include "CollData.h"

enum ConvError {
	Success,
	NotFound,
	Syntax,
	Corrupt
};

int ConvertCollider(std::string path);
int ReadCollider(std::string path);
int ConvertMesh(std::string path);
int ReadMesh(std::string path);
int ConvertAnim(std::string path);
int ReadAnim(std::string path);