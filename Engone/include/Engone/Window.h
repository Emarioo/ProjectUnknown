#pragma once

#include "Engone/RenderModule.h"
#include "Engone/EventModule.h"

namespace engone {

	enum WindowMode : uint8_t {
		WindowedMode=0,
		FullscreenMode,
		BorderlessMode,
		BorderlessFullscreenMode,
	};
	class Application;
	struct WindowDetail {
		WindowMode mode = WindowedMode;
		int w = -1, h = -1;
		int x = -1, y = -1;
	};
	// Window can be seen as a reference to a window on your computer.
	class Window {
	public:

		inline GLFWwindow* glfw() const { return m_glfwWindow; }

		void setTitle(const std::string title);
		// will make this window/context the active one.
		// will not set window as active if it already is.
		void setActiveContext();

		void setMode(WindowMode mode);
		// May send a EventMove to listeners since the mouse position is relative to window position.
		void setPosition(float x,float y);

		inline float getX() const {return x; }
		inline float getY() const {return y; }
		// Inner width of window
		inline float getWidth() const { return w; }
		// Inner height of window
		inline float getHeight() const { return h; }

		inline WindowMode getMode() const { return m_windowMode; }

		inline bool hasFocus() const { return m_focus; }
		// True if cursor is visible.
		inline bool isCursorVisible() const { return m_cursorVisible; }

		inline Application* getParent() const { return m_parent; }
		inline Assets* getAssets() { return &m_assets; }
		inline Renderer* getRenderer() { return &m_renderer; }

		void attachListener(Listener* listener);
		void addEvent(Event& e) { m_events.push_back(e); }
		void runListeners();

		float getMouseX() const { return m_mouseX; }
		float getMouseY() const { return m_mouseY; }
		void setMouseX(float x) { m_mouseX=x; }
		void setMouseY(float y) { m_mouseY=y; }
		float getScrollX() const { return m_scrollX; }
		float getScrollY() const { return m_scrollY; }
		void setScrollX(float x) { m_scrollX = x; }
		void setScrollY(float y) { m_scrollY = y; }

		void setInput(int code, bool down);

		std::string pollPathDrop();
		uint32_t pollChar();

		bool isKeyDown(int code);
		bool isKeyPressed(int code);
		void resetEvents();

		void enableFirstPerson();

		void close();
		// If true, the cursor will be made visible.
		void setCursorVisible(bool visible);
		// Whether the cursor is locked to the window.
		inline bool isCursorLocked() const { return m_cursorLocked;}
		// If true, the cursor will be made invisible and locked to the window. Use this when you want the player to lock around.
		void lockCursor(bool locked);
		// true if window isn't closed
		bool isActive();

	private:
		Window(Application* application, WindowDetail detail);
		// Deleting window will delete renderer's buffers.
		// if this window shares buffers with another unexepected things might happen.
		~Window();

		GLFWwindow* m_glfwWindow=nullptr;
		Application* m_parent=nullptr;
		Assets m_assets;
		Renderer m_renderer;
		std::vector<Listener*> m_listeners;
		std::vector<Event> m_events;
		std::vector<EventInput> m_inputs;
		std::vector<std::string> m_pathDrops;

		uint32_t m_charCount = 0;
		uint32_t m_readIndex = 0;
		static const uint32_t CHAR_ARRAY_SIZE = 20;
		uint32_t m_charArray[CHAR_ARRAY_SIZE];

		float m_mouseX, m_mouseY;
		float m_scrollX, m_scrollY;

		WindowMode m_windowMode = WindowedMode;
		bool m_cursorVisible = true, m_cursorLocked = false, m_focus = true;
		std::string m_title = "Untitled";
		float x = -1, y = -1, w = -1, h = -1;

		friend class Application;
		friend class Engone;

		friend void FocusCallback(GLFWwindow* window, int focused);
		friend void CloseCallback(GLFWwindow* window);
		friend void ResizeCallback(GLFWwindow* window, int width, int height);
		friend void PosCallback(GLFWwindow* window, int x, int y);
		friend void CharCallback(GLFWwindow* window, uint32_t chr);
		friend void DropCallback(GLFWwindow* window, int count, const char** paths);

	};

	void InitializeGLFW();
	Window* GetMappedWindow(GLFWwindow* window);

	Window* GetActiveWindow();
	// Inner width of active window
	float GetWidth();
	// Inner height of active window
	float GetHeight();
}