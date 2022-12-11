#pragma once

#include "ProUnk/DataRegistries/MasterRegistry.h"
#include "ProUnk/DataRegistries/ItemTypeRegistry.h"

namespace prounk {
	typedef int RecipeId;
	struct SimpleIngredient {
		ItemType itemType=0;
		int count=1;
	};
	class SimpleRecipe {
	public:
		SimpleRecipe() = default;
		void addInput(SimpleIngredient ingredient);
		void addOutput(SimpleIngredient ingredient);
		
		const std::string& getName();
		// getter for input/output?
		std::vector<SimpleIngredient> inputs;
		std::vector<SimpleIngredient> outputs;
		//int requirement;
		
		std::string name;

	private:
		RecipeId recipeId=0;

		friend class RecipeRegistry;
	};
	
	class RecipeRegistry : public DataRegistry {
	public:
		RecipeRegistry() : DataRegistry("recipe_registry") {}
	
		void serialize() override;
		void deserialize() override;

		// creates a new empty recipe which is returned
		// and you can then modify it
		SimpleRecipe* registerSimpleRecipe(const std::string& name);
		
		SimpleRecipe* getSimpleRecipe(RecipeId recipeId);
		SimpleRecipe* getSimpleRecipe(const std::string& name);
		
		// need more advanced ones for special item data
		std::vector<RecipeId> searchInputUsages(ItemType type);
		std::vector<RecipeId> searchOutputUsages(ItemType type);

		std::vector<SimpleIngredient> getCraftableOutputs();

	private:

		// some map<string,recipe> for fast access
		
		std::vector<SimpleRecipe> m_simpleRecipes;
	};
}