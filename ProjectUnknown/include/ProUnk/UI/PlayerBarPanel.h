#pragma once

#include "ProUnk/UI/PanelHandler.h"

namespace prounk {
	class GameApp;
	class PlayerBarPanel : public Panel {
	public:
		PlayerBarPanel(GameApp* app) : m_app(app) {}

		void render(engone::LoopInfo& info) override;


	private:


		GameApp* m_app;
	};
}