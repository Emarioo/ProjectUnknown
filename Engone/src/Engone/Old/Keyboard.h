#pragma once

namespace engone {

	// The point of this class is to convert numbers to char, string and the other way around
	//  Basically an intepreter for key codes, Also for testing for key presses

	void UpdateKeyboard(bool l);// Check current keyboard layout and change intepreter
	char GetChar(int c, bool shift, bool alt);
	/*
	Use GLFW_KEY_A for simplicity
	*/
	bool IsKey(int key);
	/*
	void AddActionKey(unsigned short keyName, int keyCode);
	bool TestActionKey(unsigned short keyName, int testCode);
	bool IsKeyActionDown(unsigned short keyName);

	/*
	Returns number of keys loaded. Zero could mean that the keybindings file has been removed.
	Less keys than there should be will most likely cause issues in the gameplay.
	
	int LoadKeybindings(const std::string& path);
	void SaveKeybindings(const std::string& path);
	void ClearKeybindings();
	*/
}