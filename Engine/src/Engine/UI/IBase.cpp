#include "IBase.h"

#include "Utility/Debugger.h"
#include "Rendering/Renderer.h"
namespace engine {
	IBase::IBase(const std::string& name)
	: name(name), x(0), y(0), w(0), h(0),
		conX(IConstraintX(this)), conY(IConstraintY(this)),
		conW(IConstraintW(this)), conH(IConstraintH(this)) {}
	IBase::~IBase() {}

	void IBase::Color(const IColor& c) {
		color = c;
	}
	void IBase::SetFixed() {
		aspectRatio = 9/16.f;
	}
	void IBase::CalcConstraints() {
		w = conW.Value();
		h = conH.Value();
		if (aspectRatio!=0) {
			float newRatio = Height() / Width();
			if (newRatio > aspectRatio) {
				// Height is bigger or width is smaller
				h *= aspectRatio / newRatio;
				if (conW.isPixel) w *= aspectRatio / newRatio;
			}
			else {
				// Width bigger or height smaller
				w *= newRatio / aspectRatio;
				if (conW.isPixel) h *= newRatio / aspectRatio;
			}
		}
		x = conX.Value();
		y = conY.Value();
	}
	void IBase::Update(float delta) {}
	void IBase::Render() {
	}
	bool IBase::ClickEvent(int mx, int my, int button, int action) { return false; }
	bool IBase::ScrollEvent(double scroll) { return false; }
	bool IBase::HoverEvent(int mx, int my) { return false; }
	bool IBase::KeyEvent(int key, int action) { return false; }
}