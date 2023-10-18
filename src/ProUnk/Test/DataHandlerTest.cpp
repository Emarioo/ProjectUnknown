#include "ProUnk/Registries/InventoryRegistry.h"

void RegistryTest() {
	using namespace prounk;
	
	MasterRegistry registry;
	InventoryRegistry invHandler;

	registry.deserialize();

	Registry* ret = registry.getRegistry("nops");
	ret = registry.getRegistry(201);
	ret = registry.getRegistry("inventory_handler");

	RegistryId id = registry.registerRegistry(&invHandler);

	ret = registry.getRegistry("inventory_handler");

	ret = registry.getRegistry(id);

	registry.serialize();

	system("pause");
}