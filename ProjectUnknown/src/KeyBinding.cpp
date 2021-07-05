#include "KeyBinding.h"

#include <unordered_map>

#include "Engine/Keyboard.h"
#include "Engine/Utility/Debugger.h"

std::unordered_map<ActionKey, int> keyActionMap;
void AddKeyAction(ActionKey keyName, int keyCode) {
	keyActionMap[keyName] = keyCode;
}
bool TestKeyAction(ActionKey keyName, int testCode) {
	if (keyActionMap.count(keyName))
		return testCode==keyActionMap[keyName];
	return false;
}
bool IsKeyActionDown(ActionKey keyName) {
	if (keyActionMap.count(keyName))
		return engine::IsKey(keyActionMap[keyName]);
	return false;
}