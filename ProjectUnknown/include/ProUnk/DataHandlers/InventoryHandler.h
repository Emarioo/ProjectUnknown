#pragma once

#include "ProUnk/DataHandlers/DataHandlerRegistry.h"
#include "Engone/Assets/ModelAsset.h"

namespace prounk {
	class Item {
	public:
		Item() = default;
		//Item(const std::string& name);
		//Item(const std::string& name, int count);

		const std::string& getName() { return m_name; }
		void setName(const std::string& name) { m_name = name; }

		int getCount() { return m_count; }
		void setCount(int count) { m_count = count; }

		engone::ModelAsset* getModel() { return m_model; }
		void setModel(engone::ModelAsset* model) { m_model = model; }

	private:
		int m_count = 0;
		std::string m_name;
		engone::ModelAsset* m_model;

		//HandlerId m_handlerId = 0;
		//int m_dataIndex;

	};
	class Inventory {
	public:
		Inventory() = default;
		~Inventory();

		//-- Potential functions
		// transferItem
		// addItem
		// takeItem
		// sort name, count...
		// 

		// no bound check
		Item* getItem(int index) { return m_items[index]; }

		int size() { return m_items.size(); }

	private:

		std::vector<Item*> m_items; // all slots
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

	private:

		std::vector<Inventory*> m_inventories;
	};
}