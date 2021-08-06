#pragma once

#include "Engine/GameState.h"

namespace GameState {
	enum GameState : unsigned char {
		Intro,
		Menu,
		Game,
		Paused,
		CameraToPlayer,
	};
}