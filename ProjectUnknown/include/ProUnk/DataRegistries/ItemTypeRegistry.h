#pragma once

#include "ProUnk/DataRegistries/MasterRegistry.h"

#include "ProUnk/DataRegistries/ModelRegistry.h"

namespace prounk {
	typedef int ItemType;
	struct ItemTypeInfo {
		ItemType itemType;
		std::string name; // default name
		ModelId modelId; // default model
	};
	class ItemTypeRegistry : public DataRegistry {
	public:
		ItemTypeRegistry() : DataRegistry("item_type_registry") {}

		void serialize() override;
		void deserialize() override;

		// may return nullptr;
		const ItemTypeInfo* getType(ItemType type);
		// may return nullptr;
		const ItemTypeInfo* getType(const std::string& name);

		ItemType registerType(const std::string& name, ModelId modelId);

	private:

		ItemType getNewType() { return m_newType++; }
		ItemType m_newType = 1;

		std::unordered_map<std::string, int> m_nameTypes;

		// this will be serialized
		std::vector<ItemTypeInfo> m_itemTypes;
	};
}