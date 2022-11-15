#include "ProUnk/Combat/Combat.h"

namespace prounk {
	
	void SetCombatData(engone::GameObject* player, engone::GameObject* weapon, bool yes) {
		if (yes) {
			weapon->setColliderUserData((void*)COLLIDER_IS_DAMAGE);
			weapon->flags |= OBJECT_HAS_COMBATDATA;
			weapon->userData = player->userData;
		}
		else {
			weapon->setColliderUserData(0); // may not need to reset user data. It may be used again. or overwritten next time.
			weapon->flags &= ~OBJECT_HAS_COMBATDATA;
			weapon->userData = nullptr;
		}
	}
	//float AttackCalculation(CombatData* atk, CombatData* def) {
	//	
	//}
}