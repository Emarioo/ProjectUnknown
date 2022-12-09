#include "ProUnk/DataRegistrys/InventoryRegistry.h"

#include "Engone/Utilities/FileUtility.h"

namespace prounk {

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
		FileWriter file("InventoryRegistry.dat");
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
		FileReader file("InventoryRegistry.dat");
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