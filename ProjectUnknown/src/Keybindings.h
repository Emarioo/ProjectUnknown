#pragma once

//#include "Engone/Keyboard.h"
#include "Engone/EventModule.h"

#define KEYBINDING_FILE "data/keybindings.dat"

#define KEY_COUNT 11

enum ActionKey : uint16_t {
	KeyForward=0,KeyLeft,KeyBack,KeyRight, KeyJump, KeyCrouch, KeySprint,
	KeyInventory, KeyCrafting, KeyPause,
	KeySwitchItem
};

/*
Will automatically create a keybindings file.
*/
void CreateDefualtKeybindings();