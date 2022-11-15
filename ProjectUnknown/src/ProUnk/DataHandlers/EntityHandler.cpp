#include "ProUnk/DataHandlers/InventoryHandler.h"

#include "Engone/Utilities/FileUtility.h"

namespace prounk {
	//void InventoryHandler::serialize() {
	//	using namespace engone;
	//	FileWriter file("handlerRegistry.dat");

	//	int totalItems = 0;

	//	int count = m_inventories.size();
	//	file.writeOne(count);
	//	for (Inventory* inv : m_inventories) {
	//		int itemCount = inv->size();
	//		file.writeOne(itemCount);
	//		for (int i = 0; i < itemCount; i++) {
	//			Item* item = inv->getItem(i);
	//			if (item) {
	//				file.writeOne(item->getCount());
	//				totalItems++;
	//			}
	//		}
	//	}

	//	log::out << "InventoryHandler serialized " << count << " inventories (total of " << totalItems << " items)\n";
	//}
	//void InventoryHandler::deserialize() {
	//	using namespace engone;
	//	FileReader file("handlerRegistry.dat");

	//	int totalItems = 0;

	//	int count;
	//	file.readOne(count);
	//	m_inventories.resize(count);
	//	for (int i = 0; i < count; i++) {
	//		Inventory* inv = m_inventories[i] = new Inventory();
	//		int itemCount;
	//		file.readOne(itemCount);
	//		for (int i = 0; i < itemCount; i++) {

	//			totalItems++;
	//		}
	//	}
	//	log::out << "InventoryHandler loaded " << count << " inventories (total of " << totalItems << " items)\n";

	//}
}