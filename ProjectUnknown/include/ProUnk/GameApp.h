#pragma once

#include "Engone/Engone.h"
#include "Objects/Sword.h"
#include "Objects/Player.h"
#include "Objects/Terrain.h"

#include "ProUnk/NetGameGround.h"

#include "Engone/ParticleModule.h"

namespace prounk {

	struct CombatParticle {
		glm::vec3 pos;
		glm::vec3 vel;
		float lifeTime;
	};
	struct CombatParticleInfo {
		int aliveCount;
	};
	// flags, port, ip
	struct GameAppInfo {
		int flags=0;
		std::string port;
		std::string ip;
	};
	class GameApp : public engone::Application, public rp3d::EventListener {
	public:
		static const int START_SERVER = 1;
		static const int START_CLIENT = 2;

		GameApp(engone::Engone* engone, GameAppInfo info);
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

		
		engone::ParticleGroup<CombatParticle>* combatParticles=nullptr;
		engone::ParticleGroup<engone::DefaultParticle>* particleGroup;

	private:
		bool paused = true;

		engone::Window* m_window=nullptr;

		Sword* sword = nullptr;

		void UiTest();
	};
}