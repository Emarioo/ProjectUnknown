#include "gonpch.h"

#include <unordered_map>
#include "EventHandler.h"
#include "Rendering/Renderer.h"

namespace engone
{
	static std::vector<EventListener*> listeners;
	static std::vector<Event> events;
	static std::unordered_map<unsigned short, int> keyboard_keyActionMap;

	static int mouseX, mouseY;


	EventListener::EventListener(EventType type, std::function<bool(Event&)> f)
		: eventType(type), run(f)
	{
		
	}

	void AddListener(EventListener* listener)
	{
		for (int i = 0; i < listeners.size();i++) {
			if (listeners[i] == listener) {
				std::cout << "I don't think you want to add the same listener.\n";
				return;
			}
		}
		listeners.push_back(listener);
	}
	int GetMouseX()
	{
		return mouseX;
	}
	int GetMouseY()
	{
		return mouseY;
	}
	void AddEvent(Event e)
	{
		events.push_back(e);
	}
	void DispatchEvents()
	{
		for (int i = 0; i < listeners.size();i++) {
			for (int j = 0; j < events.size();j++) {
				//std::cout << (int)listeners[i]->eventType<<" "<<(int)events[j].eventType<<"\n";
				if ((char)listeners[i]->eventType & (char)events[j].eventType) {
					//std::cout << "run\n";
					listeners[i]->run(events[j]);
				}
			}
		}
		while (events.size() > 0)
			events.pop_back();
	}
	void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
		// This is for debug purposes
		if (key == GLFW_KEY_1) {
			SetWindowType(Windowed);
		}
		else if (key == GLFW_KEY_2) {
			SetWindowType(Fullscreen);
		}
		else if (key == GLFW_KEY_3) {
			SetWindowType(BorderlessFullscreen);
		}
		Event e=Event(EventType::Key);
		e.key = key;
		e.scancode = scancode;
		e.action = action;
		
		AddEvent(e);

		//std::cout << "key "<<(char)key << "\n";

		DispatchEvents();
		//if (KeyEvent != nullptr) KeyEvent(key, action);
		
	}
	void MouseClickCallback(GLFWwindow* window, int action, int button, int mods) {
		Event e(EventType::Click);
		e.action=action;
		e.button=button;
		e.mx=GetMouseX();
		e.my=GetMouseY();
		AddEvent(e);
		DispatchEvents();
		//if (MouseEvent != nullptr) MouseEvent(renMouseX, renMouseY, action, button);
	}
	void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
		Event e(EventType::Scroll);
		e.scroll = yoffset;
		AddEvent(e);
		DispatchEvents();
		//if (ScrollEvent != nullptr)  ScrollEvent(yoffset);
		
	}
	void MouseMoveCallback(GLFWwindow* window, double mx, double my) {
		mouseX = mx;
		mouseY = my;
		Event e(EventType::Move);
		e.mx = mx;
		e.my = my;
		AddEvent(e);
		DispatchEvents();
		/*if (IsCursorLocked()) {
			GetCamera()->rotation.y -= (mx - renMouseX) * (3.14159f / 360) * cameraSensitivity;
			GetCamera()->rotation.x -= (my - renMouseY) * (3.14159f / 360) * cameraSensitivity;
		}
		if (DragEvent != nullptr)
			DragEvent(mx, my);
		renMouseX = mx;
		renMouseY = my;*/
	}
	void ResizeCallback(GLFWwindow* window, int width, int height) {
		Event e(EventType::Resize);
		e.rw = width;
		e.rh = height;
		AddEvent(e);
		DispatchEvents();
		/*glViewport(0, 0, width, height);
		if (windowType == Windowed) {
			winW = width;
			winH = height;
		}
		if (ResizeEvent != nullptr)
			ResizeEvent(width, height);*/
	}
	void WindowFocusCallback(GLFWwindow* window, int focus) {
		Event e(EventType::Focus);
		e.focused = focus;
		AddEvent(e);
		DispatchEvents();
		/*windowHasFocus = focus;
		if (FocusEvent != nullptr) {
			FocusEvent(focus);
		}*/
	}
	void InitEvents()
	{
		glfwSetKeyCallback(GetWindow(),KeyCallback);
		glfwSetMouseButtonCallback(GetWindow(), MouseClickCallback);
		glfwSetScrollCallback(GetWindow(), ScrollCallback);
		glfwSetCursorPosCallback(GetWindow(), MouseMoveCallback);
		glfwSetWindowFocusCallback(GetWindow(), WindowFocusCallback);
		glfwSetWindowSizeCallback(GetWindow(), ResizeCallback);
	}
	bool IsKeyDown(int key)
	{
		return glfwGetKey(GetWindow(), key) == GLFW_PRESS;
	}
	bool IsActionDown(unsigned short action)
	{
		if (keyboard_keyActionMap.count(action))
			return glfwGetKey(GetWindow(), keyboard_keyActionMap[action]) == GLFW_PRESS;
		return false;
	}
	void AddActionKey(unsigned short keyName, int keyCode)
	{
		keyboard_keyActionMap[keyName] = keyCode;
	}
	/*
	path directory starts where the .exe is. Example "data/keybindings.dat"
	return true if successful
	*/
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

		int numKeys = (int)(size / (6.));

		//std::cout << "NumKeys "<<numKeys << std::endl;

		unsigned short* keys = new unsigned short[numKeys];
		file.read(reinterpret_cast<char*>(&keys[0]), sizeof(unsigned short) * numKeys);
		int* codes = new int[numKeys];
		file.read(reinterpret_cast<char*>(&codes[0]), sizeof(int) * numKeys);

		for (int i = 0; i < numKeys; i++) {
			//std::cout << keys[i] << " "<<codes[i] << std::endl;
			AddActionKey(keys[i], codes[i]);
		}

		// Cleanup
		delete[] keys;
		delete[] codes;
		file.close();

		return numKeys;
	}
	void SaveKeybindings(const std::string& path)
	{
		std::ofstream file(path, std::ios::binary);

		int numKeys = keyboard_keyActionMap.size();

		//std::cout << "NumKeys "<<numKeys << std::endl;

		unsigned short* keys = new unsigned short[numKeys];
		int* codes = new int[numKeys];

		int i = 0;
		for (auto& pair : keyboard_keyActionMap) {
			keys[i] = pair.first;
			codes[i] = pair.second;
			//std::cout << keys[i] <<" "<< codes[i] << std::endl;
			i++;
		}

		file.write(reinterpret_cast<const char*>(&keys[0]), sizeof(unsigned short) * numKeys);
		file.write(reinterpret_cast<const char*>(&codes[0]), sizeof(int) * numKeys);

		// Cleanup
		delete[] keys;
		delete[] codes;
		file.close();
	}
	void ClearKeybindings()
	{
		keyboard_keyActionMap.clear();
	}
}