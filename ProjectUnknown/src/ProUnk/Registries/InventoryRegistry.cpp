#include "ProUnk/Registries/InventoryRegistry.h"

// #include "Engone/Utilities/FileUtility.h"

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
	Item::Item(const ItemTypeInfo* type, int count) : m_type(type->itemType), m_count(count) {
		m_displayName = type->displayName;
		m_modelId = type->modelId;
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

	int Item::getCount() { return m_count; }
	void Item::setCount(int count) { m_count = count; }

	ModelId Item::getModelId() { return m_modelId; }
	void Item::setModelId(ModelId modelId) { m_modelId = modelId; }

	//void Item::setComplexData(int dataIndex) { m_complexDataIndex = dataIndex; }
	//int Item::getComplexData() { return m_complexDataIndex; }
	ComplexData* Item::getComplexData() {
		return &m_complexData;
	}
	
	bool Item::copy(Item& outItem, int count) {
		if (getType() == 0)
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

		// deprecated code, we always have complex data. not using registry at the moment
		//if (getComplexData()) {
		//	Session* session = ((GameApp*)engone::GetActiveWindow()->getParent())->getActiveSession();
		//	auto data = session->complexDataRegistry.getData(getComplexData());
		//	ComplexData* newData = data->copy();

		//	if (!newData)
		//		return false; // copy failed.

		//	outItem.m_complexDataIndex = newData->getDataIndex();
		//}
		outItem.m_type = m_type;
		outItem.m_displayName = m_displayName;
		outItem.m_modelId = m_modelId;
		outItem.m_complexData = m_complexData;

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

		if ((!getComplexData() && item.getComplexData()) ||
			(getComplexData() && !item.getComplexData())) // if one item has complex data and the other hasn't
			return false;

		//if (getComplexData() == 0 && item.getComplexData() != 0 ||
		//	getComplexData() != 0 && item.getComplexData() == 0) // if one item has complex data and the other hasn't
		//	return false;
		if (getComplexData() && item.getComplexData()) { // check if the items have the same complex data
			//ComplexData* d0 = session->complexDataRegistry.getData(getComplexData());
			//ComplexData* d1 = session->complexDataRegistry.getData(item.getComplexData());

			ComplexData* d0 = getComplexData();
			ComplexData* d1 = item.getComplexData();
			return d0->sameAs(d1);
		}
		return true;
	}
	bool Item::transfer(Item& target, int count) {
		using namespace engone;
		if (count == -1)
			count = getCount();

		if (getType() == 0||count==0)
			return false; // cannot transfer nothing
		if (getCount() < count)
			return false; // cannot transfer more than available

		// Check max stack size and available space
		auto type = ((GameApp*)engone::GetActiveWindow()->getParent())->getActiveSession()->itemTypeRegistry.getType(getType());
		int transferAmount = count;
		if (target.getType() == 0) {
			if (count > type->maxStack)
				transferAmount = type->maxStack;
		} else {
			if (count > type->maxStack-target.getCount())
				transferAmount = type->maxStack - target.getCount();
		}
		if (transferAmount <= 0)
			return false;

		// Do transfer
		if (target.getType() == 0) {
			if (getCount() == transferAmount) {
				// full transfer
				target = *this;
				*this = {};
				return true;
			}
			// split
			copy(target, transferAmount);
			setCount(getCount() - transferAmount);
			return true;
		}

		if (!sameAs(target))
			return false; // cannot transfer when items are different

		if (getCount() == transferAmount) {
			// full transfer
			target.setCount(target.getCount() + transferAmount);

			// deprecated since complex data exists inside the item for now.
			// Could be potential leakage of ComplexData but this should fix it
			//Session* session = ((GameApp*)engone::GetActiveWindow()->getParent())->getActiveSession();
			//session->complexDataRegistry.unregisterData(getComplexData());

			*this = {};
			return true;
		}

		// split
		target.setCount(target.getCount() + transferAmount);
		setCount(getCount() - transferAmount);
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
	int Inventory::transferItem(Item& item) {
		if (item.getType() == 0)
			return false;
		
		int transferAmount =0;

		// find same item types and fill them
		for (int i = 0; i < m_slots.size(); i++) {
			Item& target = m_slots[i];

			if (target.getType() == 0) {
				continue;
			}

			bool yes = item.sameAs(target);
			if (!yes) continue;

			Session* session = ((GameApp*)engone::GetActiveWindow()->getParent())->getActiveSession();
			auto type = session->itemTypeRegistry.getType(target.getType());
			
			int emptySpace = (int)type->maxStack - (int)target.getCount();
			if (emptySpace <= 0) {
				continue;
			}

			if (item.getCount() > emptySpace) {
				target.setCount(target.getCount() + emptySpace);
				transferAmount += emptySpace;
				item.setCount(item.getCount() - emptySpace);
			} else {
				target.setCount(target.getCount() + item.getCount());
				transferAmount += item.getCount();
				item.setCount(0);
				return transferAmount;
			}
		}

		// find empty slots and fill them
		for (int i = 0; i < m_slots.size(); i++) {
			Item& target = m_slots[i];

			if (target.getType() == 0) {
				item.copy(target,0); // could do target = item if just one stack of items are transferred successfully.

				Session* session = ((GameApp*)engone::GetActiveWindow()->getParent())->getActiveSession();
				auto type = session->itemTypeRegistry.getType(item.getType());
				if (item.getCount() > type->maxStack) {
					target.setCount(type->maxStack);
					transferAmount += type->maxStack;
					item.setCount(item.getCount() - type->maxStack);
				} else {
					target.setCount(target.getCount() + item.getCount());
					transferAmount += item.getCount();
					item.setCount(0);
					return transferAmount;
				}
			}
		}

		return transferAmount;
	}
	uint32_t Inventory::findAvailableSlot(Item& item) {
		if (item.getType() == 0)
			return findEmptySlot();

		uint32_t emptySlot = -1;
		for (int i = 0; i < m_slots.size(); i++) {
			Item& target = m_slots[i];

			if (target.getType() == 0&&emptySlot==-1) {
				emptySlot = i;
				continue;
			}

			bool yes = item.sameAs(target);
			if (!yes)
				continue;
	
			Session* session = ((GameApp*)engone::GetActiveWindow()->getParent())->getActiveSession();
			auto type = session->itemTypeRegistry.getType(target.getType());

			if (target.getCount() >= type->maxStack)
				continue;

			return i;
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
	uint32 InventoryRegistry::createInventory() {
		return m_inventories.add({}) + 1;
	}
	Inventory* InventoryRegistry::getInventory(uint32 dataIndex) {
		if (dataIndex == 0) return nullptr;
		return m_inventories.get(dataIndex - 1);
	}
	void InventoryRegistry::destroyInventory(uint32 dataIndex) {
		// Todo: Unregister ComplexData of all the items?
		m_inventories.remove(dataIndex - 1);
	}
	void InventoryRegistry::serialize() {
		using namespace engone;

		log::out << log::RED << "InventoryRegistry : serialize not implemented\n";

		//FileWriter file("inventoryRegistry.dat");
		//if (!file)
		//	return;
		//int totalItems = 0;

		//int count = m_inventories.size();
		//file.writeOne(count);
		//for (Inventory* inv : m_inventories) {
		//	int itemCount = inv->getSlotSize();
		//	file.writeOne(itemCount);
		//	for (int i = 0; i < itemCount;i++) {
		//		Item& item = inv->getItem(i);
		//		file.writeOne(item.m_count);
		//		totalItems++;
		//	}
		//}

		//log::out << "InventoryRegistry serialized " << count << " inventories (total of "<<totalItems<<" items)\n";
	}
	void InventoryRegistry::deserialize() {
		using namespace engone;
		log::out << log::RED << "InventoryRegistry : deserialize not implemented\n";

		//FileReader file("inventoryRegistry.dat");
		//if (!file)
		//	return;
		//int totalItems = 0;

		//int count;
		//file.readOne(count);
		//m_inventories.resize(count);
		//for (int i = 0; i < count; i++) {
		//	Inventory* inv = m_inventories[i] = ALLOC_NEW(Inventory)();
		//	int itemCount;
		//	file.readOne(itemCount);
		//	inv->getList().resize(itemCount);
		//	for (int i = 0; i < itemCount; i++) {
		//		Item& item = inv->getItem(i);
		//		file.readOne(item.m_count);
		//		totalItems++;
		//	}
		//}
		//log::out << "InventoryRegistry loaded " << count << " inventories (total of " << totalItems << " items)\n";

	}
}