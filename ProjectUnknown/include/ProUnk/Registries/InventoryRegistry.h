#pragma once

#include "ProUnk/DataRegistries/MasterRegistry.h"
#include "ProUnk/DataRegistries/ModelRegistry.h"
#include "ProUnk/DataRegistries/ItemTypeRegistry.h"

#include "Engone/Assets/ModelAsset.h"

namespace prounk {
	class Item {
	public:
		Item() = default;
		Item(ItemType type, int count);
		//Item(ItemType type, int count, const std::string& name, ModelId modelId) : m_count(count), m_name(name), m_modelId(modelId) {}

		ItemType getType();
		void setType(ItemType type);

		const std::string& getName();
		void setName(const std::string& name);

		int getCount();
		void setCount(int count);

		ModelId getModelId();
		void setModelId(ModelId modelId);

	private:
		ItemType m_type=0;
		int m_count = 0;
		std::string m_name;
		ModelId m_modelId=0;

		friend class InventoryRegistry;
	};
	class Inventory {
	public:
		Inventory() = default;
		//~Inventory();

		//-- Potential functions
		// transferItem
		// addItem
		// takeItem
		// sort name, count...
		// 

		// no bound check
		Item& getItem(int index) { return m_items[index]; }
		void addItem(Item item) { m_items.push_back(item); }
		void removeItem(int index) { m_items.erase(m_items.begin()+index); }

		std::vector<Item>& getList() { return m_items; };

		int size() { return m_items.size(); }

	private:

		std::vector<Item> m_items;
	};
	class InventoryRegistry : public DataRegistry {
	public:
		InventoryRegistry() : DataRegistry("inventory_registry") {}

		// serialization should perhaps not load all inventories in the world at once.
		// maybe load required inventories or so. Load upon request perhaps
		// instead of a long list of inventories split them into batches.
		// better serializing would be loading a whole inventory with one read instead
		// of one read for each item. all the items would be read into the correct location at once.
		void serialize() override;
		void deserialize() override;

		// no bounds check
		Inventory* getInventory(int id);
		int addInventory();

	private:

		std::vector<Inventory*> m_inventories;
	};
}