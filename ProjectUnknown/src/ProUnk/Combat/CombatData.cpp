#include "ProUnk/Combat/CombatData.h"

namespace prounk {
	//-- flags on EngineObject
	uint32_t OBJECT_HAS_COMBATDATA = 0xF0000000;

	//-- flags in userdata
	uint32_t COLLIDER_IS_DAMAGE = 0x10000000;
	uint32_t COLLIDER_IS_HEALTH = 0x20000000;
}