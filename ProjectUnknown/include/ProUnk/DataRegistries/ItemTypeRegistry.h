#pragma once

#include "ProUnk/DataRegistries/MasterRegistry.h"

namespace prounk {
	typedef int ItemType;
	struct ItemTypeInfo {
		ItemType itemType;
		std::string name;
	};
	class ItemTypeRegistry : public DataRegistry {
	public:
		ItemTypeRegistry() : DataRegistry("item_type_registry") {}

		void serialize() override;
		void deserialize() override;

		const ItemTypeInfo* getType(ItemType type);
		const ItemTypeInfo* getType(const std::string& name);

		ItemType registerType(const std::string& name);

	private:

		ItemType getNewType() { return m_newType++; }
		ItemType m_newType = 1;

		std::unordered_map<std::string, int> m_nameTypes;

		// this will be serialized
		std::vector<ItemTypeInfo> m_itemTypes;
	};
}