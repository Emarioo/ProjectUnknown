
#include "EventHandler.h"
#include "DebugHandler.h"

namespace engone {
	struct Input {
		int code;
		bool down = false;
		int pressed = false;
	};
	static int mouseX, mouseY;
	static float scrollY;
	static std::vector<Input> inputs;

	static INPUT_RECORD inRecord[8];
	static HANDLE inHandle;
	static DWORD numRead;

	void SetInput(int code, bool down, bool isGlfw = true) {
		for (int i = 0; i < inputs.size(); i++) {
			if (inputs[i].code == code) {
				if (down) {
					if (!inputs[i].down) {
						inputs[i].down = down;
						inputs[i].pressed++;
						return;
					}
				} else {
					inputs[i].down = false;
				}
				break;
			}
		}
		if (down)
			inputs.push_back({ code, down, 1 });
	}
	void InitEvents() {
		inHandle = GetStdHandle(STD_INPUT_HANDLE);
	}
	int GetMouseX() { 
		return mouseX; 
	}
	int GetMouseY() {
		return mouseY; 
	}
	int IsScrolledY() {
		if (scrollY != 0)
			return scrollY;
		return 0;
	}
	bool IsKeyDown(int code, bool isGlfw) {
		for (int i = 0; i < inputs.size(); i++) {
			if (inputs[i].code == code)
				return inputs[i].down;
		}
		return false;
	}
	bool IsKeyPressed(int code, bool isGlfw) {
		for (int i = 0; i < inputs.size(); i++) {
			if (inputs[i].code == code)
				return inputs[i].pressed > 0;
		}
		return false;
	}
	void ResetEvents() {
		scrollY = 0;
		for (int i = 0; i < inputs.size(); i++) {
			if (inputs[i].pressed > 0)
				inputs[i].pressed--;
		}
	}
	static bool lastL = false, lastM = false, lastR = false;
	void RefreshEvents() {
		if (inHandle != nullptr) {
			DWORD num;
			GetNumberOfConsoleInputEvents(inHandle, &num);
			if (num > 0) {
				ReadConsoleInput(inHandle, inRecord, 8, &numRead);
				for (int i = 0; i < numRead; i++) {
					//Event e;
					switch (inRecord[i].EventType) {
					case KEY_EVENT:
						SetInput(inRecord[i].Event.KeyEvent.wVirtualKeyCode, inRecord[i].Event.KeyEvent.bKeyDown, false);
						break;
					case MOUSE_EVENT:
						bool lb = inRecord[i].Event.MouseEvent.dwButtonState & 1;
						bool mb = (inRecord[i].Event.MouseEvent.dwButtonState >> 2) & 1;
						bool rb = (inRecord[i].Event.MouseEvent.dwButtonState >> 1) & 1;

						SetInput(VK_LBUTTON, lb, false);
						SetInput(VK_MBUTTON, mb, false);
						SetInput(VK_RBUTTON, rb, false);
						mouseX = inRecord[i].Event.MouseEvent.dwMousePosition.X;
						mouseY = inRecord[i].Event.MouseEvent.dwMousePosition.Y;

						switch (inRecord[i].Event.MouseEvent.dwEventFlags) {
						case 0:
							if (lastL != lb) {
								lastL = lb;
							}
							if (lastM != mb) {
								lastM = mb;
							}
							if (lastR != rb) {
								lastR = rb;
							}
							break;
						case MOUSE_MOVED:
							break;
						case MOUSE_WHEELED:
							scrollY = inRecord[i].Event.MouseEvent.dwButtonState > 0 ? 1 : -1;
							// what about horisontal scroll?
							break;
						}
						break;
					}
				}
			}
		}
	}
}