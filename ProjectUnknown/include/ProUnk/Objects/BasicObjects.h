#pragma once

#include "Engone/Utilities/RandomUtility.h"

#include "ProUnk/World.h"

namespace prounk {
	enum BasicObjectType : int {
		OBJECT_DUMMY=0,
		OBJECT_SWORD,
		OBJECT_TERRAIN,
		OBJECT_PLAYER
	};

	engone::EngineObject* CreateObject(int type, World* world, engone::UUID uuid = 0);
	// Ground and object cannot be nullptr. Function should work for most objects.
	// Perhaps not player since it may have allocated some specific memory. It shouldn't allocate this memory though.
	// That should be moved elsewhere.
	void DeleteObject(World* world, engone::EngineObject* object);
	engone::EngineObject* CreateDummy(World* world, engone::UUID uuid = 0);
	engone::EngineObject* CreateSword(World* world, engone::UUID uuid = 0);
	engone::EngineObject* CreateTerrain(World* world, engone::UUID uuid = 0);

	// Player from a connection. not the player you control
	// Now both?
	engone::EngineObject* CreatePlayer(World* world, engone::UUID uuid = 0);
}