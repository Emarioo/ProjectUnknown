#pragma once

#include <map>
#include <string>
#include "Managers/FileManager.h"

void ReadOptions();
int GetOptioni(std::string);
float GetOptionf(std::string);
void AddOptioni(std::string, int);
void AddOptionf(std::string, float);