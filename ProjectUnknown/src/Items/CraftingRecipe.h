#pragma once

#include "Container.h"

class CraftingIngredient {
public:
	CraftingIngredient(const std::string& name, int count);
	std::string name;
	short count;
};

class CraftingRecipe {
public:
	CraftingRecipe(const std::string& name, int count);
	 
	std::vector<CraftingIngredient> inputs;
	CraftingIngredient output;

	// anvil requirement... 

	void AddInput(const std::string& name, int count);
	/*
	returns possible crafting attempts
	*/
	int GetPossibleCraftingAttempts(Container* inventory);
	/*
	Call GetPossibleCraftingAttempts
	returns crafted items
	*/
	int AttemptCraft(Container* inventory,int count);
};