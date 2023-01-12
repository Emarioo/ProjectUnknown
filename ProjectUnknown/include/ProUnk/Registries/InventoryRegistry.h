#pragma once

#include "ProUnk/Registries/MasterRegistry.h"
#include "ProUnk/Registries/ModelRegistry.h"
#include "ProUnk/Registries/ItemTypeRegistry.h"

#include "Engone/Assets/ModelAsset.h"

namespace prounk {
	class Item {
	public:
		Item() = default;
		Item(ItemType type, uint32_t count);
		//Item(ItemType type, int count, const std::string& name, ModelId modelId) : m_count(count), m_name(name), m_modelId(modelId) {}

		ItemType getType();
		void setType(ItemType type);

		const std::string& getDisplayName();
		void setDisplayName(const std::string& name);

		uint32_t getCount();
		void setCount(uint32_t count);

		ModelId getModelId();
		void setModelId(ModelId modelId);

		void setComplexData(int dataIndex);
		int getComplexData();

		bool sameAs(Item& item);

		// Makes a copy of the item including independent complex data.
		// This item is unaffected.
		// if count argument is -1 then count of outItem is count of this item.
		// Otherwise count of outItem is count argument
		// function fails if this item and outItem are different or duplication of the item failed.
		// outItem is never overwritten and function is safe.
		bool copy(Item& outItem, uint32_t count=-1);

		// returns false if function fails. (this item and target item may be different types)
		// nothing is changed if function failed.
		// this item is moved to target if possible.
		// count desscribes how much to move. -1 moves everything.
		bool transfer(Item& target, uint32_t count=-1);

		bool swap(Item& target);

	private:
		ItemType m_type = 0;
		uint32_t m_count = 0;
		//std::string m_name;
		std::string m_displayName;
		ModelId m_modelId = 0;

		int m_complexDataIndex=0;

		friend class InventoryRegistry;
	};
	class Inventory {
	public:
		Inventory() = default;
		//~Inventory();

		//-- Potential functions
		// sort name, count...

		// no bound check
		Item& getItem(int slotIndex);

		// give item to inventory
		// item is put in inventory if function returns true
		bool transferItem(Item& item);

		// Returns -1 if it failed
		// Fail happens if slot wasn't found-
		// Function acts as findEmptySlot if item's type is 0.
		uint32_t findAvailableSlot(Item& item);
		// Returns -1 if it failed
		// Fail happens if slot wasn't found
		uint32_t findEmptySlot();

		// take item from inventory
		// if true is returned, result is stored in outItem
		// Initial data in outItem is overwritten without any considerations.
		//bool takeItem(int slotIndex, Item& outItem);
		// USE 'getItem(slotIndex).transfer(outItem)' instead

		std::vector<Item>& getList();
		// Will fail if items are lost during resize.
		// This is to prevent where players lose items due to bugs.
		bool resizeSlots(int newSlotSize);
		// "lost items" during resize will be put into outItems.
		// outItems can be nullptr if you want to ignore the lost items.
		void resizeSlots(int newSlotSize, std::vector<Item>* outItems);
		int getSlotSize();

	private:

		std::vector<Item> m_slots;
	};
	class InventoryRegistry : public Registry {
	public:
		InventoryRegistry() : Registry("inventory_registry") {}

		// serialization should perhaps not load all inventories in the world at once.
		// maybe load required inventories or so. Load upon request perhaps
		// instead of a long list of inventories split them into batches.
		// better serializing would be loading a whole inventory with one read instead
		// of one read for each item. all the items would be read into the correct location at once.
		void serialize() override;
		void deserialize() override;

		// no bounds check
		Inventory* getInventory(int id);
		int createInventory();

	private:

		std::vector<Inventory*> m_inventories;
	};
}