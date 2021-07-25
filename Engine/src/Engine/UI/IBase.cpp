#include "IBase.h"

//#include "../Utility/DebugHandler.h"
#include "../Rendering/Renderer.h"
namespace engine {
	IBase::IBase(const std::string& name)
	: name(name),
		conX(IConstraintX(this)), conY(IConstraintY(this)),
		conW(IConstraintW(this)), conH(IConstraintH(this)) {}
	IBase::IBase(const std::string& name,int priority)
		: name(name),priority(priority),
		conX(IConstraintX(this)), conY(IConstraintY(this)),
		conW(IConstraintW(this)), conH(IConstraintH(this)) {}
	IBase::~IBase() {}

	bool IBase::IsActive() {
		return active;
	}
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
	void IBase::PreRender(){
		CalcConstraints();
	}
	bool IBase::MouseEvent(int mx, int my, int action, int button) { return false; }
	bool IBase::ScrollEvent(double scroll) { return false; }
	bool IBase::HoverEvent(int mx, int my) { return false; }
	bool IBase::KeyEvent(int key, int action) { return false; }

}