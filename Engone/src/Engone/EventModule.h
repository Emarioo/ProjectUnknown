#pragma once

#include "Rendering/Renderer.h"

namespace engone {

	/*
	Init usage of glfw events. Setup glfw callbacks
	*/
	void InitEvents(GLFWwindow* window);

	enum class EventType : char {
		None = 0,
		Click = 1,
		Move = 2,
		Scroll = 4,
		Key = 8,
		Resize = 16,
		Focus = 32
	};
	EventType operator|(EventType a, EventType b);
	bool operator==(EventType a, EventType b);
	struct Keybinding {
		int keys[3];
	};
	struct Event {
		EventType eventType=EventType::None;
		// initialization not needed but i don't want errors so... yeah
		int action=0, button=0;
		int key=0, scancode=0;
		float mx=0, my=0, scrollX=0, scrollY=0;
		int width=0, height=0, focused=0;
	};
	/*
	Listener for glfw. Can listen to multiple events.
	*/
	class Listener {
	public:
		Listener(EventType eventTypes, std::function<EventType(Event&)> f);
		/*
		@eventtypes events which will be listened to
		@priority a higher number will call this listener before others
		@f returning an eventtype will stop listeners with lower @priority from listening to that/those events
		*/
		Listener(EventType eventTypes, int priority, std::function<EventType(Event&)> f);
		std::function<EventType(Event&)> run;
		int priority = 0;
		EventType eventTypes;
	};

	void AddListener(Listener* listener);

	// Called absolute last of update loop, always necessary
	void ResetEvents();

	std::string PollClipboard();
	void SetClipboard(const char* str);
	std::string PollPathDrop();
	uint32_t PollChar();

	float GetMouseX();
	float GetMouseY();
	/*
	return 0 if no scroll and 1 or -1 if scrolled.
	*/
	float IsScrolledY();
	/*
	return 0 if no scroll and 1 or -1 if scrolled.
	*/
	float IsScrolledX();

	/*
	virtualKeyCode: Window or glfw virtual key code depending on what you are using.
	return true if the key or mouse button is down.
	*/
	bool IsKeyDown(int virtualKeyCode);
	/*
	virtualKeyCode: Windows.h or glfw virtual key code depending on what you are using.
	return true if the key was pressed this refresh/frame/update.
	*/
	bool IsKeyPressed(int virtualKeyCode);

	bool IsKeybindingDown(uint16_t id);
	bool IsKeybindingPressed(uint16_t id);
	/*
	When using IsKeybindingDown: All of the specified keycodes needs to be down.
	When using IsKeybindingPressed: The last two keycodes needs to be down before pressing the first one.
	The last two keycodes are optional
	*/
	void AddKeybinding(uint16_t id, int keyCode0, int keyCode1 = -1, int keyCode2 = -1);
	/*
	Returns number of keys loaded. You should create default keybindings if zero is returned.
	Less keybindings than there should be will most likely cause issues in gameplay.
	@path is the path of the file. data/keybindings.dat for example. The .dat is optional.
	*/
	int LoadKeybindings(const std::string& path);
	/*
	File could not be saved if false is returned. Directory might not exist.
	*/
	bool SaveKeybindings(const std::string& path);
	void ClearKeybindings();
}