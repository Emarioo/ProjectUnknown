#pragma once

#include "ProUnk/UI/PanelHandler.h"

namespace prounk {

	class GameApp;

	class SessionPanel : public Panel {
	public:
		SessionPanel(GameApp* app);

		void render(engone::LoopInfo& info) override;

		void setDefaultPortIP(const std::string& port, const std::string& ip, const std::string& type);

		void connect(engone::LoopInfo& info);
		void stop(engone::LoopInfo& info);

	private:

		std::string m_type = "Client"; // server or client
		engone::ui::TextBox m_portBox = { "" };
		engone::ui::TextBox m_ipBox = { "" };
		bool once = false;

		GameApp* app;
	};
}