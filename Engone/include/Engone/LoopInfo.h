#pragma once

namespace engone {
	class Application;
	class Window;
	// timeStep, app, window, interpolation
	struct LoopInfo {
		float timeStep;
		Application* app;
		Window* window;
		float interpolation;
	};
}