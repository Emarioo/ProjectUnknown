#include "ProUnk/DataHandlers/InventoryHandler.h"

void DataHandlerTest() {
	using namespace prounk;
	
	DataHandlerRegistry registry;
	InventoryHandler invHandler;

	registry.deserialize();

	DataHandler* ret = registry.getHandler("nops");
	ret = registry.getHandler(201);
	ret = registry.getHandler("inventory_handler");

	HandlerId id = registry.registerHandler(&invHandler);

	ret = registry.getHandler("inventory_handler");

	ret = registry.getHandler(id);

	registry.serialize();

	system("pause");
}