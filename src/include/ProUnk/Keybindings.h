#pragma once

#include "Engone/EventModule.h"

// not sure if preprocessor definition is necessary for this path but, ehhh. why not I guess.
#define KEYBINDING_FILE "data/keybindings.dat"

enum ActionKey : uint16_t {
	KeyForward=0,KeyLeft,KeyBack,KeyRight, // WASD movement
	KeyJump, KeyCrouch, KeySprint, // other movement
	KeyMoveCamera, // camera movement
	KeyPause, // pause
};

// Will create a keybindings file.
void CreateDefualtKeybindings();