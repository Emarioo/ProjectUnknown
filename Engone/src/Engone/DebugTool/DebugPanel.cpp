#include "gonpch.h"

#include "DebugPanel.h"

#include "Rendering/Renderer.h"

#include "GameState.h"

namespace engone {
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
				SetState(GameState::DebugMode,active);
			}
		}
		return false;
	}
	void DebugPanel::Update(float delta) {

	}
	void DebugPanel::Render() {
		
		renderer->BindTexture(0, "blank");
		renderer->DrawRect(x,y,w,h,0.5,0.2,0.8,1);
	}
}