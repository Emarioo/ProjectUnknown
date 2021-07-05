#pragma once

#include <string>
#include <vector>

#include "IConstraint.h"
#include "IColor.h"

namespace engine {
	class IBase {
	private:
		float aspectRatio = 0;// height/width
	public:
		IBase(const std::string& name);
		~IBase();

		std::string name;

		IColor color;
		void Color(const IColor& color);

		// They change quite frequently depending on transition and constraints
		float x=0, y=0, w=0, h=0;

		IConstraintX conX;
		IConstraintY conY;
		IConstraintW conW;
		IConstraintH conH;
		// Fix the aspect ratio of the IBase's width and height. 9/16 is normal aspect ratio.
		void SetFixed();
		// Update positioning and size by constraints
		void CalcConstraints();

		//std::vector<IComponent*> components;

		virtual void Update(float delta);
		virtual void Render();

		virtual bool ClickEvent(int mx, int my, int button, int action);
		virtual bool ScrollEvent(double scroll);
		virtual bool HoverEvent(int mx, int my);
		virtual bool KeyEvent(int key, int action);
	};
}