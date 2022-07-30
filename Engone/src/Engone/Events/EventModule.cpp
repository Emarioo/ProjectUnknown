
#include "Engone/EventModule.h"
#include "Engone/Window.h"

namespace engone {
	TrackerId Listener::trackerId="Listener";

	static std::unordered_map<uint16_t, Keybinding> keybindings;

	Listener::Listener(EventTypes eventTypes, std::function<EventTypes(Event&)> f)
		: eventTypes(eventTypes), run(f) {}
	Listener::Listener(EventTypes eventTypes, int priority, std::function<EventTypes(Event&)> f)
		: eventTypes(eventTypes), run(f), priority(priority) {}

	std::string PollClipboard() {
		return glfwGetClipboardString(GetActiveWindow()->glfw());
	}
	void SetClipboard(const char* str) {
		return glfwSetClipboardString(GetActiveWindow()->glfw(), str);
	}
	std::string PollPathDrop() {
		Window* win = GetActiveWindow();
		if (win) return win->pollPathDrop();
		else return "";
	}
	uint32_t PollChar() {
		Window* win = GetActiveWindow();
		if (win) return win->pollChar();
		else return 0;
	}
	float GetMouseX() {
		Window* win = GetActiveWindow();
		if (win) return win->getMouseX();
		return -1;
	}
	float GetMouseY() {
		Window* win = GetActiveWindow();
		if (win) return win->getMouseY();
		return -1;
	}
	float IsScrolledX() {
		Window* win = GetActiveWindow();
		if (win) return win->getScrollX();
		return 0;
	}
	float IsScrolledY() {
		Window* win = GetActiveWindow();
		if (win) return win->getScrollY();
		return 0;
	}
	bool IsKeyDown(int code) {
		Window* win = GetActiveWindow();
		if (win) return win->isKeyDown(code);
		return 0;
	}
	bool IsKeyPressed(int code) {
		Window* win = GetActiveWindow();
		if (win) return win->isKeyPressed(code);
		return 0;
	}
	bool IsKeybindingDown(uint16_t id) {
		if (keybindings.count(id)) {
			Keybinding& bind = keybindings[id];

			if (!IsKeyDown(bind.keys[0]))
				return false;
			if (bind.keys[1] != -1 && !IsKeyDown(bind.keys[1]))
				return false;
			if (bind.keys[2] != -1 && !IsKeyDown(bind.keys[2]))
				return false;
			return true;
		}
		return false;
	}
	bool IsKeybindingPressed(uint16_t id) {
		if (keybindings.count(id)) {
			Keybinding& bind = keybindings[id];

			if (!IsKeyPressed(bind.keys[0]))
				return false;
			if (bind.keys[1] != -1 && !IsKeyDown(bind.keys[1]))
				return false;
			if (bind.keys[2] != -1 && !IsKeyDown(bind.keys[2]))
				return false;
			return true;
		}
		return false;
	}
	void AddKeybinding(uint16_t id, int key0, int key1, int key2) {
		keybindings[id] = { key0,key1,key2 };
	}
	int LoadKeybindings(const std::string& path) {
		std::ifstream file(path, std::ios::binary);

		if (!file) {
			file.close();
			return 0;
		}
		file.seekg(0, file.end);
		uint32_t size = (uint32_t)file.tellg();
		file.seekg(0, file.beg);

		struct keyset {
			short id;
			int keys[3];
		};

		int numKeys = (int)(size / (sizeof(keyset)));

		keyset* sets = new keyset[numKeys];

		file.read(reinterpret_cast<char*>(&sets[0]), sizeof(keyset) * numKeys);

		for (int i = 0; i < numKeys; i++) {
			//std::cout << sets[i].id << " " << sets[i].keys[0] << "\n";
			AddKeybinding(sets[i].id, sets[i].keys[0], sets[i].keys[1], sets[i].keys[2]);
		}

		// Cleanup
		delete[] sets;
		file.close();

		return numKeys;
	}
	bool SaveKeybindings(const std::string& path) {
		std::ofstream file(path, std::ios::binary);
		if (!file) {
			return false;
		}

		int numKeys = keybindings.size();

		struct keyset {
			uint16_t id;
			int keys[3];
		};

		keyset* sets = new keyset[numKeys];

		int i = 0;
		for (auto& pair : keybindings) {
			sets[i].id = pair.first;
			for (int j = 0; j < 3;j++)
				sets[i].keys[j] = pair.second.keys[j];

			i++;
		}

		file.write(reinterpret_cast<const char*>(&sets[0]), sizeof(keyset) * numKeys);

		// Cleanup
		delete[] sets;
		file.close();
		return true;
	}
	void ClearKeybindings() {
		keybindings.clear();
	}

}