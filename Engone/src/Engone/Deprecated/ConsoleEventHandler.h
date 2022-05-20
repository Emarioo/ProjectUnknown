#pragma once

/*

ConsoleEventHandler made by Emarioo/Dataolsson, Version 2.0 (2021.10.17)

What is it:
	This header features input event handling. This particular header is an event wrapper for windows console events.
	Checking input can be done by a listener or from just a function.

	Disclaimer, there may be bugs.

How to use:
	Begin by including this header into your main.cpp file and add #define DEFINE_HANDLER.

	The Init() should be called before other functions in this header. Preferable before the game loop.
	Init(GlfWwindow) needs a window from glfw to set the callbacks.

	After this use RefreshEvents() in the beginning of your game loop to update events.
	Use ResetInput() at the end of your game loop to reset certain events.

Other:
	You can use and edit this however you want but give credit where credit is due.

*/

#include <functional>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <iostream>

#include <windows.h>

namespace input
{
	/*
	Init usage of windows console.
	*/
	void Init();

	typedef char EventTypes;

	enum class EventType : char
	{
		NONE = 0,
		Click = 1,
		Move = 2,
		Scroll = 4,
		Key = 8,
		Resize = 16,
		Focus = 32,
	};

	inline EventType operator|(EventType a, EventType b);
	inline bool operator==(EventType a, EventType b);

	//extern EventTypes ClickEvent,MoveEvent,ScrollEvent,KeyEvent,ResizeEvent,FocusEvent;

	struct Keybinding
	{
		int keys[3];
	};
	class Event
	{
	public:
		Event() = default;
		Event(EventType types);

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
		Listener(EventType eventTypes, std::function<bool(Event&)> f);
		/*
		@priority determines if this listener should be called first when handling events. A high number is more important.
		This takes effect if the listener's function returns true which will stop other listeners.
		*/
		Listener(EventType eventTypes, int priority, std::function<bool(Event&)> f);
		std::function<bool(Event&)> run;
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

	//-- Dynamic event/input checking
	/*
	virtualKeyCode: Windows.h or glfw virtual key code depending on what you are using.
	return true if the key or mouse button is down/held.
	*/
	bool IsDown(int virtualKeyCode);
	/*
	virtualKeyCode: Windows.h or glfw virtual key code depending on what you are using.
	return true if the key was pressed this refresh/frame/update.
	*/
	bool IsPressed(int virtualKeyCode);
	/*
	return 0 if no scroll and 1 or -1 if scrolled.
	*/
	int IsScrolledY();
	int IsScrolledX();

	//-- Keybindings
	bool IsKeybindingDown(short id);
	void AddKeybinding(short id, int keyCode0, int keyCode1, int keyCode2);
	/*
	Returns number of keys loaded. Zero means that an empty keybindings file was loaded or that there is no file. Either way, default keybindings should be created.
	Less keybindings than there should be will most likely cause issues in the gameplay.
	@path is the path of the file. data/keybindings.dat for example. The format is irrelevant since it will use binary either way.
	*/
	int LoadKeybindings(const std::string& path);
	void SaveKeybindings(const std::string& path);
	void ClearKeybindings();
}