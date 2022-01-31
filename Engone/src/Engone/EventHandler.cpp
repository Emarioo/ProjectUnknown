#include "gonpch.h"

#include "EventHandler.h"
#include "DebugHandler.h"

namespace engone
{
	struct Input
	{
		int code;
		bool down = false;
		int pressed = false;
		bool isGlfw = true;
	};
	static int mouseX, mouseY;
	static float scrollX, scrollY, scrollYConsole;
	static std::unordered_map<short, Keybinding> keybindings;
	static std::vector<Listener*> listeners;
	static std::vector<Event> events;
	static std::vector<Input> inputs;

	static INPUT_RECORD inRecord[8];
	static HANDLE inHandle;
	static DWORD numRead;

	EventType operator|(EventType a, EventType b)
	{
		return (EventType)((char)a | (char)b);
	}
	bool operator==(EventType a, EventType b)
	{
		return ((char)a & (char)b) > 0;
	}

	Event::Event(EventType type) : eventType(type) {}
	Listener::Listener(EventType eventTypes, std::function<EventType(Event&)> f)
		: eventTypes(eventTypes), run(f){}
	Listener::Listener(EventType eventTypes, int priority, std::function<EventType(Event&)> f)
		: eventTypes(eventTypes), run(f), priority(priority){}
	void SetInput(int code, bool down, bool isGlfw=true)
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
			inputs.push_back({ code, down, 1, isGlfw});
	}
	void ExecuteListeners()
	{
		for (int j = 0; j < events.size(); j++) {
			char breaker = 0; // if a flag is simular it will break
			for (int i = 0; i < listeners.size(); i++) {
				if ((char)listeners[i]->eventTypes & breaker)// continue if an event has been checked/used/disabled
					continue;

				if ((char)listeners[i]->eventTypes == (char)events[j].eventType) {
					EventType types = listeners[i]->run(events[j]);
					if ((char)types!=0)
						breaker = (char)(breaker | (char)listeners[i]->eventTypes);
				}
			}
		}
		while (events.size() > 0)
			events.pop_back();
	}

	void InitEvents()
	{
		inHandle = GetStdHandle(STD_INPUT_HANDLE);
	}
	void InitEvents(GLFWwindow* window)
	{
		glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
			Event e(EventType::Key);
			e.key = key;
			e.scancode = scancode;
			e.action = action;
			events.push_back(e);
			SetInput(key, action != 0);
			ExecuteListeners();
			});
		glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods) {
			Event e(EventType::Click);
			e.button = button;
			e.action = action;
			e.mx = mouseX;
			e.my = mouseY;
			events.push_back(e);
			SetInput(button, action == 1);
			ExecuteListeners();
			});
		glfwSetCursorPosCallback(window, [](GLFWwindow* window, double mx, double my) {
			mouseX = mx;
			mouseY = my;
			Event e(EventType::Move);
			e.mx = mx;
			e.my = my;
			events.push_back(e);
			ExecuteListeners();
			});
		glfwSetScrollCallback(window, [](GLFWwindow* window, double xoffset, double yoffset) {
			Event e(EventType::Scroll);
			e.scrollX = xoffset;
			e.scrollY = yoffset;
			e.mx = mouseX;
			e.my = mouseY;
			events.push_back(e);
			//std::cout << xoffset << " " << yoffset << "\n";
			scrollX = xoffset;
			scrollY = yoffset;
			ExecuteListeners();
			});
		glfwSetWindowFocusCallback(window, [](GLFWwindow* window, int focus) {
			Event e(EventType::Focus);
			e.focused = focus;
			events.push_back(e);
			ExecuteListeners();
			});
		glfwSetWindowSizeCallback(window, [](GLFWwindow* window, int width, int height) {
			Event e(EventType::Resize);
			e.width = width;
			e.height = height;
			events.push_back(e);
			ExecuteListeners();
			});
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
	int GetMouseX()	{return mouseX;}
	int GetMouseY(){return mouseY;}
	int IsScrolledY(bool isGlfw)
	{
		if (isGlfw) {
			if (scrollY != 0)
				return scrollY;
		}
		else {
			if (scrollYConsole != 0)
				return scrollY;
		}
		return 0;
	}
	int IsScrolledX(bool isGlfw)
	{
		if (isGlfw) {
			if (scrollX != 0)
				return scrollX;
		}
		return 0;
	}
	bool IsKeyDown(int code, bool isGlfw)
	{
		for (int i = 0; i < inputs.size(); i++) {
			if (inputs[i].code == code && inputs[i].isGlfw==isGlfw)
				return inputs[i].down;
		}
		return false;
	}
	bool IsKeyPressed(int code, bool isGlfw)
	{
		for (int i = 0; i < inputs.size(); i++) {
			if (inputs[i].code == code && inputs[i].isGlfw == isGlfw)
				return inputs[i].pressed > 0;
		}
		return false;
	}
	void ResetEvents()
	{
		scrollX = 0;
		scrollY = 0;
		scrollYConsole = 0;
		for (int i = 0; i < inputs.size(); i++) {
			if (inputs[i].pressed > 0)
				inputs[i].pressed--;
		}
	}
	bool IsKeybindingDown(short id)
	{
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
	bool IsKeybindingPressed(short id)
	{
		if (keybindings.count(id)) {
			Keybinding& bind = keybindings[id];

			if (!IsKeyPressed(bind.keys[0]))
				return false;
			if (bind.keys[1] != -1&&!IsKeyDown(bind.keys[1]))
				return false;
			if (bind.keys[2] != -1 && !IsKeyDown(bind.keys[2]))
				return false;
			return true;
		}
		return false;
	}
	void AddKeybinding(short id, int key0, int key1, int key2)
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
			//std::cout << sets[i].id << " " << sets[i].keys[0] << "\n";
			AddKeybinding(sets[i].id, sets[i].keys[0], sets[i].keys[1], sets[i].keys[2]);
		}

		// Cleanup
		delete[] sets;
		file.close();

		return numKeys;
	}
	bool SaveKeybindings(const std::string& path)
	{
		std::ofstream file(path, std::ios::binary);
		if (!file) {
			return false;
		}

		int numKeys = keybindings.size();

		struct keyset
		{
			short id;
			int keys[3];
		};

		keyset* sets = new keyset[numKeys];

		int i = 0;
		for (auto& pair : keybindings) {
			sets[i].id = pair.first;
			memcpy_s(&sets[i].keys[0], 3*sizeof(int), &pair.second.keys[0], 3*sizeof(int));
			//std::cout << sets[i].id << " " << sets[i].keys[0]<<"\n";
			i++;
		}

		file.write(reinterpret_cast<const char*>(&sets[0]), sizeof(keyset) * numKeys);

		// Cleanup
		delete[] sets;
		file.close();
		return true;
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
							scrollYConsole = inRecord[i].Event.MouseEvent.dwButtonState > 0 ? 1 : -1;
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