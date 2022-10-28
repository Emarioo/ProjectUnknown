#pragma once

#include "Engone/LoopInfo.h"
#include "Engone/GameObject.h"
#include "ProUnk/Combat/Skills.h"

namespace prounk {
	class CombatData {
	public:
		CombatData() {}

		engone::GameObject* owner = nullptr;
		float animationTime = 0; // endFrame-currentFrame in seconds
		
		bool attacking = false;

		float hitCooldown = 0;

		SkillType skillType = (SkillType)0;

		//-- stats
		float totalFlatAtk = 30;
		float totalModAtk = 1;
		float health = 100;
		float totalFlatMaxHealth = 100;
		float totalModMaxHealth = 1;

		inline float getAttack() {
			return totalFlatAtk * totalModAtk;
		}
		inline float getMaxHealth() {
			return totalFlatMaxHealth * totalModMaxHealth;
		}

		void attack() { attacking = true; }

		void update(engone::UpdateInfo& info) {
			using namespace engone;
			//log::out << "animtime " << animationTime << "\n";
			animationTime -= info.timeStep;
			if (animationTime < 0) {
				animationTime = 0;
				attacking = false;
			}

			hitCooldown -= info.timeStep;
			if (hitCooldown < 0)
				hitCooldown = 0;

			//log::out << getAttack() << "\n";
		}
	};
}