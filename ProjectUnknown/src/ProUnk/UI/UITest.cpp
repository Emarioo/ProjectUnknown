
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

	void Stop(engone::RenderInfo& info) {
		GameApp* app = (GameApp*)info.window->getParent();
		NetGameGround* ground = app->getGround();
		if (ground->getServer().isRunning()) {
			ground->getServer().stop();
		}
		if (ground->getClient().isRunning()) {
			ground->getClient().stop();
		}
	}
	void Connect(engone::RenderInfo& info) {
		using namespace engone;
		GameApp* app = (GameApp*)info.window->getParent();
		Stuff& stuff = app_data[app];
		NetGameGround* ground = app->getGround();
		if (ground->getServer().isRunning()) {
			ground->getServer().stop();
		}
		if (ground->getClient().isRunning()) {
			ground->getClient().stop();
		}
		// TODO: handle potential error messages.
		if (stuff.type == "Client") {
			ground->getClient().start(stuff.ipBox.text, stuff.portBox.text);
		}
		else if (stuff.type == "Server") {
			ground->getServer().start(stuff.portBox.text);
		}
	}
	void RenderServerClientMenu(engone::RenderInfo& info) {
		using namespace engone;
		GameApp* app = (GameApp*)info.window->getParent();

		auto find = app_data.find(app);
		if (find == app_data.end())
			app_data[app] = {};

		Stuff& stuff = app_data[app];
		if (!stuff.once) {
			stuff.once = true;
			
			NetGameGround* ground = ((GameApp*)info.window->getParent())->getGround();
			if (ground->getServer().isRunning()) {
				stuff.type = "Server";
				stuff.ipBox.text = ground->getServer().getIP();
				stuff.portBox.text = ground->getServer().getPort();
			}
			if (ground->getClient().isRunning()) {
				stuff.type = "Client";
				stuff.ipBox.text = ground->getClient().getIP();
				stuff.portBox.text = ground->getClient().getPort();
			}
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

		if (ui::Clicked(stuff.portBox)==1) {
			stuff.portBox.editing = true;
		}

		ui::Edit(&stuff.portBox);

		ui::Draw(stuff.portBox);

		stuff.ipBox.x = connType.x;
		stuff.ipBox.y = stuff.portBox.y+ stuff.portBox.h;
		stuff.ipBox.h = 25;
		stuff.ipBox.font = consolas;
		stuff.ipBox.rgba = textColor;
		
		if (ui::Clicked(stuff.ipBox) == 1) {
			stuff.ipBox.editing = true;
		}
		ui::Edit(&stuff.ipBox);
		ui::Draw(stuff.ipBox);

		ui::TextBox connectBox = { "Connect",stuff.ipBox.x,stuff.ipBox.y + stuff.ipBox.h,25,consolas, textColor };
		if (ui::Clicked(connectBox)==1) {
			Connect(info);
			//log::out << "clicked\n";
		}
		ui::Draw(connectBox);

		ui::TextBox stopBox = { "Stop",connectBox.x,connectBox.y + connectBox.h,25,consolas, textColor };
		if (ui::Clicked(stopBox) == 1) {
			Stop(info);
			//log::out << "clicked\n";
		}
		ui::Draw(stopBox);

		// server/client toggle button
		// port
		// ip box if client
		// connect button
		//	will disconnect previous connections
		
	}
}