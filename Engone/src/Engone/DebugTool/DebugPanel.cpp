#include "gonpch.h"

#include "DebugPanel.h"

#include "Rendering/Renderer.h"

#include "GameState.h"

namespace engine {
	DebugPanel::DebugPanel(const std::string& name) 
	: IBase(name) {

	}
	bool DebugPanel::MouseEvent(int mx,int my,int action,int button) {

		return false;
	}
	bool DebugPanel::ScrollEvent(double scroll) {

		return false;
	}
	bool DebugPanel::KeyEvent(int key, int action) {
		if (action == 1) {
			if (key == GLFW_KEY_L) {
				active = !active;
				engine::SetState(GameState::DebugMode,active);
			}
		}
		return false;
	}
	void DebugPanel::Update(float delta) {

	}
	void DebugPanel::Render() {
		
		engine::BindTexture(0, "blank");
		engine::DrawRect(x,y,w,h,0.5,0.2,0.8,1);
	}
}