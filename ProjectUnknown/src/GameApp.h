#pragma once

#include "Engone/Engone.h"
#include "Objects/Sword.h"
#include "Objects/Player.h"
#include "Objects/Terrain.h"

namespace game {
	class GameApp : public engone::Application {
	public:
		GameApp(engone::Engone* engone);

		void update(engone::UpdateInfo& info) override;
		void render(engone::RenderInfo& info) override;
		void onClose(engone::Window* window) override;

		Player* player=nullptr;
		Terrain* terrain = nullptr;

	private:
		bool paused = true;

		engone::Window* m_window=nullptr;

		Sword* sword = nullptr;

		void TestScene();
		void IntroScreen();
		void UiTest();
	};
}