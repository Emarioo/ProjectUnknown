#include "IBase.h"

#include "Utility/Debugger.h"
#include "Rendering/Renderer.h"
namespace engine {
	IBase::IBase(const std::string& name)
	: name(name), x(0), y(0), w(0), h(0),
		conX(IConstraint(this, true)), conY(IConstraint(this, false)),
		conW(IConstraint(this, true)), conH(IConstraint(this, false)) {}
	IBase::~IBase() {}

	void IBase::Color(const IColor& c) {
		color = c;
	}
	void IBase::SetFixed() {
		aspectRatio = 9/16.f;
	}
	void IBase::CalcConstraints() {
		w = conW.Value(0);
		h = conH.Value(0);
		if (aspectRatio!=0) {
			float newRatio = Height() / Width();
			if (newRatio > aspectRatio) {
				// Height is bigger or width is smaller
				h *= aspectRatio / newRatio;
			}
			else {
				// Width bigger or height smaller
				w *= newRatio / aspectRatio;
			}
		}
		x = conX.Value(w);
		y = conY.Value(h);
	}
	void IBase::Update(float delta) {}
	void IBase::Render() {
	}
	bool IBase::ClickEvent(int mx, int my, int button, int action) { return false; }
	bool IBase::ScrollEvent(double scroll) { return false; }
	bool IBase::HoverEvent(int mx, int my) { return false; }
	bool IBase::KeyEvent(int key, int action) { return false; }
}