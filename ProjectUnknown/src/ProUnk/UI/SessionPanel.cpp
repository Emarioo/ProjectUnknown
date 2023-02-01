#include "ProUnk/UI/SessionPanel.h"

#include "ProUnk/GameApp.h"

#include "ProUnk/UI/InvUtility.h"

namespace prounk {
	SessionPanel::SessionPanel(GameApp* app) : app(app) {
		
	}
	// lovely name
	//struct Stuff {
	//	std::string type = "Client"; // server or client
	//	engone::ui::TextBox portBox = { "port" };
	//	engone::ui::TextBox ipBox = { "ip" };
	//	bool once = false;
	//};
	//std::unordered_map<GameApp*, Stuff> app_data;

	void SessionPanel::stop(engone::LoopInfo& info) {
		GameApp* app = (GameApp*)info.window->getParent();
		Session* session = app->getActiveSession();
		if (session->getServer().isRunning()) {
			session->getServer().stop();
		}
		if (session->getClient().isRunning()) {
			session->getClient().stop();
		}
	}
	IPAndPort SplitAddress(const std::string& address) {
		std::string ip = Session::DEFAULT_IP;
		std::string port = Session::DEFAULT_PORT;

		//-- Convert address
		std::vector<std::string> split = engone::SplitString(address, ":");
		if (split.size() >= 1) {
			if (!split[0].empty())
				ip = split[0];
		}
		if (split.size() >= 2) {
			if (!split[1].empty())
				port = split[1];
		}
		return { ip, port };
	}
	void SessionPanel::setDefaultAddress(const std::string& address, const std::string& type) {
		m_type = type;
		m_address = address;
	}
	void SessionPanel::connect(engone::LoopInfo& info) {
		using namespace engone;
		Session* session = app->getActiveSession();

		// Nothing will happen if client or server is active. You have to stop first.
		// NOTE: restarting server is a bit tricky because you can't call stop and then start because stop is asynchronous and when start is called the server is still running
		//		resulting in just a stopped server.
		//		You can't call cleanup (synchronous) because it would freeze the game (not for long but still not ideal).
		//		A good solution would be a queue. Calling stop and start would put actions onto the queue for a worker thread to handle. The worker thread is synchrounous when
		//      doing the actions while the thread calling stop and start is asynchronous.
		//		THIS idea has been added to the TODO list in Engone for Networking. Change this code when it is implemented.

		if (!session->getServer().isRunning() && !session->getClient().isRunning()) {
			bool result = false;

			auto [ip, port] = SplitAddress(m_address);

			// TODO: handle potential error messages.
			if (m_type == "Client") {
				result = session->getClient().start(ip,port);
				if (!result)
					log::out << log::RED << "SessionPanel : Client failed (ip: " << ip << ", port: " << port << "\n";
			} else if (m_type == "Server") {
				result = session->getServer().start(port);
				if (!result)
					log::out << log::RED << "SessionPanel : Server failed (ip: " << ip << ", port: " << port<< "\n";
			}
		}
	}

	void SessionPanel::render(engone::LoopInfo& info) {
		using namespace engone;
		CommonRenderer* renderer = GET_COMMON_RENDERER();
		//Renderer* renderer = info.window->getRenderer();
		Session* session = app->getActiveSession();

		FontAsset* consolas = info.window->getStorage()->get<FontAsset>("fonts/consolas42");

		// does not reflect the current connections at startup.
		// active when locked or not?
		float sw = GetWidth();
		float sh = GetHeight();

		const ui::Color backColor = { 0.,0.,0.,0.5 };
		const ui::Color textColor = { 1 };
		const ui::Color hiddenColor = { 0.7, 1 };

		ui::Box area = getBox();
		area.rgba = backColor;

		const float textHeight = 25;
		const float border = 4;
		float offsetX = area.x+border;
		float offsetY = area.y+border;
		ui::TextBox connType = { m_type,0,0,textHeight,consolas, textColor };
		connType.x = offsetX;
		connType.y = offsetY;
		
		offsetY += connType.h;

		if (ui::Clicked(connType) == 1) {
			if (m_type == "Server") m_type = "Client";
			else if (m_type== "Client") m_type = "Server";
			connType.text = m_type;
		}

		ui::Box addressBack = { 0,0,0,0,{0,0,0.1,0.1} };
		addressBack.x = offsetX;
		addressBack.y = offsetY;

		ui::TextBox addressDesc = { "Address: ",0,0,textHeight,consolas,textColor};
		addressDesc.x = addressBack.x;
		addressDesc.y = addressBack.y;

		ui::TextBox addressText = { m_address,0,0,addressDesc.h,consolas,textColor };
		addressText.x = addressDesc.x + addressDesc.getWidth();
		addressText.y = addressDesc.y;

		ui::TextBox suggestText = { "",0,0,addressText.h, consolas, hiddenColor };
		suggestText.x = addressText.x + addressText.getWidth();
		suggestText.y = addressText.y;
		if (m_address.empty()) {
			suggestText.text = std::string(Session::DEFAULT_IP) + ":" + std::string(Session::DEFAULT_PORT);
		} else {
			int index = addressText.text.find_last_of(':');
			if (index == -1)
				suggestText.text = ":" + std::string(Session::DEFAULT_PORT);
			else if (index == addressText.text.length() - 1)
				suggestText.text = Session::DEFAULT_PORT;
		}
		addressBack.w = addressDesc.getWidth() + addressText.getWidth()+ suggestText.getWidth();
		addressBack.h = addressDesc.h;

		offsetY += addressBack.h;

		addressText.editing = editingAddress;
		addressText.at = editingAt;

		int clickAddress = ui::Clicked(addressBack);
		//int clickPortBox2 = ui::Clicked(portText);
		if (clickAddress == 1) {
			addressText.editing = true;
		} else if (clickAddress == -1) {
			addressText.editing = false;
		}

		ui::Edit(&addressText);

		editingAddress = addressText.editing;
		editingAt = addressText.at;

		m_address = addressText.text;

		//ui::TextBox ipText = { "IP: " ,0,0,textHeight,consolas,textColor };
		//ipText.x = offsetX;
		//ipText.y = offsetY;

		//m_ipBox.x = offsetX + ipText.getWidth();
		//m_ipBox.y = offsetY;
		//m_ipBox.h = textHeight;
		//m_ipBox.font = consolas;
		//m_ipBox.rgba = textColor;

		//offsetY += portText.h;

		//int clickIpBox = ui::Clicked(m_ipBox);
		//int clickIpBox2 = ui::Clicked(ipText);
		//if (clickIpBox == 1|| clickIpBox2 == 1) {
		//	m_ipBox.editing = true;
		//} else if (clickIpBox == -1|| clickIpBox2 == -1) {
		//	m_ipBox.editing = false;
		//}

		//ui::Edit(&m_ipBox);

		ui::Draw(area);
		ui::Draw(connType);
		ui::Draw(addressBack);
		ui::Draw(addressDesc);
		ui::Draw(addressText);
		ui::Draw(suggestText);

		ui::TextBox connectBox = { "Connect",offsetX,offsetY,textHeight,consolas, textColor };
		if (session->getServer().isRunning() || session->getClient().isRunning()) {
			connectBox.text = "Stop";
			if (ui::Clicked(connectBox) == 1) {
				stop(info);
			}
		} else {
			if (m_type == "Server") {
				connectBox.text = "Start";
			}
			if (ui::Clicked(connectBox) == 1) {
				connect(info);
			}
		}
		ui::Draw(connectBox);
		offsetY += textHeight; // textHeight to leave some room for start/stop textBox

		float stoof[] = { connType.getWidth() , addressBack.w, connectBox.getWidth()};
		//float stoof[] = { fpsBox.getWidth() , upsBox.getWidth(), runtimeBox.getWidth(), sleepBox.getWidth(),speedBox.getWidth() };
		float areaW = 0;
		for (int i = 0; i < sizeof(stoof) / sizeof(float); i++) {
			if (stoof[i] > areaW) {
				areaW = stoof[i];
			}
		}
		setMinHeight(offsetY-area.y+border);
		setMinWidth(areaW+2*border); // Todo: hardcoded not great

		DrawToolTip(area.x + area.w, area.y, 20, 20, "Session panel: Server/Client stuff\nClick Client/Server to switch\nFill in port and ip\nPress Start/Connect");

		// Special keybindings
		ui::TextBox infodump = { "G : Flight, C : No Clip, K : Die, O : Hitboxes, R : Respawn, E : Pickup item\nQ : Throw held item, TAB : Inventory, ALT + LM/RM : Change UI layout\nJ : Spawn Dummies, I : Enable Dummies",0,0,17,consolas,{1} };
		infodump.x = 3;
		infodump.y = GetHeight()-infodump.getHeight() - 3;

		ui::Box infoback = { 0,0,0,0,{0.f,0.5f} };
		infoback.x = 0;
		infoback.y = infodump.y;
		infoback.w = infodump.getWidth()+6;
		infoback.h = infodump.getHeight()+6;
		ui::Draw(infoback);

		ui::Draw(infodump);
	}
}