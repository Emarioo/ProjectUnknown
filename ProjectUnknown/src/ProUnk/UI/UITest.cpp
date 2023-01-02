
#include "ProUnk/UI/UIMenus.h"

#include "ProUnk/GameApp.h"

namespace prounk {

	//engone::ui::TextBox editText = { "012345",50,100,50 };
		//std::string hey = "Okay then";
		//int indexing = hey.length();
	void UiTest(GameApp* app) {
		using namespace engone;
		//editText.y += 0.05;

		/*ui::Edit(hey, indexing);
		ui::TextBox wow = { hey, 50, 100, 30 };
		wow.font = GetAsset<Font>("consolas");
		wow.at = indexing;
		wow.edited = true;
		ui::Draw(wow);*/

		/*if (editText.font == nullptr) {
			editText.font = GetAsset<Font>("consolas");
		}*/
		//ui::Draw({ "Hejsan", 100, 100, 30, 0.5f, 0.5f, 0.5f});
		/*ui::Draw({50,100,100,100,1,0,0});
		ui::Draw({GetAsset<Texture>("textures/wall"), 500, 100, 400, 400});*/

		//ui::Box fullscreen = {100+2,100,100,100,1.f,1.f,0};
		//ui::Box windowed = {100,400,100,100,1.f,0,0};
		//ui::Box borderless = {100,600,100,100,1.f,0,1.f};

		//float w = editText.font->getWidth(editText.text, editText.h);
		//ui::Draw({editText.x,editText.y,w,editText.h,1.f,0,0});

		//ui::Draw(fullscreen);
		///*ui::Draw(windowed);
		//ui::Draw(borderless);*/
		//if (ui::Hover(editText)) {
		//	ui::Edit(&editText);
		//}

		//ui::Draw(editText);

		/*if (ui::Clicked(fullscreen)) {
			GetWindow()->setMode(WindowMode::Fullscreen);
		}
		if (ui::Clicked(windowed)) {
			GetWindow()->setMode(WindowMode::Windowed);
		}
		if (ui::Clicked(borderless)) {
			GetWindow()->setMode(WindowMode::BorderlessFullscreen);
		}*/
	}

	// lovely name
	struct Stuff {
		std::string type = "Client"; // server or client
		engone::ui::TextBox portBox = { "port" };
		engone::ui::TextBox ipBox = { "ip" };
		bool once = false;
	};
	std::unordered_map<GameApp*, Stuff> app_data;

	void Stop(engone::LoopInfo& info) {
		GameApp* app = (GameApp*)info.window->getParent();
		Session* session = app->getActiveSession();
		if (session->getServer().isRunning()) {
			session->getServer().stop();
		}
		if (session->getClient().isRunning()) {
			session->getClient().stop();
		}
	}
	void SetDefaultPortIP(GameApp* app, const std::string& port, const std::string& ip) {
		auto find = app_data.find(app);
		if (find == app_data.end()) {
			app_data[app] = { "Client",{port},{ip},false };
		} else {
			find->second.ipBox.text = ip;
			find->second.portBox.text = port;
		}
	}
	void Connect(engone::LoopInfo& info) {
		using namespace engone;
		GameApp* app = (GameApp*)info.window->getParent();
		Stuff& stuff = app_data[app];
		Session* session = app->getActiveSession();

		// Nothing will happen if client or server is active. You have to stop first.
		// NOTE: restarting server is a bit tricky because you can't call stop and then start because stop is asynchronous and when start is called the server is still running
		//		resulting in just a stopped server.
		//		You can't call cleanup (synchronous) because it would freeze the game (not for long but still not ideal).
		//		A good solution would be a queue. Calling stop and start would put actions onto the queue for a worker thread to handle. The worker thread is synchrounous when
		//      doing the actions while the thread calling stop and start is asynchronous.
		//		THIS idea has been added to the TODO list in Engone for Networking. Change this code when it is implemented.

		if (!session->getServer().isRunning()&&!session->getClient().isRunning()) {
			bool result = false;
			// TODO: handle potential error messages.
			if (stuff.type == "Client") {
				result = session->getClient().start(stuff.ipBox.text, stuff.portBox.text);
			} else if (stuff.type == "Server") {
				result = session->getServer().start(stuff.portBox.text);
			}
			if (!result) {
				log::out << log::RED << "UITest::Connect - Server/Client::start failed\n";
			}
		}
	}
	void RenderServerClientMenu(engone::LoopInfo& info) {
		using namespace engone;
		GameApp* app = (GameApp*)info.window->getParent();

		if (info.window->isCursorLocked()) // TODO: change this to check for pause state
			return;

		auto find = app_data.find(app);
		if (find == app_data.end())
			app_data[app] = {};

		Session* session = ((GameApp*)info.window->getParent())->getActiveSession();
		
		Stuff& stuff = app_data[app];
		if (!stuff.once) {
			stuff.once = true;
			
			if (session->getServer().isRunning()) {
				stuff.type = "Server";
				stuff.ipBox.text = session->getServer().getIP();
				stuff.portBox.text = session->getServer().getPort();
			}
			if (session->getClient().isRunning()) {
				stuff.type = "Client";
				stuff.ipBox.text = session->getClient().getIP();
				stuff.portBox.text = session->getClient().getPort();
			}
			// specify default values with SetDefaultIPPort
		}

		FontAsset* consolas = info.window->getStorage()->get<FontAsset>("fonts/consolas42");

		// does not reflect the current connections at startup.
		// active when locked or not?
		float sw = GetWidth();
		float sh = GetHeight();

		ui::Color textColor = {0.8,0.8,0.8,1};

		ui::TextBox connType = { stuff.type,10,sh/2,25,consolas, textColor };
		
		if (ui::Clicked(connType) == 1) {
			if (stuff.type == "Server") stuff.type = "Client";
			else if (stuff.type == "Client") stuff.type = "Server";
			connType.text = stuff.type;
		}

		ui::Draw(connType);

		stuff.portBox.x = connType.x;
		stuff.portBox.y = connType.y+connType.h;
		stuff.portBox.h = 25;
		stuff.portBox.font = consolas;
		stuff.portBox.rgba = textColor;

		int clickPortBox = ui::Clicked(stuff.portBox);
		if (clickPortBox==1) {
			stuff.portBox.editing = true;
		} else if(clickPortBox==-1) {
			stuff.portBox.editing = false;
		}

		ui::Edit(&stuff.portBox);

		ui::Draw(stuff.portBox);

		stuff.ipBox.x = connType.x;
		stuff.ipBox.y = stuff.portBox.y+ stuff.portBox.h;
		stuff.ipBox.h = 25;
		stuff.ipBox.font = consolas;
		stuff.ipBox.rgba = textColor;
		
		int clickIpBox = ui::Clicked(stuff.ipBox);
		if (clickIpBox == 1) {
			stuff.ipBox.editing = true;
		} else if (clickIpBox == -1) {
			stuff.ipBox.editing = false;
		}

		ui::Edit(&stuff.ipBox);
		ui::Draw(stuff.ipBox);

		if (session->getServer().isRunning() || session->getClient().isRunning()) {
			ui::TextBox stopBox = { "Stop",stuff.ipBox.x,stuff.ipBox.y + stuff.ipBox.h,25,consolas, textColor };
			if (ui::Clicked(stopBox) == 1) {
				Stop(info);
				//log::out << "clicked\n";
			}
			ui::Draw(stopBox);
		} else {
			ui::TextBox connectBox = { "Connect",stuff.ipBox.x,stuff.ipBox.y + stuff.ipBox.h,25,consolas, textColor };
			if (stuff.type=="Server") {
				connectBox.text = "Start";
			}
			if (ui::Clicked(connectBox) == 1) {
				Connect(info);
				//log::out << "clicked\n";
			}
			ui::Draw(connectBox);
		}

		// server/client toggle button
		// port
		// ip box if client
		// connect button
		//	will disconnect previous connections
		
	}
}