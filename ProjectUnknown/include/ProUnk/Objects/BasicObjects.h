#pragma once

namespace engone {
	class GameObject; 
	class GameGround;
}

#include "Engone/Utilities/RandomUtility.h"

namespace prounk {
	enum BasicObjectType : int {
		OBJECT_DUMMY=0,
		//OBJECT_SLIME,
		OBJECT_SWORD,
		OBJECT_TERRAIN,
		OBJECT_PLAYER
	};

	engone::GameObject* CreateObject(int type, engone::GameGround* ground, engone::UUID uuid = 0);
	// Ground and object cannot be nullptr. Function should work for most objects.
	// Perhaps not player since it may have allocated some specific memory. It shouldn't allocate this memory though.
	// That should be moved elsewhere.
	void DeleteObject(engone::GameGround* ground, engone::GameObject* object);
	engone::GameObject* CreateDummy(engone::GameGround* ground, engone::UUID uuid = 0);
	//engone::GameObject* CreateSlime(engone::GameGround* ground, engone::UUID uuid = 0);
	engone::GameObject* CreateSword(engone::GameGround* ground, engone::UUID uuid = 0);
	engone::GameObject* CreateTerrain(engone::GameGround* ground, engone::UUID uuid = 0);
	// player from a connection. not the player you control
	engone::GameObject* CreatePlayer(engone::GameGround* ground, engone::UUID uuid = 0);
}