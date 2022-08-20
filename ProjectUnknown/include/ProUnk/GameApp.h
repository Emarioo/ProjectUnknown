#pragma once

#include "Engone/Engone.h"
#include "Objects/Sword.h"
#include "Objects/Player.h"
#include "Objects/Terrain.h"

#include "Engone/World/Playground.h"

#include "Engone/ParticleModule.h"

namespace game {

	struct CombatParticle {
		glm::vec3 pos;
		glm::vec3 vel;
		float lifeTime;
	};
	struct CombatParticleInfo {
		int aliveCount;
	};
	class GameApp : public engone::Application, public rp3d::EventListener {
	public:
		GameApp(engone::Engone* engone);

		void update(engone::UpdateInfo& info) override;
		void render(engone::RenderInfo& info) override;
		void onClose(engone::Window* window) override;

		void onTrigger(const rp3d::OverlapCallback::CallbackData& callbackData) override;

		Player* player=nullptr;
		Terrain* terrain = nullptr;

	private:
		bool paused = true;

		engone::ParticleGroup<engone::DefaultParticle>* particleGroup;

		engone::ParticleGroup<CombatParticle>* combatParticles=nullptr;

		engone::Window* m_window=nullptr;

		Sword* sword = nullptr;

		void TestScene();
		void IntroScreen();
		void UiTest();
	};
}