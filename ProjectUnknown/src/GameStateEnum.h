#pragma once

#include "Engone/GameState.h"

namespace GameState {
	enum CustomGameState : unsigned char {
		Intro,
		Menu,
		Paused,
		CameraToPlayer,
	};
}