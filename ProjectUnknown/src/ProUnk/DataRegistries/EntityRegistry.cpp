#include "ProUnk/DataRegistrys/EntityHandler.h"

#include "Engone/Utilities/FileUtility.h"

namespace prounk {

	EntityHandler::Entry& EntityHandler::getEntry(int id) {
		return m_entries[id-1];
	}
	int EntityHandler::addEntry() {
		m_entries.push_back({});
		return m_entries.size();
	}

	void EntityHandler::serialize() {
		using namespace engone;
		//FileWriter file("handlerRegistry.dat");
		//if (!file)
			//return;

		//int count = m_inventories.size();
		//file.writeOne(count);
		//for (Inventory* inv : m_inventories) {
		//	int itemCount = inv->size();
		//	file.writeOne(itemCount);
		//	for (int i = 0; i < itemCount; i++) {
		//		Item* item = inv->getItem(i);
		//		if (item) {
		//			file.writeOne(item->getCount());
		//			totalItems++;
		//		}
		//	}
		//}

		//log::out << "InventoryRegistry serialized " << count << " inventories (total of " << totalItems << " items)\n";
	}
	void EntityHandler::deserialize() {
		using namespace engone;
		//FileReader file("handlerRegistry.dat");
		//if (!file)
		//	return;

		//int count;
		//file.readOne(count);
		//m_inventories.resize(count);
		//for (int i = 0; i < count; i++) {
		//	Inventory* inv = m_inventories[i] = new Inventory();
		//	int itemCount;
		//	file.readOne(itemCount);
		//	for (int i = 0; i < itemCount; i++) {

		//		totalItems++;
		//	}
		//}
		//log::out << "InventoryRegistry loaded " << count << " inventories (total of " << totalItems << " items)\n";

	}
}