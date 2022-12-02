#pragma once

#include "ProUnk/DataHandlers/DataHandlerRegistry.h"
#include "Engone/Assets/ModelAsset.h"

#include "ProUnk/DataHandlers/ModelHandler.h"

namespace prounk {
	class Item {
	public:
		Item() = default;
		Item(int count, const std::string& name, ModelId modelId) : m_count(count), m_name(name), m_modelId(modelId) {}
		//Item(const std::string& name);
		//Item(const std::string& name, int count);

		const std::string& getName() { return m_name; }
		void setName(const std::string& name) { m_name = name; }

		int getCount() { return m_count; }
		void setCount(int count) { m_count = count; }

		//engone::ModelAsset* getModelId() { return m_modelId; }
		ModelId getModelId() { return m_modelId; }
		// not sure about this function
		void setModelId(ModelId modelId){ m_modelId = modelId; }

	private:
		int m_count = 0;
		std::string m_name;
		ModelId m_modelId;
		//engone::ModelAsset* m_model;

		//HandlerId m_handlerId = 0;
		//int m_dataIndex;

		friend class InventoryHandler;
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

		std::vector<Item>& getList() { return m_items; };

		int size() { return m_items.size(); }

	private:

		std::vector<Item> m_items; // all slots
	};
	class InventoryHandler : public DataHandler {
	public:
		InventoryHandler() : DataHandler("inventory_handler") {}

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