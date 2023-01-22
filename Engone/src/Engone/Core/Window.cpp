#include "Engone/Window.h"

#include "Engone/EventModule.h"

#include "Engone/Application.h"
#include "Engone/Utilities/Utilities.h"
#include "Engone/Engone.h"

namespace engone {
	TrackerId Window::trackerId="Window";
	static bool glfwIsActive = false;
	static double glfwFreezeTime = 0;
	void InitializeGLFW() {
		// There is this bug with glfw where it freezes. This will tell you if it has.
		std::thread tempThread = std::thread([] {
			for (int i = 0; i < 20; ++i) {
				std::this_thread::sleep_for((std::chrono::milliseconds)100);
				if (glfwIsActive) {
					return;
				}
			}
			std::cerr << "GLFW has frozen... ";
			glfwFreezeTime = engone::GetSystemTime()-20*0.1;// systemTime subtract the time the for loop took
		});

		if (!glfwInit()) {
			std::cout << "Glfw Init error!" << std::endl;
			return;
		}

		if (glfwFreezeTime != 0) {
			std::cout << "it took " << (GetSystemTime() - glfwFreezeTime) << " seconds\n";
		}
		glfwIsActive = true;
		tempThread.join();
	}
	static Window* activeWindow=nullptr;
	//static Window* mainWindow=nullptr;
	Window* GetActiveWindow() {
		return activeWindow;
	}
	float GetWidth() {
		return activeWindow->getWidth();
	}
	float GetHeight() {
		return activeWindow->getHeight();
	}
	static std::unordered_map<GLFWwindow*, Window*> windowMapping;
	Window* GetMappedWindow(GLFWwindow* window) {
		auto win = windowMapping.find(window);
		if (win != windowMapping.end())
			return win->second;
		return nullptr;
	}
	void CharCallback(GLFWwindow* window, uint32_t chr) {
		Window* win = GetMappedWindow(window);
		if (!win) {
			log::out << "Window isn't mapped\n";
			return;
		}
		if (win->m_charIn == win->m_charOut && !win->m_emptyCharArray) {
			log::out << log::RED << "CharCallback : m_charArray["<<Window::CHAR_ARRAY_SIZE<<"] of Window is full, skipping "<<chr<<" (increase size to prevent this).\n";
			return; // list is full
		}

		win->m_charArray[win->m_charIn] = chr;
		win->m_charIn = (win->m_charIn+1)% Window::CHAR_ARRAY_SIZE;
		win->m_emptyCharArray = false;
	}
	void DropCallback(GLFWwindow* window, int count, const char** paths) {
		Window* win = GetMappedWindow(window);
		if (!win) {
			log::out << "Window isn't mapped\n";
			return;
		}
		for (uint32_t i = 0; i < (uint32_t)count; ++i) {
			win->m_pathDrops.push_back(paths[i]);
		}
	}
	void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
		Window* win = GetMappedWindow(window);
		if (!win) {
			log::out << "Window isn't mapped\n";
			return;
		}
		Event e{ EventKey };
		e.key = key;
		e.scancode = scancode;
		e.action = action;
		e.window = win;
		win->addEvent(e);

		win->setInput(key, action != 0); // if not 0 means if pressed or repeating

		if (action != 0 && (key == GLFW_KEY_BACKSPACE || key == GLFW_KEY_DELETE || key == GLFW_KEY_ENTER || key == GLFW_KEY_LEFT || key == GLFW_KEY_RIGHT)) {
			CharCallback(window, key);
		}
		win->runListeners();
	}
	void MouseCallback(GLFWwindow* window, int button, int action, int mods) {
		Window* win = GetMappedWindow(window);
		if (!win) {
			log::out << "Window isn't mapped\n";
			return;
		}
		// Position needs updating
		double mx, my;
		glfwGetCursorPos(window, &mx, &my);
		win->setMouseX((float)mx);
		win->setMouseY((float)my);
		win->setInput(button, action != 0);

		//log::out << "click " << button << " " <<action<< "\n";

		Event e{ EventClick };
		e.button = button;
		e.action = action;
		e.mx = (float)mx;
		e.my = (float)my;
		e.window = win;
		win->addEvent(e);
		win->runListeners();
	}
	void CursorPosCallback(GLFWwindow* window, double mx, double my) {
		Window* win = GetMappedWindow(window);
		if (!win) {
			log::out << "Window isn't mapped\n";
			return;
		}
		win->setMouseX((float)mx);
		win->setMouseY((float)my);
		Event e{ EventMove };
		e.mx = (float)mx;
		e.my = (float)my;
		e.window = win;
		win->addEvent(e);
		win->runListeners();
	}
	void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
		Window* win = GetMappedWindow(window);
		if (!win) {
			log::out << "Window isn't mapped\n";
			return;
		}
		win->setScrollX((float)xoffset);
		win->setScrollY((float)yoffset);

		Event e{ EventScroll };
		e.scrollX = (float)xoffset;
		e.scrollY = (float)yoffset;
		e.mx = win->getMouseX();
		e.my = win->getMouseY();
		e.window = win;
		win->addEvent(e);
		win->runListeners();
	}
	static void FocusCallback(GLFWwindow* window, int focused) {
		Window* win = GetMappedWindow(window);
		if (win) {
			win->m_focus = focused;
		}
	}
	static void ResizeCallback(GLFWwindow* window, int width, int height) {
		Window* win = GetMappedWindow(window);
		if (!win){
			log::out << "Window isn't mapped\n";
			return;
		}

		Event e{ EventResize };
		e.width = (float)width;
		e.height = (float)height;
		e.window = win;
		win->addEvent(e);
		win->runListeners();

		win->w = (float)width;
		win->h = (float)height;
	}
	static void CloseCallback(GLFWwindow* window) {
		Window* win = GetMappedWindow(window);
		if (win) {
			if (win->m_parent) {
				win->m_parent->onClose(win);
			}
			//if (mainWindow == win)
			//	mainWindow = nullptr;
		}
	}
	static void PosCallback(GLFWwindow* window, int x, int y) {
		Window* win = GetMappedWindow(window);
		if (win) {
			win->x = (float)x;
			win->y = (float)y;
		}
	}
	static float cameraSensitivity = 0.2f;
	EventType FirstPerson(Event& e) {
		if (e.window->m_lastMouseX != -1) {
			CommonRenderer* renderer = (CommonRenderer*)e.window->getPipeline()->getComponent("common_renderer");
			Camera* camera = nullptr;
			if (renderer)
				camera = renderer->getCamera();
			if (e.window->isCursorLocked() && camera != nullptr) {
				float rawX = -(e.mx - e.window->m_lastMouseX) * (glm::pi<float>() / 360.0f) * cameraSensitivity;
				float rawY = -(e.my - e.window->m_lastMouseY) * (glm::pi<float>() / 360.0f) * cameraSensitivity;

				e.window->m_tickRawMouseX += rawX;
				e.window->m_frameRawMouseX += rawX;
				e.window->m_tickRawMouseY += rawY;
				e.window->m_frameRawMouseY += rawY;

				if (e.window->m_enabledFirstPerson) {
					glm::vec3 rot = camera->getRotation(); // get a copy of rotation
					rot.y += rawX;
					rot.x += rawY;
					// clamp up and down directions.
					if (rot.x > glm::pi<float>() / 2) {
						rot.x = glm::pi<float>() / 2;
					}
					if (rot.x < -glm::pi<float>() / 2) {
						rot.x = -glm::pi<float>() / 2;
					}
					rot.x = remainder(rot.x, glm::pi<float>() * 2);
					rot.y = remainder(rot.y, glm::pi<float>() * 2);
					camera->setRotation(rot); // set the new rotation
				}
			}
		}
		e.window->m_lastMouseX = e.mx;
		e.window->m_lastMouseY = e.my;
		return EventNone;
	}

	Window::Window(Application* application, WindowDetail detail) {
		// setup
		if(!glfwIsActive) InitializeGLFW();
		
		// Minor setup stuff
		m_parent = application;
		//m_renderer.m_parent = this;

		// Window related setup
		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* vidmode = glfwGetVideoMode(monitor);
		
		if (detail.x == -1) x = (float)vidmode->width / 6.0f;
		else x = (float)detail.x;
		if (detail.y == -1) y = (float)vidmode->height / 6.0f;
		else y = (float)detail.y;
		if (detail.w == -1) w = (float)vidmode->width / 1.5f;
		else w = (float)detail.w;
		if (detail.h == -1) h = (float)vidmode->height / 1.5f;
		else h = (float)detail.h;

		m_firstPersonListener = ALLOC_NEW(Listener)(EventMove, 9998, FirstPerson);
		attachListener(m_firstPersonListener);

		setMode(detail.mode,true);
		windowMapping[m_glfwWindow] = this;
		
		CommonRenderer* commonRenderer = ALLOC_NEW(CommonRenderer)();
		m_renderPipeline.addComponent(commonRenderer,true);
		UIRenderer* uiRenderer = ALLOC_NEW(UIRenderer)();
		m_renderPipeline.addComponent(uiRenderer,true);

		setActiveContext();
	}
	Window::~Window() {
		// we need to set window as active context before deleting buffers.
		setActiveContext();
		windowMapping.erase(m_glfwWindow);
		if (activeWindow == this) {
			activeWindow = nullptr;
		}
		// destroy execution context
		
		// deleting context will delete buffers to. If you are sharing any unexepected things might happen
		glfwDestroyWindow(m_glfwWindow);

		for (Listener* l : m_listeners) {
			if (l->m_ownedByWindow) {
				GetTracker().untrack(l);
				//delete l;
				ALLOC_DELETE(Listener, l);
			}
		}
		ALLOC_DELETE(Listener, m_firstPersonListener);
		//delete m_firstPersonListener;
		m_firstPersonListener = nullptr;
		//log::out << "deleted window\n";

		// then we set it to nullptr
		activeWindow = nullptr;
	}
	void Window::setTitle(const std::string title) {
		m_title = title;
		glfwSetWindowTitle(m_glfwWindow, title.c_str());
	}
	void Window::setIcon(RawImage* img) {
		if (!img) {
			log::out << log::RED << "Window::setIcon - img is nullptr\n";
			return;
		}
		GLFWimage tmp = {img->width,img->height,(uint8_t*)img->data()};
		glfwSetWindowIcon(m_glfwWindow,1,&tmp);
	}
	void Window::setActiveContext() {
		//if(m_renderer)
		UIRenderer* uiRenderer = (UIRenderer*)getPipeline()->getComponent("ui_renderer");
		if(uiRenderer)
			uiRenderer->setActiveUIRenderer();
		CommonRenderer* commonRenderer = (CommonRenderer*)getPipeline()->getComponent("common_renderer");
		if(commonRenderer)
			commonRenderer->setActiveRenderer();
		m_renderPipeline.setActivePipeline();
		// no need to do this if window already is active
		if (activeWindow != this) {
			glfwMakeContextCurrent(m_glfwWindow);
			if (!initializedGLEW) {
				initializedGLEW = true;
				GLenum err = glewInit();
				if (err != GLEW_OK) {
					std::cout << "GLEW Error: " << glewGetErrorString(err) << std::endl;
					return;
				}
			}
		}
		activeWindow = this;
		int wid;
		int hei;
		glfwGetWindowSize(m_glfwWindow, &wid, &hei);

		glViewport(0, 0, wid, hei);

		if (commonRenderer)
			commonRenderer->init();
		//m_renderer.init();
		// 
		//if (mainWindow == nullptr)
			//mainWindow = this;
	}
	void Window::setPosition(float x, float y) {
		glfwSetWindowPos(m_glfwWindow, (int)x, (int)y);
		PosCallback(m_glfwWindow, (int)x, (int)y);
	}
	void Window::setInput(int code, bool down) {
		for (uint32_t i = 0; i < m_inputs.size(); ++i) {
			if (m_inputs[i].code == code) {
				if (down) {
					if (!m_inputs[i].down) {
						m_inputs[i].down = true;
						m_inputs[i].tickPressed++;
						m_inputs[i].framePressed++;
						return;
					}
				} else {
					m_inputs[i].down = false;
					m_inputs[i].tickReleased++;
					m_inputs[i].frameReleased++;
				}
				break;
			}
		}
		// HEY YOU! don't forget to change these when messing about.
		if (down)
			m_inputs.push_back({ code, down, 1, 1, 0, 0 });
		else
			m_inputs.push_back({ code, down, 0, 0, 1, 1 });
	}
	float Window::getRawMouseX() const {
		if (m_parent->isRenderingWindow()) {
			return m_frameRawMouseX;
		} else {
			return m_tickRawMouseX;
		}
	}
	float Window::getRawMouseY() const {
		if (m_parent->isRenderingWindow()) {
			return m_frameRawMouseY;
		} else {
			return m_tickRawMouseY;
		}
	}
	float Window::getScrollX() const {
		if (m_parent->isRenderingWindow()) {
			return m_frameScrollX;
		} else {
			return m_tickScrollX;
		}
	}
	float Window::getScrollY() const {
		if (m_parent->isRenderingWindow()) {
			return m_frameScrollY;
		} else {
			return m_tickScrollY;
		}
	}
	ExecutionControl& Window::getControl() {
		return renderControl;
	}
	ExecutionTimer& Window::getExecTimer() {
		return executionTimer;
	}
	void Window::setFPS(double fps) {
		if (fps < 1)
			fps = 1;
		if (getParent()->isMultiThreaded()) {
			executionTimer.aimedDelta = 1. / fps;
		} else
			getParent()->getEngine()->mainRenderTimer.aimedDelta = 1. / fps;
	}
	double Window::getFPS() {
		if (getParent()->isMultiThreaded())
			return 1./executionTimer.aimedDelta;

		return 1. / getParent()->getEngine()->mainRenderTimer.aimedDelta;
	}
	double Window::getRealFPS() {
		return getParent()->getProfiler().getSampler(this).getAverage();
		//if (getParent()->isMultiThreaded())
		//	return 1. / executionTimer.delta;

		//return 1. / getParent()->getEngine()->mainUpdateTimer.delta;
	}
	void Window::attachListener(Listener* listener) {
		// Prevent duplicates
		for (uint32_t i = 0; i < m_listeners.size(); ++i) {
			if (listener == m_listeners[i]) {
				return;
			}
		}
		// Priorities
		for (uint32_t i = 0; i < m_listeners.size(); ++i) {
			if (listener->priority > m_listeners[i]->priority) {
				m_listeners.insert(m_listeners.begin() + i, listener);
				return;
			}
		}
		m_listeners.push_back(listener);
	}
	std::string Window::pollPathDrop() {
		if (m_pathDrops.size() == 0) return "";
		std::string path = m_pathDrops.back();
		m_pathDrops.pop_back();
		return path;
	}
	uint32_t Window::pollChar() {
		if (m_charIn == m_charOut)
			return 0;
		
		uint32_t chr = m_charArray[m_charOut];
		m_charOut = (m_charOut + 1) % CHAR_ARRAY_SIZE;
		if (m_charIn == m_charOut)
			m_emptyCharArray = true;
		return chr;
	}
	bool Window::isKeyDown(int code) {
		for (uint32_t i = 0; i < m_inputs.size(); ++i) {
			if (m_inputs[i].code == code)
				return m_inputs[i].down;
		}
		return false;
	}
	bool Window::isKeyPressed(int code) {
		for (uint32_t i = 0; i < m_inputs.size(); ++i) {
			if (m_inputs[i].code == code) {
				if (m_parent->isRenderingWindow()) {
					if (m_inputs[i].framePressed > 0) {
						return true;
					}
				} else {
					if (m_inputs[i].tickPressed > 0) {
						return true;
					}
				}
				return false;
			}
		}
		return false;
	}
	void Window::resetKey(int code) {
		for (uint32_t i = 0; i < m_inputs.size(); ++i) {
			if (m_inputs[i].code == code) {
				if (m_parent->isRenderingWindow()) {
					m_inputs[i].framePressed = 0;
				} else {
					m_inputs[i].tickPressed = 0;
				}
				break;
			}
		}
	}
	bool Window::isKeyReleased(int code) {
		for (uint32_t i = 0; i < m_inputs.size(); ++i) {
			if (m_inputs[i].code == code) {
				if (m_parent->isRenderingWindow()) {
					if (m_inputs[i].frameReleased > 0) {
						return true;
					}
				}
				else {
					if (m_inputs[i].tickReleased > 0) {
						return true;
					}
				}
				return false;
			}
		}
		return false;
	}
	void Window::resetEvents(bool resetFrameInput) {
		if (m_parent != nullptr) {
			if (resetFrameInput) {
				m_frameScrollX = 0;
				m_frameScrollY = 0;
				m_frameRawMouseX = 0;
				m_frameRawMouseY = 0;
				for (uint32_t i = 0; i < m_inputs.size(); ++i) {
					m_inputs[i].framePressed = 0;
					m_inputs[i].frameReleased = 0;
				}
			} else {
				m_tickScrollX = 0;
				m_tickScrollY = 0;
				m_tickRawMouseX = 0;
				m_tickRawMouseY = 0;
				for (uint32_t i = 0; i < m_inputs.size(); ++i) {
					m_inputs[i].tickPressed = 0;
					m_inputs[i].tickReleased = 0;
				}
			}
		}
	}
	void Window::runListeners() {
		for (uint32_t j = 0; j < m_events.size(); ++j) {
			EventTypes breaker = EventNone; // if a flag is simular it will break
			for (uint32_t i = 0; i < m_listeners.size(); ++i) {
				if (m_listeners[i]->eventTypes & breaker)// continue if an event has been checked/used/disabled
					continue;

				if (m_listeners[i]->eventTypes & m_events[j].eventType) {
					EventTypes types = m_listeners[i]->run(m_events[j]);
					if (types != EventNone)
						breaker = (breaker | m_listeners[i]->eventTypes);
				}
			}
		}
		m_events.clear();
	}
	void Window::enableFirstPerson(bool yes) {
		m_enabledFirstPerson = yes;
	}
	static void SetCallbacks(GLFWwindow* window) {
		glfwSetKeyCallback(window, KeyCallback);
		glfwSetMouseButtonCallback(window, MouseCallback);
		glfwSetCursorPosCallback(window, CursorPosCallback);
		glfwSetScrollCallback(window, ScrollCallback);
		glfwSetWindowFocusCallback(window, FocusCallback);
		glfwSetWindowSizeCallback(window, ResizeCallback);
		glfwSetWindowCloseCallback(window, CloseCallback);
		glfwSetWindowPosCallback(window, PosCallback);
		glfwSetCharCallback(window, CharCallback);
		glfwSetDropCallback(window, DropCallback);
	}
	void Window::setMode(WindowModes winMode,bool force) {
		if (m_windowMode == winMode&&!force) return;
		 
		// last monitor this window or screen was in
		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // this was 3 before
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		if (m_glfwWindow) {
			GLFWmonitor* mayUseMonitor = NULL;
			if (winMode & ModeBorderless) {
				glfwSetWindowAttrib(m_glfwWindow, GLFW_DECORATED, true);
			} else {
				glfwSetWindowAttrib(m_glfwWindow, GLFW_DECORATED, false);
				mayUseMonitor = monitor;
			}
			if ((winMode & ModeTransparent)!=(m_windowMode&ModeTransparent)) {
				log::out << "Window::setMode - window cannot toggle transparent framebuffers\n";
			}
			if (winMode & ModeFullscreen) {
				if((m_windowMode&ModeFullscreen)==0)
					saveCoords();
				glfwSetWindowMonitor(m_glfwWindow, mayUseMonitor, 0, 0, mode->width, mode->height, mode->refreshRate);
				PosCallback(m_glfwWindow, (int)0, (int)0);
				ResizeCallback(m_glfwWindow, (int)mode->width, (int)mode->height);
			} else {
				if (m_windowMode & ModeFullscreen)
					loadCoords();
				glfwSetWindowMonitor(m_glfwWindow, NULL, (int)x, (int)y, (int)w, (int)h, mode->refreshRate);
			}
		} else {
			//glfwWindowHint(GLFW_FOCUS_ON_SHOW, true);
			GLFWwindow* sharing = NULL;
			GLFWmonitor* mayUseMonitor = NULL;
			if (winMode & ModeBorderless) {
				glfwWindowHint(GLFW_DECORATED, false);
			}
			else {
				glfwWindowHint(GLFW_DECORATED, true);
				mayUseMonitor = monitor;
			}
			if (winMode & ModeTransparent) {
				glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, true);
			} else {
				glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, false);
			}
			if (winMode & ModeTransparent) {
				glfwWindowHint(GLFW_MOUSE_PASSTHROUGH, true);
			} else {
				glfwWindowHint(GLFW_MOUSE_PASSTHROUGH, false);
			}
			glfwWindowHint(GLFW_FOCUSED,false);
			glfwWindowHint(GLFW_FOCUS_ON_SHOW,false);
			if ((winMode & ModeFullscreen)==0) {
				if (m_windowMode & ModeFullscreen)
					loadCoords();
				m_glfwWindow = glfwCreateWindow((int)w, (int)h, m_title.c_str(), NULL, NULL);
				glfwSetWindowPos(m_glfwWindow, (int)x, (int)y);
			} else {
				if ((m_windowMode & ModeFullscreen) == 0)
					saveCoords();
				m_glfwWindow = glfwCreateWindow(mode->width, mode->height, m_title.c_str(), mayUseMonitor, NULL);
				//m_glfwWindow = glfwCreateWindow(mode->width, mode->height, m_title.c_str(), NULL, NULL);
				x = 0;
				y = 0;
				w = mode->width;
				h = mode->height;
			}
			SetCallbacks(m_glfwWindow);
			ZeroMemory(m_charArray, CHAR_ARRAY_SIZE);
		}

		m_focus = true;

		m_windowMode = winMode;
	}
	void Window::close() {
		// handle invalid glfwWindow?
		if (!glfwWindowShouldClose(m_glfwWindow)) {
			glfwSetWindowShouldClose(m_glfwWindow, true);
			CloseCallback(m_glfwWindow);
		}
	}
	void Window::setCursorVisible(bool visible) {
		m_cursorVisible = visible;
		if (visible) glfwSetInputMode(m_glfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		else glfwSetInputMode(m_glfwWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	}
	void Window::lockCursor(bool locked) {
		m_cursorLocked = locked;
		if (locked) {
			glfwSetInputMode(m_glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			if (glfwRawMouseMotionSupported())
				glfwSetInputMode(m_glfwWindow, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
		} else {
			setCursorVisible(isCursorVisible());
			if (glfwRawMouseMotionSupported())
				glfwSetInputMode(m_glfwWindow, GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);
		}
	}
	bool Window::isOpen() {
		return !glfwWindowShouldClose(m_glfwWindow);
	}
}