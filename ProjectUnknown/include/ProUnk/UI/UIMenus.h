#pragma once

#include "Engone/LoopInfo.h"
#include "Engone/EngineObject.h"

namespace prounk {
	class GameApp;
	void IntroScreen(GameApp* app);
	void UiTest(GameApp* app);
	void TestScene(GameApp* app);

	void DebugInfo(engone::LoopInfo& info, GameApp* app);
	// app is required because the values are stored globally and you need a way to differentiate the values from multiple applications.
	void SetDefaultPortIP(GameApp* app, const std::string& port, const std::string& ip, const std::string& type);
	void RenderServerClientMenu(engone::LoopInfo& info);
}