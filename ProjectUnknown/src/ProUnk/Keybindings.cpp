
#include "ProUnk/Keybindings.h"

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
	AddKeybinding(KeyMoveCamera, GLFW_MOUSE_BUTTON_RIGHT);
	AddKeybinding(KeyPause, GLFW_KEY_ESCAPE);
	
	SaveKeybindings(KEYBINDING_FILE);
}