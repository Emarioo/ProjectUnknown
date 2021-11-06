#pragma once

#include "Rendering/Renderer.h"

namespace engone {

	/*
	Init usage of glfw3.
	*/
	void InitEvents(GLFWwindow* window);
	/*
	Init usage of windows console.
	*/
	void InitEvents();

	enum class EventType : char
	{
		None=0,
		GLFW=1,
		Console=2,
		Click=4,
		Move=8,
		Scroll=16,
		Key=32,
		Resize=64,
		Focus=128
	};
	EventType operator|(EventType a, EventType b);
	bool operator==(EventType a, EventType b);
	struct Keybinding
	{
		int keys[3];
	};
	class Event
	{
	public:
		Event()=default;
		Event(EventType t);
	
		EventType eventType;

		union
		{
			struct // key
			{
				int action, key, scancode;
			};
			struct // mouse click move
			{
				int action, button, mx, my;
			};
			struct // mouse scroll
			{
				float scrollX, scrollY;
				int mx, my;
			};
			struct // Event functions
			{
				// from the resize event
				int width, height;
				bool focused;
			};
		};
	};
	/*
	Cannot listen for multiple different events that uses the same member variables in the union.
	Mouse click, move and scroll events are fine but not mouse and key events.
	Use EventType::Click|EventType::Move|EventType::Scroll as first argument to listen to multiple events
	The bool of the function will stop an event from being listened from other listeners.
	*/
	class Listener
	{
	public:
		Listener(EventType eventTypes, std::function<EventType(Event&)> f);
		/*
		@priority determines if this listener should be called first when handling events. A high number is more important.
		This takes effect if the listener's function returns true which will stop other listeners.
		*/
		Listener(EventType eventTypes, int priority, std::function<EventType(Event&)> f);
		std::function<EventType(Event&)> run;
		int priority = 0;
		EventType eventTypes;
	};

	void AddListener(Listener* listener);

	// Use this at the beginning of you gameloop.
	void RefreshEvents();
	// Use this at the end of you gameloop.
	void ResetEvents();

	int GetMouseX();
	int GetMouseY();
	/*
	return 0 if no scroll and 1 or -1 if scrolled.
	*/
	int IsScrolledY();
	int IsScrolledX();

	/*
	virtualKeyCode: Windows.h or glfw virtual key code depending on what you are using.
	return true if the key or mouse button is down/held.
	*/
	bool IsKeyDown(int virtualKeyCode);
	/*
	virtualKeyCode: Windows.h or glfw virtual key code depending on what you are using.
	return true if the key was pressed this refresh/frame/update.
	*/
	bool IsKeyPressed(int virtualKeyCode);

	bool IsKeybindingDown(short id);
	bool IsKeybindingPressed(short id);
	/*
	When using IsKeybindingDown: All of the specified keycodes needs to be down.
	When using IsKeybindingPressed: The last two keycodes needs to be down before pressing the first one.
	*/
	void AddKeybinding(short id, int keyCode0, int keyCode1=-1, int keyCode2=-1);
	/*
	Returns number of keys loaded. Zero means that an empty keybindings file was loaded or that there is no file. Either way, default keybindings should be created.
	Less keybindings than there should be will most likely cause issues in the gameplay.
	@path is the path of the file. data/keybindings.dat for example. The format is irrelevant since it will use binary either way.
	*/
	int LoadKeybindings(const std::string& path);
	void SaveKeybindings(const std::string& path);
	void ClearKeybindings();
}