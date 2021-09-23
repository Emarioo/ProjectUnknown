#pragma once

namespace engone {

	enum class EventType
	{
		NONE=0,
		Mouse,
		Drag,
		Key,
		Scroll,
		Resize,
		Focus,
	};
	class Event
	{
	public:
		Event(EventType t, int d0)
			: eventType(t), values{ d0 } {}
		Event(EventType t, int d0, int d1)
			: eventType(t), values{d0, d1} {}
		Event(EventType t, int d0, int d1, int d2, int d3)
			: eventType(t), values{ d0, d1, d2, d3 } {}
		int GetX() {
			return values[0];
		}
		int GetY() {
			return values[1];
		}
		int GetAction() {
			return values[2];
		}
		int GetButton() {
			return values[3];
		}
		int GetScroll() {
			return values[0];
		}
		int GetFocus() {
			return values[0];
		}
	private:
		EventType eventType;
		int values[4];
	};
	class EventListener
	{
	public:
		EventListener();

		virtual void run(Event* e) = 0;
	private:
		EventType eventType;
	};
	class EventHandler
	{
	public:
		EventHandler() = default;
		/*
		Set event callbacks from glfw
		*/
		void Init();

		void AddListener(EventListener* dispatcher);
		void AddEvent(Event* e);
		void DispatchEvents();

		int GetMouseX();
		int GetMouseY();
		void SetMouseX(int x);
		void SetMouseY(int y);

	private:
		std::vector<EventListener*> listeners;
		std::vector<Event*> events;

		int mouseX,mouseY;
	};
	extern EventHandler* eventHandler;
}