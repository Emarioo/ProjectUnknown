#pragma once

#include "Engone/GameState.h"

namespace GameState {
	enum GameState : unsigned char {
		Intro,
		Menu,
		Game,
		Paused,
		CameraToPlayer,
	};
}