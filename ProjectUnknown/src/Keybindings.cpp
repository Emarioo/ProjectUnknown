
#include "Keybindings.h"

#include <GLFW/glfw3.h>

void CreateDefualtKeybindings() {
	using namespace engone;
	ClearKeybindings();
	AddKeybinding(KeyForward, GLFW_KEY_W);
	AddKeybinding(KeyLeft, GLFW_KEY_A);
	AddKeybinding(KeyBack, GLFW_KEY_S);
	AddKeybinding(KeyRight, GLFW_KEY_D);
	AddKeybinding(KeyJump, GLFW_KEY_SPACE);
	AddKeybinding(KeyCrouch, GLFW_KEY_LEFT_CONTROL);
	AddKeybinding(KeySprint, GLFW_KEY_LEFT_SHIFT);
	AddKeybinding(KeyInventory, GLFW_KEY_E);
	AddKeybinding(KeyCrafting, GLFW_KEY_C);
	AddKeybinding(KeyPause, GLFW_KEY_ESCAPE);
	AddKeybinding(KeySwitchItem, GLFW_KEY_LEFT_CONTROL);
	
	SaveKeybindings(KEYBINDING_FILE);
}