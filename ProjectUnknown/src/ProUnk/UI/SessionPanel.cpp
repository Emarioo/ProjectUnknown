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
	void SessionPanel::setDefaultPortIP(const std::string& port, const std::string& ip, const std::string& type) {
		m_type = type;
		m_ipBox.text = ip;
		m_portBox.text = port;
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
			// TODO: handle potential error messages.
			if (m_type == "Client") {
				result = session->getClient().start(m_ipBox.text, m_portBox.text);
				if (!result)
					log::out << log::RED << "SessionPanel : Client failed (ip: " << m_ipBox.text << ", port: " << m_portBox.text << "\n";
			} else if (m_type == "Server") {
				result = session->getServer().start(m_portBox.text);
				if (!result)
					log::out << log::RED << "SessionPanel : Server failed (ip: " << m_ipBox.text << ", port: " << m_portBox.text << "\n";
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

		ui::TextBox portText = { "Port: ",0,0,textHeight,consolas,textColor};
		portText.x = offsetX;
		portText.y = offsetY;

		m_portBox.x = offsetX + portText.getWidth();
		m_portBox.y = offsetY;
		m_portBox.h = textHeight;
		m_portBox.font = consolas;
		m_portBox.rgba = textColor;
		
		offsetY += portText.h;

		int clickPortBox = ui::Clicked(m_portBox);
		int clickPortBox2 = ui::Clicked(portText);
		if (clickPortBox == 1|| clickPortBox2==1) {
			m_portBox.editing = true;
		} else if (clickPortBox == -1|| clickPortBox2==-1) {
			m_portBox.editing = false;
		}

		ui::Edit(&m_portBox);

		ui::TextBox ipText = { "IP: " ,0,0,textHeight,consolas,textColor };
		ipText.x = offsetX;
		ipText.y = offsetY;

		m_ipBox.x = offsetX + ipText.getWidth();
		m_ipBox.y = offsetY;
		m_ipBox.h = textHeight;
		m_ipBox.font = consolas;
		m_ipBox.rgba = textColor;

		offsetY += portText.h;

		int clickIpBox = ui::Clicked(m_ipBox);
		int clickIpBox2 = ui::Clicked(ipText);
		if (clickIpBox == 1|| clickIpBox2 == 1) {
			m_ipBox.editing = true;
		} else if (clickIpBox == -1|| clickIpBox2 == -1) {
			m_ipBox.editing = false;
		}

		ui::Edit(&m_ipBox);

		ui::Draw(area);
		ui::Draw(connType);
		ui::Draw(portText);
		ui::Draw(ipText);
		ui::Draw(m_portBox);
		ui::Draw(m_ipBox);

		ui::TextBox stopBox = { "Stop",offsetX,offsetY,textHeight,consolas, textColor };
		ui::TextBox connectBox = { "Connect",offsetX,offsetY,textHeight,consolas, textColor };
		if (session->getServer().isRunning() || session->getClient().isRunning()) {
			if (ui::Clicked(stopBox) == 1) {
				stop(info);
			}
			ui::Draw(stopBox);
		} else {
			if (m_type == "Server") {
				connectBox.text = "Start";
			}
			if (ui::Clicked(connectBox) == 1) {
				connect(info);
			}
			ui::Draw(connectBox);
		}
		offsetY += textHeight; // textHeight to leave some room for start/stop textBox

		float stoof[] = { connType.getWidth() , portText.getWidth()+m_portBox.getWidth(), portText.getWidth() + m_portBox.getWidth(), connectBox.getWidth()};
		//float stoof[] = { fpsBox.getWidth() , upsBox.getWidth(), runtimeBox.getWidth(), sleepBox.getWidth(),speedBox.getWidth() };
		float areaW = 0;
		for (int i = 0; i < sizeof(stoof) / sizeof(float); i++) {
			if (stoof[i] > areaW) {
				areaW = stoof[i];
			}
		}
		setMinHeight(offsetY-area.y+border);
		setMinWidth(areaW+2*border); // Todo: hardcoded not great
	}
}