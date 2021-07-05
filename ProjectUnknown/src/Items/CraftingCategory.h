#pragma once

#include "CraftingRecipe.h"

class CraftingCategory {
public:
	CraftingCategory(const std::string& name);
	std::string name;
	std::vector<CraftingRecipe> recipes;
	CraftingRecipe* AddRecipe(const std::string& name, int count);
	// May return nullptr
	CraftingRecipe* GetRecipe(int index);
};