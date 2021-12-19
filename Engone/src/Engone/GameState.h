#pragma once

#define MAX_ENGINE_GAMESTATES 5
#define MAX_CUSTOM_GAMESTATES 20

namespace GameState
{
	enum EngineGameState : unsigned char
	{
		RenderGame = MAX_CUSTOM_GAMESTATES,
		RenderGui,
		

		DebugLog,
		DebugMode
	};
}
namespace engone {
	void SetState(unsigned char state,bool f);
	bool CheckState(unsigned char state);
}