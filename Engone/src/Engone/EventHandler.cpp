
#include "EventHandler.h"
#include "Logger.h"

#include "Window.h"

namespace engone
{
	struct Input
	{
		int code;
		bool down = false;
		int pressed = false;
	};
	static int mouseX, mouseY;
	static float scrollX, scrollY;
	static std::unordered_map<uint16_t, Keybinding> keybindings;
	static std::vector<Listener*> listeners;
	static std::vector<Event> events;
	static std::vector<Input> inputs;

	EventType operator|(EventType a, EventType b) {
		return (EventType)((char)a | (char)b);
	}
	bool operator==(EventType a, EventType b) {
		return ((char)a & (char)b) > 0;
	}

	Event::Event(EventType type) : eventType(type) {}
	Listener::Listener(EventType eventTypes, std::function<EventType(Event&)> f)
		: eventTypes(eventTypes), run(f){}
	Listener::Listener(EventType eventTypes, int priority, std::function<EventType(Event&)> f)
		: eventTypes(eventTypes), run(f), priority(priority){}
	void SetInput(int code, bool down, bool isGlfw=true) {
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
			inputs.push_back({ code, down, 1});
	}
	void ExecuteListeners() {
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
	static int charCount = 0;
	static int readIndex = 0;
	static const int ARRAY_SIZE = 20;
	static uint32_t charArray[ARRAY_SIZE];
	void CharCallback(GLFWwindow* window, uint32_t chr) {
		//log::out << (char)chr << "\n";
		if (readIndex != 0) {
			for (int i = readIndex; i < charCount; i++)
				charArray[i - readIndex] = charArray[readIndex];
			charCount = charCount - readIndex;
			readIndex = 0;
			ZeroMemory(charArray + charCount, ARRAY_SIZE - charCount);
		}
		if (charCount < ARRAY_SIZE) {
			charArray[charCount++] = chr;
		}
	}
	static std::vector<std::string> pathDrops;
	void DropCallback(GLFWwindow* window, int count, const char** paths) {
		for (int i = 0; i < count;i++) {
			pathDrops.push_back(paths[i]);
		}
	}
	std::string PollClipboard() {
		return glfwGetClipboardString(GetWindow()->glfw());
	}
	void SetClipboard(const char* str) {
		return glfwSetClipboardString(GetWindow()->glfw(),str);
	}
	std::string PollPathDrop() {
		if (pathDrops.size() == 0) return "";
		std::string path = pathDrops.back();
		pathDrops.pop_back();
		return path;
	}
	uint32_t PollChar() {
		if (readIndex < ARRAY_SIZE) {
			if (charArray[readIndex] != 0) {
				return charArray[readIndex++];
			}
		}
		return 0;
	}
	void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
		Window* win = GetMappedWindow(window);
		if (!win) {
			log::out << "Window isn't mapped\n";
			return;
		}
		if (!win->hasFocus()) return;

		Event e(EventType::Key);
		e.key = key;
		e.scancode = scancode;
		e.action = action;
		events.push_back(e);

		SetInput(key, action != 0);

		if (action!=0 && (key == GLFW_KEY_BACKSPACE || key == GLFW_KEY_DELETE || key == GLFW_KEY_ENTER||key==GLFW_KEY_LEFT||key==GLFW_KEY_RIGHT)) {
			CharCallback(window,key);
		}

		ExecuteListeners();
	}
	void MouseCallback(GLFWwindow* window, int button, int action, int mods) {
		Window* win = GetMappedWindow(window);
		if (!win) {
			log::out << "Window isn't mapped\n";
			return;
		}
		if (!win->hasFocus()) return;

		Event e(EventType::Click);
		e.button = button;
		e.action = action;
		e.mx = mouseX;
		e.my = mouseY;
		events.push_back(e);
		SetInput(button, action == 1);
		ExecuteListeners();
	}
	void CursorPosCallback(GLFWwindow* window, double mx, double my) {
		Window* win = GetMappedWindow(window);
		if (!win) {
			log::out << "Window isn't mapped\n";
			return;
		}
		if (!win->hasFocus()) return;

		mouseX = mx;
		mouseY = my;
		Event e(EventType::Move);
		e.mx = mx;
		e.my = my;
		events.push_back(e);
		ExecuteListeners();
	}
	void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
		Window* win = GetMappedWindow(window);
		if (!win) {
			log::out << "Window isn't mapped\n";
			return;
		}
		if (!win->hasFocus()) return;

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
	}
	void InitEvents(GLFWwindow* window) {
		glfwSetKeyCallback(window, KeyCallback);
		glfwSetMouseButtonCallback(window,	MouseCallback);
		glfwSetCursorPosCallback(window, CursorPosCallback);
		glfwSetScrollCallback(window, ScrollCallback);
		glfwSetCharCallback(window, CharCallback);
		glfwSetDropCallback(window, DropCallback);

		ZeroMemory(charArray, sizeof(charArray));

		/*
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
		*/
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
		if (scrollY != 0)
			return scrollY;
		
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
			if (inputs[i].code == code)
				return inputs[i].down;
		}
		return false;
	}
	bool IsKeyPressed(int code, bool isGlfw)
	{
		for (int i = 0; i < inputs.size(); i++) {
			if (inputs[i].code == code)
				return inputs[i].pressed > 0;
		}
		return false;
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
	bool IsKeybindingDown(uint16_t id)
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
	bool IsKeybindingPressed(uint16_t id)
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
	void AddKeybinding(uint16_t id, int key0, int key1, int key2)
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
	void ClearKeybindings() { 
		keybindings.clear();
	}

}