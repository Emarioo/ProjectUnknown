#pragma once

#include "Engine/Engine.h"

#include "GameStates.h"

namespace UI {
	void InitializeUI();

	// These functions exists in seperate cpp files (reason is because they contain a lot of code)
	void InitStartMenu();
	void InitPauseMenu();
	void InitGameMenu();
}