#pragma once

#include "../UI/IBase.h"

namespace engine {
	class DebugPanel : public engine::IBase {
	public:
		DebugPanel(const std::string& name);

		bool MouseEvent(int mx, int my, int action, int button) override;
		bool ScrollEvent(double scroll) override;
		bool KeyEvent(int key, int action) override;

		void Update(float delta) override;
		void Render() override;
	};
}