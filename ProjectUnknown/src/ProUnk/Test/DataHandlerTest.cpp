#include "ProUnk/DataRegistrys/InventoryRegistry.h"

void DataRegistryTest() {
	using namespace prounk;
	
	MasterRegistry registry;
	InventoryRegistry invHandler;

	registry.deserialize();

	DataRegistry* ret = registry.getHandler("nops");
	ret = registry.getHandler(201);
	ret = registry.getHandler("inventory_handler");

	RegistryId id = registry.registerHandler(&invHandler);

	ret = registry.getHandler("inventory_handler");

	ret = registry.getHandler(id);

	registry.serialize();

	system("pause");
}