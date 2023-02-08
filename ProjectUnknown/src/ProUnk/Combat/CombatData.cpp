#include "ProUnk/Combat/CombatData.h"

namespace prounk {
	//-- flags on EngineObject
	uint64 OBJECT_HAS_COMBATDATA = 0xF0000000;

	//-- flags in userdata
	uint64 COLLIDER_IS_DAMAGE = 0x10000000;
	uint64 COLLIDER_IS_HEALTH = 0x20000000;
}