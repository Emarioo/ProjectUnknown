#pragma once

enum ActionKey : int{
	KeyForward,KeyRight,KeyLeft,KeyBack, KeyJump, KeyCrouch, KeySprint,
	KeyInventory, KeyCrafting, KeyPause
};

void AddKeyAction(ActionKey keyName, int keyCode);
bool TestKeyAction(ActionKey keyName, int testCode);
bool IsKeyActionDown(ActionKey keyName);