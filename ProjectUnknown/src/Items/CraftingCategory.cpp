#include "gonpch.h"

#include "CraftingCategory.h"

CraftingCategory::CraftingCategory(const std::string& name)
	: name(name) {

}
CraftingRecipe* CraftingCategory::AddRecipe(const std::string& name, int count) {
	recipes.push_back(CraftingRecipe(name,count));
	return &recipes.back();
}
CraftingRecipe* CraftingCategory::GetRecipe(int index) {
	if (-1 < index && index < recipes.size())
		return &recipes[index];
	return nullptr;
}