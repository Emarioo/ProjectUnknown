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
	enum class EngoneOption : uint32_t {
		None = 0,
		UI = 1,
		Game3D = 2,
		Network = 4,
		Sound = 8,
		MultiThreaded = 16,
	};

	EngoneOption operator|(EngoneOption a, EngoneOption b);
	bool operator==(EngoneOption a, EngoneOption b);

	EngoneOption GetEngoneOptions();
	bool HasEngoneOption(EngoneOption option);
	// Doesn't work that well
	void SetEngoneOptions(EngoneOption hints);

	void SetState(unsigned char state,bool f);
	bool CheckState(unsigned char state);
}