#include "ProUnk/DataRegistries/InventoryRegistry.h"

void DataRegistryTest() {
	using namespace prounk;
	
	MasterRegistry registry;
	InventoryRegistry invHandler;

	registry.deserialize();

	DataRegistry* ret = registry.getRegistry("nops");
	ret = registry.getRegistry(201);
	ret = registry.getRegistry("inventory_handler");

	RegistryId id = registry.registerRegistry(&invHandler);

	ret = registry.getRegistry("inventory_handler");

	ret = registry.getRegistry(id);

	registry.serialize();

	system("pause");
}