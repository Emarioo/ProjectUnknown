#include "KeyBinding.h"

#include <unordered_map>

#include "Engine/Keyboard.h"
#include "Engine/Handlers/FileHandler.h"

#include "GLFW/glfw3.h"

std::unordered_map<ActionKey, int> keyActionMap;
void AddActionKey(ActionKey keyName, int keyCode) {
	keyActionMap[keyName] = keyCode;
}
bool TestActionKey(ActionKey keyName, int testCode) {
	if (keyActionMap.count(keyName))
		return testCode==keyActionMap[keyName];
	return false;
}
bool IsKeyActionDown(ActionKey keyName) {
	if (keyActionMap.count(keyName))
		return engine::IsKey(keyActionMap[keyName]);
	return false;
}
void InitKeyBindings() {
	engine::FileReport err;
	std::vector<std::string> keyData = engine::ReadTextFile("data/keybindings.dat", &err);
	if (err == engine::FileReport::Success) {
		for (std::string str : keyData) {
			std::vector<std::string> split = engine::SplitString(str, "=");
			if (split.size() == 2) {
				ActionKey keyName = (ActionKey)std::stoi(split[0]);
				int code = std::stoi(split[1]);
				AddActionKey(keyName, code);
			}
		}
	}
	else {
		bug::out < "Error with key bindings. Creating standard bindings\n";
		AddActionKey(KeyForward, GLFW_KEY_W);
		AddActionKey(KeyRight, GLFW_KEY_D);
		AddActionKey(KeyBack, GLFW_KEY_S);
		AddActionKey(KeyLeft, GLFW_KEY_A);
		AddActionKey(KeyJump, GLFW_KEY_SPACE);
		AddActionKey(KeyCrouch, GLFW_KEY_LEFT_SHIFT);
		AddActionKey(KeySprint, GLFW_KEY_F);
		AddActionKey(KeyInventory, GLFW_KEY_E);
		AddActionKey(KeyCrafting, GLFW_KEY_C);
		AddActionKey(KeyPause, GLFW_KEY_ESCAPE);
		SaveKeyBindings();
	}
}
void SaveKeyBindings() {
	std::vector<std::string> keyData;
	
	for (auto& pair : keyActionMap) {
		keyData.push_back(std::to_string(pair.first)+"="+std::to_string(pair.second));
	}

	engine::FileReport err = engine::WriteTextFile("data/keybindings.dat",keyData);
}