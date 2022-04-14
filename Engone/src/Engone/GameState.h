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
	enum class EngoneHint : uint32_t {
		None = 0,
		UI = 1,
		Game3D = 2,
		Network = 3,
		Sound = 4,
	};

	EngoneHint operator|(EngoneHint a, EngoneHint b);
	bool operator&(EngoneHint a, EngoneHint b);

	EngoneHint GetEngoneHints();
	void SetEngoneHints(EngoneHint hints);

	void SetState(unsigned char state,bool f);
	bool CheckState(unsigned char state);
}