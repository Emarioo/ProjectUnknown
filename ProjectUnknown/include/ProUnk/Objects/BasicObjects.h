#pragma once

#include "Engone/Utilities/RandomUtility.h"

#include "ProUnk/World/Session.h"
#include "ProUnk/Combat/CombatData.h"

namespace prounk {
	enum BasicObjectType : uint32 {
		OBJECT_CREATURE=0x0100,// inv, combat data, model
		OBJECT_DUMMY,
		OBJECT_PLAYER,

		OBJECT_TERRAIN=0x0200, // model
		
		OBJECT_TRIGGER=0x0400,

		OBJECT_ITEM=0x0800, // item
		
		OBJECT_WEAPON=0x1000 // model, combat data
	};
	bool IsObject(engone::EngineObject* obj, BasicObjectType type);
	const char* to_string(BasicObjectType value);
	engone::Logger& operator<<(engone::Logger& log, BasicObjectType value);

	//-- Some useful functions
	
	bool HasCombatData(int type);
	void DropInventory(engone::EngineObject* object, float shock=1);
	bool IsDead(engone::EngineObject* object);
	CombatData* GetCombatData(engone::EngineObject* object);
	Inventory* GetInventory(engone::EngineObject* object);
	Session* GetSession(engone::EngineObject* object);
	
	engone::EngineObject* GetHeldObject(engone::EngineObject* object);
	
	// object should have an inventory
	// void DropAllItems(engone::EngineObject* object);
	//-- Creation functions

	//engone::EngineObject* CreateObject(int type, Dimension* dimension, engone::UUID uuid = 0);
	// Ground and object cannot be nullptr. Function should work for most objects.
	// Perhaps not player since it may have allocated some specific memory. It shouldn't allocate this memory though.
	// That should be moved elsewhere.
	void DeleteObject(Dimension* dimension, engone::EngineObject* object);

	//engone::EngineObject* CreateSword(Dimension* dimension, engone::UUID uuid = 0);
	engone::EngineObject* CreateTerrain(Dimension* dimension, const std::string& modelName, engone::UUID uuid = 0);
	// Creates a weapon which the player or an enemy can hold.
	engone::EngineObject* CreateWeapon(Dimension* dimension, const std::string& modelName, engone::UUID uuid = 0);

	// Creates an item you can pickup.
	engone::EngineObject* CreateItem(Dimension* dimension, Item& item, engone::UUID uuid = 0);
	engone::EngineObject* CreateTrigger(Dimension* dimension, glm::vec3 size, engone::UUID uuid = 0);

	engone::EngineObject* CreateDummy(Dimension* dimension, engone::UUID uuid = 0);
	engone::EngineObject* CreatePlayer(Dimension* dimension, engone::UUID uuid = 0);

}