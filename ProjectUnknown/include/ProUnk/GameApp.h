#pragma once

#include "Engone/Engone.h"
#include "Objects/Sword.h"
#include "Objects/Player.h"
#include "Objects/Terrain.h"

#include "ProUnk/NetGameGround.h"

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

		enum Flag : int {
			StartServer = 1,
			StartClient = 2
		};
		typedef int Flags;

		GameApp(engone::Engone* engone, Flags flag=0);
		~GameApp() { cleanup(); }
		void cleanup() {}

		void update(engone::UpdateInfo& info) override;
		void render(engone::RenderInfo& info) override;
		void onClose(engone::Window* window) override;

		void onTrigger(const rp3d::OverlapCallback::CallbackData& callbackData) override;

		Player* player = nullptr;
		Terrain* terrain = nullptr;

		engone::DelayCode delayed;

		inline NetGameGround* getGround() override { return (NetGameGround*)Application::getGround(); }
		inline void setGround(NetGameGround* ground) { Application::setGround(ground); }

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