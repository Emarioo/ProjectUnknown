#pragma once

#include <string>

namespace engine {
	class ISystem {
	public:
		/*
		Add a system to a list in interface manager.
		Functions will be run.
		*/
		ISystem(const std::string& name);
		virtual void MouseEvent(double mx, double my, int button, int action);
		virtual void HoverEvent(double mx, double my);
		virtual void KeyEvent(int key, int action);
		virtual void ScrollEvent(double yoffset);
		virtual void Update(float delta);
		virtual void Render();

		std::string name;
	};
}