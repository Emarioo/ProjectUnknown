#pragma once

namespace engone {
	class Application;
	class Window;
	struct UpdateInfo {
		float timeStep;
		Application* app;
	};
	struct RenderInfo {
		float interpolation;
		Window* window;
	};
}