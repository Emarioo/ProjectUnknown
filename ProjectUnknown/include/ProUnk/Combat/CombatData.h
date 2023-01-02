#pragma once

#include "Engone/LoopInfo.h"
#include "Engone/EngineObject.h"
#include "ProUnk/Combat/Skills.h"

namespace prounk {
	extern uint32_t COLLIDER_IS_DAMAGE;
	extern uint32_t COLLIDER_IS_HEALTH;
	class CombatData {
	public:
		CombatData() {}
		enum DamageType : int {
			NONE,
			CONTINOUS_DAMAGE,
			SINGLE_DAMAGE,
		};

		// useful if weapon uses combat data which is also used by player.
		// dealDamage could then get the owner which is the player can do something with the player's rotation
		engone::EngineObject* owner = nullptr;
		float animationTime = 0; // endFrame-currentFrame in seconds
		
		bool attacking = false; // should be true when dealing damage

		std::vector<engone::UUID> hitObjects; // cleared at beginning of attack

		SkillType skillType = (SkillType)0;

		//-- stats
		float health = 100;
		float totalFlatMaxHealth = 100;
		float totalModMaxHealth = 1;
		
		DamageType damageType = NONE;
		float damagePerSecond=0;
		float singleDamage=0;
		float knockStrength = 0.1f;

		// attack types
		//	continous damage )damage/second)
		//	single damage
		//	damage at certain frames?

		//bool wasUpdated = false; // temporary

		//inline float getAttack() {
		//	return totalFlatAtk * totalModAtk;
		//}
		inline float getMaxHealth() {
			return totalFlatMaxHealth * totalModMaxHealth;
		}

		void attack() { attacking = true; hitObjects.clear(); }

		void update(engone::LoopInfo& info) {
			using namespace engone;
			//log::out << "animtime " << animationTime << "\n";
			animationTime -= info.timeStep;
			if (animationTime < 0) {
				animationTime = 0;
				attacking = false;
			}

			//hitCooldown -= info.timeStep;
			//if (hitCooldown < 0)
			//	hitCooldown = 0;

			//log::out << getAttack() << "\n";
		}
	};
}