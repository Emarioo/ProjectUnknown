#pragma once

namespace engine {

	// The point of this class is to convert numbers to char, string and the other way around
	//  Basically an intepreter for key codes, Also for testing for key presses

	void UpdateKeyboard(bool l);// Check current keyboard layout and change intepreter
	char GetChar(int c, bool shift, bool alt);
	/*
	Use GLFW_KEY_A for simplicity
	*/
	bool IsKey(int key);

}