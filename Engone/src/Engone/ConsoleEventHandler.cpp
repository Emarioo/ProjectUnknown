#pragma once

#include "ConsoleEventHandler.h"
namespace input
{
	struct Input
	{
		int code;
		bool down = false;
		int pressed = false;
	};
	static int mouseX, mouseY;
	static float scrollX, scrollY;
	static std::unordered_map<short, Keybinding> keybindings;
	static std::vector<Listener*> listeners;
	static std::vector<Event> events;
	static std::vector<Input> inputs;

	static INPUT_RECORD inRecord[8];
	static HANDLE inHandle;
	static DWORD numRead;

	inline EventType operator|(EventType a, EventType b)
	{
		return (EventType)((char)a | (char)b);
	}
	inline bool operator==(EventType a, EventType b)
	{
		//std::cout << int(a) << " - " << int(b)<<" "<<(((char)a & (char)b)>0) << "\n";
		return (char(a) & char(b)) > 0;
	}

	Event::Event(EventType type) : eventType(type) {}
	Listener::Listener(EventType eventTypes, std::function<bool(Event&)> f)
		: eventTypes(eventTypes), run(f)
	{
	}
	Listener::Listener(EventType eventTypes, int priority, std::function<bool(Event&)> f)
		: eventTypes(eventTypes), run(f), priority(priority)
	{
	}
	void SetInput(int code, bool down)
	{
		for (int i = 0; i < inputs.size(); i++) {
			if (inputs[i].code == code) {
				if (down) {
					if (!inputs[i].down) {
						inputs[i].down = down;
						inputs[i].pressed++;
						return;
					}
				}
				else {
					inputs[i].down = false;
				}
				break;
			}
		}
		if (down)
			inputs.push_back({ code, down, 1 });
	}
	void ExecuteListeners()
	{
		char breaker = 0;
		for (int j = 0; j < events.size(); j++) {
			for (int i = 0; i < listeners.size(); i++) {
				if ((char)listeners[i]->eventTypes & breaker)
					continue;

				if ((char)listeners[i]->eventTypes & (char)events[j].eventType) {
					//std::cout << "hm\n";
					if (listeners[i]->run(events[j]))
						breaker = (char)(breaker | (char)listeners[i]->eventTypes);
				}
			}
		}
		while (events.size() > 0)
			events.pop_back();
	}

	void Init()
	{
		inHandle = GetStdHandle(STD_INPUT_HANDLE);
	}
	void AddListener(Listener* listener)
	{
		// Prevent duplicates
		for (int i = 0; i < listeners.size(); i++) {
			if (listener == listeners[i]) {
				return;
			}
		}
		// Priorities
		for (int i = 0; i < listeners.size(); i++) {
			if (listener->priority > listeners[i]->priority) {
				listeners.insert(listeners.begin() + i, listener);
				return;
			}
		}
		listeners.push_back(listener);
	}
	int GetMouseX() { return mouseX; }
	int GetMouseY() { return mouseY; }
	bool IsDown(int code)
	{
		for (int i = 0; i < inputs.size(); i++) {
			if (inputs[i].code == code)
				return inputs[i].down;
		}
		return false;
	}
	bool IsPressed(int code)
	{
		for (int i = 0; i < inputs.size(); i++) {
			if (inputs[i].code == code)
				return inputs[i].pressed > 0;
		}
		return false;
	}
	int IsScrolledY()
	{
		if (scrollY != 0)
			return scrollY;
		return 0;
	}
	int IsScrolledX()
	{
		if (scrollX != 0)
			return scrollX;
		return 0;
	}
	void ResetEvents()
	{
		scrollX = 0;
		scrollY = 0;
		for (int i = 0; i < inputs.size(); i++) {
			if (inputs[i].pressed > 0)
				inputs[i].pressed--;
		}
	}
	bool IsKeybindingDown(short id)
	{
		if (keybindings.count(id)) {
			Keybinding& bind = keybindings[id];
			if (IsDown(bind.keys[0])) {
				if (bind.keys[1] == 0) {
					return true;
				}
				else if (IsDown(bind.keys[1])) {
					if (bind.keys[2] == 0) {
						return true;
					}
					else if (IsDown(bind.keys[2])) {
						return true;
					}
				}
			}
		}
		return false;
	}
	void AddKeybinding(short id, int key0 = 0, int key1 = 0, int key2 = 0)
	{
		keybindings[id] = { key0,key1,key2 };
	}
	int LoadKeybindings(const std::string& path)
	{
		std::ifstream file(path, std::ios::binary);

		if (!file) {
			file.close();
			return 0;
		}

		file.seekg(0, file.end);
		int size = file.tellg();
		file.seekg(0, file.beg);

		struct keyset
		{
			short id;
			int keys[3];
		};

		int numKeys = (int)(size / (sizeof(keyset)));

		keyset* sets = new keyset[numKeys];

		file.read(reinterpret_cast<char*>(&sets[0]), sizeof(keyset) * numKeys);

		for (int i = 0; i < numKeys; i++) {
			AddKeybinding(sets[i].id, sets[i].keys[0], sets[i].keys[1], sets[i].keys[2]);
		}

		// Cleanup
		delete[] sets;
		file.close();

		return numKeys;
	}
	void SaveKeybindings(const std::string& path)
	{
		std::ofstream file(path, std::ios::binary);

		int numKeys = keybindings.size();

		struct keyset
		{
			short id;
			int keys[3];
		};

		keyset* sets = new keyset[numKeys];

		int i = 0;
		for (auto& pair : keybindings) {
			sets->id = pair.first;
			memcpy_s(sets->keys, 3, pair.second.keys, 3);
			i++;
		}

		file.write(reinterpret_cast<const char*>(&sets[0]), sizeof(keyset) * numKeys);

		// Cleanup
		delete[] sets;
		file.close();
	}
	void ClearKeybindings() { keybindings.clear(); }
	static bool lastL = false, lastM = false, lastR = false;
	void RefreshEvents()
	{
		if (inHandle != nullptr) {
			DWORD num;
			GetNumberOfConsoleInputEvents(inHandle, &num);
			if (num > 0) {
				ReadConsoleInput(inHandle, inRecord, 8, &numRead);
				for (int i = 0; i < numRead; i++) {
					Event e;
					switch (inRecord[i].EventType) {
					case KEY_EVENT:
						input::SetInput(inRecord[i].Event.KeyEvent.wVirtualKeyCode, inRecord[i].Event.KeyEvent.bKeyDown);
						e.eventType = EventType::Key;
						e.key = inRecord[i].Event.KeyEvent.wVirtualKeyCode;
						e.scancode = inRecord[i].Event.KeyEvent.wVirtualScanCode;
						e.action = inRecord[i].Event.KeyEvent.bKeyDown;
						events.push_back(e);
						break;
					case MOUSE_EVENT:
						bool lb = inRecord[i].Event.MouseEvent.dwButtonState & 1;
						bool mb = (inRecord[i].Event.MouseEvent.dwButtonState >> 2) & 1;
						bool rb = (inRecord[i].Event.MouseEvent.dwButtonState >> 1) & 1;

						input::SetInput(VK_LBUTTON, lb);
						input::SetInput(VK_MBUTTON, mb);
						input::SetInput(VK_RBUTTON, rb);
						mouseX = inRecord[i].Event.MouseEvent.dwMousePosition.X;
						mouseY = inRecord[i].Event.MouseEvent.dwMousePosition.Y;

						e.mx = mouseX;
						e.my = mouseY;
						switch (inRecord[i].Event.MouseEvent.dwEventFlags) {
						case 0:
							e.eventType = EventType::Click;
							if (lastL != lb) {
								e.action = lb;
								e.button = VK_LBUTTON;
								lastL = lb;
								events.push_back(e);
							}
							if (lastM != mb) {
								e.action = mb;
								e.button = VK_MBUTTON;
								lastM = mb;
								events.push_back(e);
							}
							if (lastR != rb) {
								e.action = rb;
								e.button = VK_RBUTTON;
								lastR = rb;
								events.push_back(e);
							}
							break;
						case MOUSE_MOVED:
							e.eventType = EventType::Move;
							events.push_back(e);
							break;
						case MOUSE_WHEELED:
							e.eventType = EventType::Scroll;
							scrollY = inRecord[i].Event.MouseEvent.dwButtonState > 0 ? 1 : -1;
							e.scrollY = scrollY;
							// what about horisontal scroll?
							events.push_back(e);
							break;
						}
						break;
					}
				}
			}
		}
		ExecuteListeners();
	}
}