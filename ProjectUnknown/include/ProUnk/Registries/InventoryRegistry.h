#pragma once

#include "ProUnk/Registries/MasterRegistry.h"
#include "ProUnk/Registries/ModelRegistry.h"
#include "ProUnk/Registries/ItemTypeRegistry.h"
#include "ProUnk/Registries/ComplexDataRegistry.h"

#include "Engone/Assets/ModelAsset.h"

#include "Engone/Structures/FrameArray.h"

namespace prounk {
	class Item {
	public:
		Item() = default;
		Item(ItemType type, int count);
		Item(const ItemTypeInfo* type, int count);
		//Item(ItemType type, int count, const std::string& name, ModelId modelId) : m_count(count), m_name(name), m_modelId(modelId) {}

		ItemType getType();
		void setType(ItemType type);

		const std::string& getDisplayName();
		void setDisplayName(const std::string& name);

		int getCount();
		// can override max stack size
		void setCount(int count);

		ModelId getModelId();
		void setModelId(ModelId modelId);

		// deprecated?
		//void setComplexData(int dataIndex);
		//int getComplexData();

		// Do not change the return value of this function when using registries.
		// easily getting the ptr to complex data is useful.
		ComplexData* getComplexData();

		bool sameAs(Item& item);

		// Makes a copy of the item including independent complex data.
		// This item is unaffected.
		// if count argument is -1 then count of outItem is count of this item.
		// Otherwise count of outItem is count argument
		// function fails if this item and outItem are different or duplication of the item failed.
		// outItem is never overwritten and function is safe.
		bool copy(Item& outItem, int count=-1);

		// Returns false if it failed. Nothing is changed if function fails.
		// This item is moved to target if possible.
		// Count desscribes how much to move. -1 moves whole stack.
		// Stack size can limit how many items that are moved.
		bool transfer(Item& target, int count=-1);

		bool swap(Item& target);

	private:
		ItemType m_type = 0;
		// not using uint32_t to avoid bugs when doing - < > operations. You may forget to cast uint32_t to int
		int m_count = 0;
		//std::string m_name;
		std::string m_displayName;
		ModelId m_modelId = 0;

		ComplexData m_complexData; // <- temporary? use registry instead?

		//int m_complexDataIndex=0;

		friend class InventoryRegistry;
	};
	class Inventory {
	public:
		Inventory() = default;
		//~Inventory();

		//-- Potential functions
		// sort name, count...

		// no bound check
		Item* getItem(int slotIndex);

		// give item to inventory
		// item is put in inventory if function returns true
		// returns the amount of moved items. Zero if it failed or no items to move.
		int transferItem(Item* item);

		// Returns -1 if it failed
		// Fail happens if slot wasn't found.
		// prioritizes slots of the same type that aren't full. Empty slots are chosen otherwise.
		// Function acts as findEmptySlot if item's type is 0.
		// slots with maximum stack size are skipped.
		// rename function to findNonFullSlot?
		uint32_t findAvailableSlot(Item* item);
		// Returns -1 if it failed
		// Fail happens if slot wasn't found
		uint32_t findEmptySlot();

		engone::Array& getList();
		// 
		// "lost items" during resize will be put into outItems.
		// outItems can be nullptr if you want to ignore the lost items.
		bool resizeSlots(int newSlotSize, std::vector<Item>* outItems);
		int getSlotSize();

		// Will fail if items are lost during resize.
		// This is to prevent where players lose items due to bugs.
		//bool resizeSlots(int newSlotSize);

	private:
		engone::Array m_slots{sizeof(Item),ALLOC_TYPE_GAME_MEMORY};
		//std::vector<Item> m_slots;
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
		uint32 createInventory(Inventory** outPtr = nullptr);
		Inventory* getInventory(uint32 dataIndex);
		void destroyInventory(uint32 dataIndex);

	private:

		engone::FrameArray m_inventories{sizeof(Inventory),64,ALLOC_TYPE_GAME_MEMORY };
		//engone::FrameArray m_inventories{sizeof(Inventory),64,ALLOC_TYPE_HEAP };
	};
}