
#include "ProUnk/DataRegistries/RecipeRegistry.h"

namespace prounk {
	void SimpleRecipe::addInput(SimpleIngredient ingredient) {
		inputs.push_back(ingredient);
	}
	void SimpleRecipe::addOutput(SimpleIngredient ingredient) {
		outputs.push_back(ingredient);
	}
	const std::string& SimpleRecipe::getName() {
		return name;
	}
	SimpleRecipe* RecipeRegistry::registerSimpleRecipe(const std::string& name) {
		m_simpleRecipes.push_back({});
		m_simpleRecipes.back().recipeId = m_simpleRecipes.size();
		m_simpleRecipes.back().name = name;
		return &m_simpleRecipes.back();
	}
	SimpleRecipe* RecipeRegistry::getSimpleRecipe(RecipeId recipeId) {
		if (recipeId<1 || recipeId>m_simpleRecipes.size())
			return nullptr;
		return &m_simpleRecipes[recipeId-1];
	}
	SimpleRecipe* RecipeRegistry::getSimpleRecipe(const std::string& name) {
		for (auto& recipe : m_simpleRecipes) {
			if (recipe.name == name) {
				return &recipe;
			}
		}
		return nullptr;
	}
	std::vector<RecipeId> RecipeRegistry::searchInputUsages(ItemType type) {
		std::vector<RecipeId> result;
		for (auto& recipe : m_simpleRecipes) {
			for (auto& ingredient : recipe.inputs) {
				if (ingredient.itemType == type) {
					result.push_back(recipe.recipeId);
					break;
				}
			}
		}
		return result;
	}
	std::vector<RecipeId> RecipeRegistry::searchOutputUsages(ItemType type) {
		std::vector<RecipeId> result;
		for (auto& recipe : m_simpleRecipes) {
			for (auto& ingredient : recipe.outputs) {
				if (ingredient.itemType == type) {
					result.push_back(recipe.recipeId);
					break;
				}
			}
		}
		return result;
	}
	std::vector<SimpleIngredient> RecipeRegistry::getCraftableOutputs() {
		std::vector<SimpleIngredient> result;
		std::unordered_map<ItemType,bool> map;
		for (auto& recipe : m_simpleRecipes) {
			for (auto& ingredient : recipe.outputs) {
				auto find = map.find(ingredient.itemType);
				if (find == map.end()) {
					//result.push_back(ingredient.itemType);
					map[ingredient.itemType] = true;
				}
			}
		}
		return result;
	}
	void RecipeRegistry::serialize() {
		using namespace engone;
		FileWriter file(std::string(getName()) + ".dat");
		if (!file) {
			log::out << log::RED << "RecipeRegistry failed writing file\n";
			return;
		}
		int count = m_simpleRecipes.size();
		file.writeOne(count);
		for (auto& recipe: m_simpleRecipes) {
			file.write(recipe.name);
			// doesn't need to store the inputs and outputs
		}

		log::out << "ReciepRegistry serialized " << count << " recipes\n";
	}
	void RecipeRegistry::deserialize() {
		using namespace engone;
		FileReader file(std::string(getName()) + ".dat");
		if (!file) {
			log::out << log::RED << "RecipeRegistry failed reading file\n";
			return;
		}

		int count;
		file.readOne(count);
		m_simpleRecipes.resize(count);
		for (int i = 0; i < count; i++) {
			file.read(m_simpleRecipes[i].name);
		}

		log::out << "RecipeRegistry loaded " << count << " recipes\n";

	}
}