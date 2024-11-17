#pragma once

#include "Engone/LoopInfo.h"
#include "Engone/EngineObject.h"

namespace prounk {
	class GameApp;
	void IntroScreen(GameApp* app);
	void UiTest(GameApp* app);
	void TestScene(GameApp* app);

	void DebugInfo(engone::LoopInfo& info, GameApp* app);
}