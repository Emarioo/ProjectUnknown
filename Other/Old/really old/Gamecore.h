#pragma once

#include <vector>
#include "Objects/GameObject.h"

// This class handles the updating, gameloop

namespace gamecore {
	void Init();
	void Update(float delta);
	void Render();
	void LoadGameData();

	int GetTime();
}