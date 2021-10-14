#pragma once

#include <functional>

namespace engone {

	enum class EventType : char
	{
		NONE=0,
		Click=1,
		Move=2,
		Scroll=4,
		Key=8,
		Resize=16,
		Focus=32,
	};
	class Event
	{
	public:
		Event(EventType t)
			: eventType(t) {}
	
		EventType eventType;

		union
		{
			struct
			{
				int action, key, scancode;
			};
			struct
			{
				int action, mx, my, button, scroll;
			};
			struct
			{
				// New size of window
				int rw, rh;
				bool focused;
			};
		};
	};
	/*
	Can not use multiple events that uses the same member variables in the union.
	Mouse click, move and scroll is fine for example but not mouse and key events.
	*/
	class EventListener
	{
	public:
		EventListener(EventType type, std::function<bool(Event&)> f);
		std::function<bool(Event&)> run;
		EventType eventType;
	};

	/*
	Set event callbacks from glfw. Currently done at the end of MakeWindow() in renderer
	*/
	void InitEvents();

	void AddListener(EventListener* listener);
	void AddEvent(Event e);
	void DispatchEvents();

	int GetMouseX();
	int GetMouseY();

	bool IsKeyDown(int key);
	bool IsActionDown(unsigned short action);
	void AddActionKey(unsigned short keyName, int keyCode);
	/*
	Returns number of keys loaded. Zero means that an empty keybindings file was loaded or that there is no file. Either way, default keybindings should be created.
	Less keys than there should be will most likely cause issues in the gameplay.
	*/
	int LoadKeybindings(const std::string& path);
	void SaveKeybindings(const std::string& path);
	void ClearKeybindings();
}