#pragma once

namespace engone {
	class GameObject; 
	class GameGround;
}

#include "Engone/Utilities/RandomUtility.h"

namespace prounk {
	enum BasicObjectType : int {
		OBJECT_DUMMY=0,
		OBJECT_SWORD,
		OBJECT_TERRAIN,
		OBJECT_PLAYER
	};
	engone::GameObject* CreateObject(int type, engone::GameGround* ground, engone::UUID uuid = 0);
	engone::GameObject* CreateDummy(engone::GameGround* ground, engone::UUID uuid = 0);
	engone::GameObject* CreateSword(engone::GameGround* ground, engone::UUID uuid = 0);
	engone::GameObject* CreateTerrain(engone::GameGround* ground, engone::UUID uuid = 0);
	// player from a connection. not the player you control
	engone::GameObject* CreatePlayer(engone::GameGround* ground, engone::UUID uuid = 0);
}