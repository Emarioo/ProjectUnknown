#pragma once

#include "IConstraint.h"
#include "IColor.h"

namespace engone {
	class IBase {
	private:
		float aspectRatio = 0;// height/width
	public:
		IBase(const std::string& name);
		IBase(const std::string& name,int priority);
		~IBase();

		bool active;
		bool IsActive();

		std::string name;
		int priority=0;
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

		virtual void Update(float delta) = 0;
		/*
		run 'CalcConstraints' and run 'Render' if 'active'
		*/
		void PreRender();
		virtual void Render() = 0;

		virtual bool MouseEvent(int mx, int my, int action, int button);
		virtual bool ScrollEvent(double scroll);
		virtual bool HoverEvent(int mx, int my);
		virtual bool KeyEvent(int key, int action);
	};
}