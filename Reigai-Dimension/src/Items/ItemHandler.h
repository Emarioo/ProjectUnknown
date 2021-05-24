#pragma once

#include <vector>
#include <string>
#include "Engine/Engine.h"

void InitItemHandler();

Texture* GetTextureFromItem(const std::string& name);
void AddItemTexture(const std::string& name, const std::string& path);

void DrawItem(const std::string& name, float x, float y, float w, float h);