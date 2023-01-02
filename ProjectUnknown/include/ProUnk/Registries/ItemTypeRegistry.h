#pragma once

#include "ProUnk/Registries/MasterRegistry.h"

#include "ProUnk/Registries/ModelRegistry.h"

namespace prounk {
	typedef int ItemType;
	struct ItemTypeInfo {
		ItemType itemType;
		std::string name; // registered name
		std::string displayName; // default display name for item
		ModelId modelId; // default model
	};
	class ItemTypeRegistry : public Registry {
	public:
		ItemTypeRegistry() : Registry("item_type_registry") {}

		void serialize() override;
		void deserialize() override;

		// may return nullptr;
		const ItemTypeInfo* getType(ItemType type);
		// may return nullptr;
		// name is not the display name
		const ItemTypeInfo* getType(const std::string& name);

		// name is the registered name not the display name!
		ItemType registerType(const std::string& name, ModelId modelId);
		ItemType registerType(const std::string& name, ModelId modelId, const std::string& displayName);

	private:

		ItemType getNewType() { return m_newType++; }
		ItemType m_newType = 1;

		std::unordered_map<std::string, int> m_nameTypes;

		// this will be serialized
		std::vector<ItemTypeInfo> m_itemTypes;
	};
}