#pragma once

#include "Engone/Engone.h"
// #include "Objects/BasicObjects.h"

#include "Engone/ParticleModule.h"

// #include "ProUnk/Combat/CombatData.h"

// #include "ProUnk/PlayerController.h"
// #include "ProUnk/Registries/InventoryRegistry.h"

#include "ProUnk/UI/MasterInventoryPanel.h"
#include "ProUnk/UI/InventoryPanel.h"
#include "ProUnk/UI/PlayerBarPanel.h"
#include "ProUnk/UI/CraftingPanel.h"
#include "ProUnk/UI/CheatPanel.h"
#include "ProUnk/UI/SessionPanel.h"

#include "ProUnk/World/Session.h"

#include "ProUnk/Effects/VisualEffects.h"

#include "Engone/UIModule/UIModule.h"

// #include "Engone/Sound/SoundStream.h"
#ifdef gone
namespace prounk {

	struct CombatParticle {
		glm::vec3 pos;
		glm::vec3 vel;
		float lifeTime;
	};
	struct CombatParticleInfo {
		int aliveCount;
	};
	// flags, address
	struct GameAppInfo {
		int flags=0;
		std::string address;
	};
	class GameApp : public engone::Application, public rp3d::EventListener {
	public:
		static const int START_SERVER = 1;
		static const int START_CLIENT = 2;

		GameApp(engone::Engone* engone, GameAppInfo info);
		~GameApp() override;
		void cleanup();

		void update(engone::LoopInfo& info) override;
		void render(engone::LoopInfo& info) override;
		void onClose(engone::Window* window) override;

		void onTrigger(const rp3d::OverlapCallback::CallbackData& callbackData) override;
		void onContact(const rp3d::CollisionCallback::CallbackData& callbackData) override;
		void dealCombat(engone::EngineObject* atkObj, engone::EngineObject* collider, glm::vec3 contactPoint);

		PlayerController playerController;

		PanelHandler panelHandler;
		
		// UIModule uiModule{};

		MasterInventoryPanel* masterInventoryPanel = nullptr;
		InventoryPanel* inventoryPanel = nullptr;
		PlayerBarPanel* playerBarPanel = nullptr;
		CheatPanel* cheatPanel = nullptr;
		SessionPanel* sessionPanel = nullptr;
		//CraftingPanel craftingPanel=nullptr;

		engone::DelayCode delayed;

		rp3d::RigidBody* testRB = nullptr;
		
		//inline World* getWorld() override { return (World*)Application::getWorld(); }
		//inline void setWorld(World* world) { Application::setWorld(world); }

		Session* m_session = nullptr;
		Session* getActiveSession();

		void panelInput(engone::LoopInfo& info);
		void createPanels();

		void registerItems();

		// request system for particles.
		void doParticles(glm::vec3 pos) {
			partRequested = true; requestPos = pos;
		}

		// engone::SoundStream melody;

		float codeReloadTime=0;
		double codeLastModified=0;
		void* codeLibrary=nullptr;
		const char* codePath = "D:\\Backup\\CodeProjects\\ProjectUnknown\\bin\\GameCode\\Debug-MSVC\\GameCode.dll";
		const char* codePath2 = "D:\\Backup\\CodeProjects\\ProjectUnknown\\bin\\GameCode\\Debug-MSVC\\GameCode_.dll";
		const char* pdbPath = "D:\\Backup\\CodeProjects\\ProjectUnknown\\bin\\GameCode\\Debug-MSVC\\GameCode.pdb";
		const char* pdbPath2 = "D:\\Backup\\CodeProjects\\ProjectUnknown\\bin\\GameCode\\Debug-MSVC\\GameCode_.pdb";

		typedef void(*PlayerUpdateFunc)(engone::LoopInfo*,engone::EngineObject*);

		PlayerUpdateFunc updatePlayerMovement = 0;

		//std::unordered_map<std::string, bool>& getStates() {
		//	return m_stateMap;
		//}
		//void setState(const std::string& name, bool yes) {
		//	m_stateMap[name] = yes;
		//}
		//bool getState(const std::string& name) {
		//	return m_stateMap[name];
		//}
		// ESC : Open menu to save and leave Session. Mouse is made visible
		// TAB : Open Inventory. Mouse is made visible
		// G : Cheat Panel.

		// Pressing any of these buttons should make the Mouse visible.

		// Some button that allows you to move and resize panels like ALT. Should it be alt though?.

		int showCursor = 0;
		void incrCursor() { showCursor++; }
		void decrCursor() { showCursor--; }

		bool debugInfoToggle = false;

		bool partRequested = false;
		glm::vec3 requestPos;
		
		engone::ParticleGroup<CombatParticle>* combatParticles=nullptr;
		engone::ParticleGroup<engone::DefaultParticle>* particleGroup;

		VisualEffects visualEffects;

	private:
		bool paused = true;

		//std::unordered_map<std::string, bool> m_stateMap;

		engone::Window* m_window=nullptr;
	};
}
#endif