#include "propch.h"

#include "Keybindings.h"

#include "GLFW/glfw3.h"

void CreateDefualtKeybindings() {
	using namespace engone;
	ClearKeybindings();
	AddActionKey(KeyForward, GLFW_KEY_W);
	AddActionKey(KeyLeft, GLFW_KEY_A);
	AddActionKey(KeyBack, GLFW_KEY_S);
	AddActionKey(KeyRight, GLFW_KEY_D);
	AddActionKey(KeyJump, GLFW_KEY_SPACE);
	AddActionKey(KeyCrouch, GLFW_KEY_LEFT_SHIFT);
	AddActionKey(KeySprint, GLFW_KEY_F);
	AddActionKey(KeyInventory, GLFW_KEY_E);
	AddActionKey(KeyCrafting, GLFW_KEY_C);
	AddActionKey(KeyPause, GLFW_KEY_ESCAPE);
	AddActionKey(KeySwitchItem, GLFW_KEY_LEFT_CONTROL);
	
	SaveKeybindings(KEYBINDING_FILE);
}