#pragma once

enum ActionKey : int{
	KeyForward,KeyRight,KeyLeft,KeyBack, KeyJump, KeyCrouch, KeySprint,
	KeyInventory, KeyCrafting, KeyPause
};

void AddActionKey(ActionKey keyName, int keyCode);
bool TestActionKey(ActionKey keyName, int testCode);
bool IsKeyActionDown(ActionKey keyName);
void InitKeyBindings();
void SaveKeyBindings();