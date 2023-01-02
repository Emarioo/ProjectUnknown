#include "ProUnk/Registries/InventoryRegistry.h"

#include "Engone/Utilities/FileUtility.h"

#include "ProUnk/GameApp.h"

namespace prounk {
	
	Item::Item(ItemType type, int count) : m_type(type), m_count(count) {
		auto app = (GameApp*)engone::GetActiveWindow()->getParent();
		if (!app->getActiveSession())
			return;
		auto session = app->getActiveSession();
		auto info = session->itemTypeRegistry.getType(type);

		if (!info)
			return;

		m_displayName = info->displayName;
		m_modelId = info->modelId;
	}
	ItemType Item::getType() { return m_type; }
	void Item::setType(ItemType type) { 
		m_type = type;
		auto app = (GameApp*)engone::GetActiveWindow()->getParent();
		if (!app->getActiveSession())
			return;
		auto session = app->getActiveSession();

		auto info = session->itemTypeRegistry.getType(type);
		if (!info)
			return;

		m_displayName = info->displayName;
		m_modelId = info->modelId;
	}

	const std::string& Item::getDisplayName() { return m_displayName; }
	void Item::setDisplayName(const std::string& name) { m_displayName = name; }

	uint32_t Item::getCount() { return m_count; }
	void Item::setCount(uint32_t count) { m_count = count; }

	ModelId Item::getModelId() { return m_modelId; }
	void Item::setModelId(ModelId modelId) { m_modelId = modelId; }

	void Item::setComplexData(int dataIndex) { m_complexDataIndex = dataIndex; }
	int Item::getComplexData() { return m_complexDataIndex; }
	
	bool Item::copy(Item& outItem, uint32_t count) {
		if (count == 0 || getType() == 0)
			return false; // cannot copy nothing

		if(outItem.getType() != 0) {
			if (!sameAs(outItem))
				return false; // cannot overwrite outItem
			
			if (count == -1) {
				outItem.setCount(outItem.getCount() + getCount());
			} else {
				outItem.setCount(outItem.getCount() + count);
			}
			return true;
		}
		// outItem is nothing, a copy of the item is required

		if (getComplexData()) {
			Session* session = ((GameApp*)engone::GetActiveWindow()->getParent())->getActiveSession();
			auto data = session->complexDataRegistry.getData(getComplexData());
			ComplexData* newData = data->copy();

			if (!newData)
				return false; // copy failed.

			outItem.m_complexDataIndex = newData->getDataIndex();
		}
		outItem.m_type = m_type;
		outItem.m_displayName = m_displayName;
		outItem.m_modelId = m_modelId;

		if (count == -1) {
			outItem.m_count = getCount();
		} else {
			outItem.setCount(count);
		}
		return true;
	}
	bool Item::sameAs(Item& item) {
		if (getType() == 0 && item.getType() == 0)
			return true;
		if (getType() != item.getType())
			return false;

		// maybe check max count in a stack too?
		Session* session = ((GameApp*)engone::GetActiveWindow()->getParent())->getActiveSession();

		if (getComplexData() == 0 && item.getComplexData() != 0 ||
			getComplexData() != 0 && item.getComplexData() == 0) // if one item has complex data and the other hasn't
			return false;
		if (getComplexData() != 0 && item.getComplexData() != 0) { // check if the items have the same complex data
			ComplexData* d0 = session->complexDataRegistry.getData(getComplexData());
			ComplexData* d1 = session->complexDataRegistry.getData(item.getComplexData());

			if (!d0->sameAs(d1))
				return false;
		}
		return true;
	}
	bool Item::transfer(Item& target, uint32_t count) {
		if (getType() == 0||count==0)
			return false; // cannot transfer nothing
		if (count != -1 && getCount() < count)
			return false; // cannot transfer more than available

		if (target.getType() == 0) {
			if (count == -1||getCount()==count) {
				// full transfer
				target = *this;
				*this = {};
				return true;
			}
			// split
			copy(target, count);
			setCount(m_count - count);
			return true;
		}

		if (!sameAs(target))
			return false; // cannot transfer when items are different

		if (count == -1||getCount()==count) {
			// full transfer
			target.setCount(target.getCount() + getCount());

			// Could be potential leakage of ComplexData but this should fix it
			Session* session = ((GameApp*)engone::GetActiveWindow()->getParent())->getActiveSession();
			session->complexDataRegistry.unregisterData(getComplexData());

			*this = {};
			return true;
		}

		// split
		target.setCount(target.getCount()+count);
		setCount(getCount() - count);
		return true;
	}
	bool Item::swap(Item& target) {
		Item temp = target;
		target = *this;
		*this = temp;
		return true;
	}
	Item& Inventory::getItem(int slotIndex) {
		return m_slots[slotIndex];
	}
	std::vector<Item>& Inventory::getList() {
		return m_slots;
	}
	bool Inventory::resizeSlots(int newSlotSize) {
		// check if items would be lost
		for (int i = m_slots.size()-1; i >= newSlotSize;i--) {
			if (m_slots[i].getType() != 0)
				return false; // item is lost, resize failed
		}
		m_slots.resize(newSlotSize, Item());
		return true;
	}
	void Inventory::resizeSlots(int newSlotSize, std::vector<Item>* outItems) {
		if (outItems) {
			// add lost items to the list
			for (int i = m_slots.size() - 1; i >= newSlotSize; i--) {
				if (m_slots[i].getType() != 0) {
					outItems->push_back(m_slots[i]);
				}
			}
		}
		m_slots.resize(newSlotSize, Item());
	}
	int Inventory::getSlotSize() {
		return m_slots.size();
	}
	bool Inventory::transferItem(Item& item) {
		if (item.getType() == 0)
			return false;
		// find empty slot
		int index = -1;
		for (int i = 0; i < m_slots.size(); i++) {
			Item& item2 = m_slots[i];

			if (item2.getType() == 0) {
				index = i;
				break;
			}

			bool yes = item.sameAs(item2);
			if (yes) {
				index = i;
				break;
			}
		}
		if (index == -1)
			return false;

		if (m_slots[index].getType() == 0) {
			m_slots[index] = item;
		} else {
			m_slots[index].setCount(m_slots[index].getCount() + item.getCount());
			// name, modelId can stay for now
		}
		return true;
	}
	uint32_t Inventory::findAvailableSlot(Item& item) {
		if (item.getType() == 0)
			return findEmptySlot();

		uint32_t emptySlot = -1;
		for (int i = 0; i < m_slots.size(); i++) {
			Item& item2 = m_slots[i];

			if (item2.getType() == 0&&emptySlot==-1) {
				emptySlot = i;
				continue;
			}

			bool yes = item.sameAs(item2);
			if (yes) {
				return i;
			}
		}
		return emptySlot;
	}
	uint32_t Inventory::findEmptySlot() {
		for (int i = 0; i < m_slots.size(); i++) {
			Item& item = m_slots[i];

			if (item.getType() == 0) {
				return i;
			}
		}
		return -1;
	}
	//bool Inventory::takeItem(int slotIndex, Item& outItem) {
	//	if (m_slots[slotIndex].getType() == 0)
	//		return false;
	//	
	//	outItem = m_slots[slotIndex];
	//	m_slots[slotIndex] = Item();
	//	return true;
	//}
	Inventory* InventoryRegistry::getInventory(int id) {
		if (id == 0) return nullptr;
		return m_inventories[id-1]; // id-1 because 0 is seen as null while 1 is seen as the first element
	}
	int InventoryRegistry::createInventory() {
		m_inventories.push_back(ALLOC_NEW(Inventory)());
		return m_inventories.size();
	}
	void InventoryRegistry::serialize() {
		using namespace engone;
		FileWriter file("inventoryRegistry.dat");
		if (!file)
			return;
		int totalItems = 0;

		int count = m_inventories.size();
		file.writeOne(count);
		for (Inventory* inv : m_inventories) {
			int itemCount = inv->getSlotSize();
			file.writeOne(itemCount);
			for (int i = 0; i < itemCount;i++) {
				Item& item = inv->getItem(i);
				file.writeOne(item.m_count);
				totalItems++;
			}
		}

		log::out << "InventoryRegistry serialized " << count << " inventories (total of "<<totalItems<<" items)\n";
	}
	void InventoryRegistry::deserialize() {
		using namespace engone;
		FileReader file("inventoryRegistry.dat");
		if (!file)
			return;
		int totalItems = 0;

		int count;
		file.readOne(count);
		m_inventories.resize(count);
		for (int i = 0; i < count; i++) {
			Inventory* inv = m_inventories[i] = ALLOC_NEW(Inventory)();
			int itemCount;
			file.readOne(itemCount);
			inv->getList().resize(itemCount);
			for (int i = 0; i < itemCount; i++) {
				Item& item = inv->getItem(i);
				file.readOne(item.m_count);
				totalItems++;
			}
		}
		log::out << "InventoryRegistry loaded " << count << " inventories (total of " << totalItems << " items)\n";

	}
}