#include "ISystem.h"

namespace engine {
	ISystem::ISystem(const std::string& name) : name(name) {

	}
	void ISystem::MouseEvent(double mx, double my, int button, int action) {

	}
	void ISystem::HoverEvent(double mx, double my) {

	}
	void ISystem::KeyEvent(int key, int action) {

	}
	void ISystem::ScrollEvent(double yoffset) {

	}
	void ISystem::Update(float delta) {

	}
	void ISystem::Render() {

	}
}