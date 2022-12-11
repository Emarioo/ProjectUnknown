#include "ProUnk/DataRegistries/InventoryRegistry.h"

#include "Engone/Utilities/FileUtility.h"

#include "ProUnk/GameApp.h"

namespace prounk {
	
	Item::Item(ItemType type, int count) : m_type(type), m_count(count) {
		auto win = engone::GetActiveWindow();
		World* world = (World*)win->getParent()->getWorld();
		auto info = world->itemTypeRegistry.getType(type);
		if (info) {
			m_name = info->name;
			m_modelId = info->modelId;
		}
	}
	ItemType Item::getType() { return m_type; }
	void Item::setType(ItemType type) { 
		m_type = type;
		auto win = engone::GetActiveWindow();
		World* world = (World*)win->getParent()->getWorld();
		auto info = world->itemTypeRegistry.getType(type);
		if (info) {
			m_name = info->name;
			m_modelId = info->modelId;
		}
	}

	const std::string& Item::getName() { return m_name; }
	void Item::setName(const std::string& name) { m_name = name; }

	int Item::getCount() { return m_count; }
	void Item::setCount(int count) { m_count = count; }

	ModelId Item::getModelId() { return m_modelId; }
	void Item::setModelId(ModelId modelId) { m_modelId = modelId; }

	Inventory* InventoryRegistry::getInventory(int id) {
		if (id == 0) return nullptr;
		return m_inventories[id-1]; // id-1 because 0 is seen as null while 1 is seen as the first element
	}
	int InventoryRegistry::addInventory() {
		m_inventories.push_back(new Inventory());
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
			int itemCount = inv->size();
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
			Inventory* inv = m_inventories[i] = new Inventory();
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