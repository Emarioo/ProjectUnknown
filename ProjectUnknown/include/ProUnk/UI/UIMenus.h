#pragma once

#include "Engone/LoopInfo.h"

namespace prounk {
	class GameApp;
	void IntroScreen(GameApp* app);
	void UiTest(GameApp* app);
	void TestScene(GameApp* app);

	void DebugInfo(engone::RenderInfo& info, GameApp* app);

	void RenderServerClientMenu(engone::RenderInfo& info);
}