#pragma once

#include "Engone/Engone.h"

#include "Engone/Application.h"

#include "Objects/Player.h"
#include "Objects/Goblin.h"

namespace game {
	class GameApp : public engone::Application {
	public:
		GameApp();

		void update(float delta) override;
		void render() override;
		static void CloseCallback(engone::Window* window);

	private:
		Player* m_player;
		engone::Window* m_window;

		void TestScene();
		void IntroScreen();
		void UiTest();
	};
}