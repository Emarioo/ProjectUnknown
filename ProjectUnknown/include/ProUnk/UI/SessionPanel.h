#pragma once

#include "ProUnk/UI/PanelHandler.h"

namespace prounk {

	class GameApp;

	struct IPAndPort {
		std::string ip;
		std::string port;
	};
	IPAndPort SplitAddress(const std::string& address);

	class SessionPanel : public Panel {
	public:
		SessionPanel(GameApp* app);

		void render(engone::LoopInfo& info) override;

		// address could be 127.0.0.1:59123
		void setDefaultAddress(const std::string& address, const std::string& type);

		void connect(engone::LoopInfo& info);
		void stop(engone::LoopInfo& info);

	private:

		std::string m_type = "Client"; // server or client
		std::string m_address;
		bool editingAddress=false;
		int editingAt = -1;
		bool once = false;

		GameApp* app;
	};
}