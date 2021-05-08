#pragma once

#include "Text.h"
#include <functional>
#include <vector>

namespace engine {
	class IElement;
	class Color {
	public:
		float r,g,b,a;
		Color();
		Color(float gray);
		Color(int gray);
		Color(float gray, float alpha);
		Color(int gray, float alpha);
		Color(float red,float green,float blue);
		Color(int red, int green, int blue);
		Color(float red,float green,float blue,float alpha);
		Color(int red, int green, int blue, float alpha);
	};
	class IConstraint {
	public:
		IConstraint(IElement* e, bool use_xAxis);
		IElement* Center(int pos);
		IElement* Center(float pos);
		IElement* Center(IElement* element, int pos);
		IElement* Center(IElement* element, float pos);
		IElement* Left(int pos);
		IElement* Left(float pos);
		IElement* Left(IElement* element, int pos);
		IElement* Left(IElement* element, float pos);
		IElement* Right(int pos);
		IElement* Right(float pos);
		IElement* Right(IElement* element, int pos);
		IElement* Right(IElement* element, float pos);
		IElement* Bottom(int pos);
		IElement* Bottom(float pos);
		IElement* Bottom(IElement* element, int pos);
		IElement* Bottom(IElement* element, float pos);
		IElement* Top(int pos);
		IElement* Top(float pos);
		IElement* Top(IElement* element, int pos);
		IElement* Top(IElement* element, float pos);
		void Add(int pos);
		void Add(float pos);
		float Value(float w);
	private:
		IElement* parent;

		bool center = false;
		bool xAxis = false;
		bool side = false;
		IElement* elem = nullptr;

		bool isPixel = false;
		float raw = 0;
	};
	/*
	Add Functionality for timed functions?
	*/
	class ITransition {
	private:
		// std::function<void(int)> func = nullptr;
		// float funcS = 0, funcT = 0;
		float fadeS = 0, moveS = 0, sizeS = 0;
		bool* activator = nullptr;
		std::function<bool()> activatorF = nullptr;
		bool movePixel = false, sizePixel = false;
		float x = 0, y = 0, w = 0, h = 0;
	public:
		bool fade = false, move = false, size = false;
		//float r = 1, g = 1, b = 1, a = 1;
		Color color;
		float fadeT = 0, moveT = 0, sizeT = 0;

		ITransition(bool* act);
		ITransition(std::function<bool()> act);

		ITransition* Fade(const Color& color, float time);
		ITransition* Move(int x, int y, float time);
		ITransition* Move(float x, float y, float time);
		ITransition* Size(int w, int h, float time);
		ITransition* Size(float w, float h, float time);

		float ValueX();
		float ValueY();
		float ValueW();
		float ValueH();

		// Fade is calculated in element.cpp since it requires some more hardcore calculations
		// Might be possible to make a functions out of it but ehh, whatever

		void Update(float delta);
	};
	class IElement {
	public:
		IElement(const std::string& name, int priority);
		~IElement();

		// Final values
		// They get updated as things change
		float x = 0, y = 0, w = 0, h = 0;
		//float r = 1, g = 1, b = 1, a = 1;
		Color color;

		// What is this?
		bool isBlank = false;

		bool isEditable = false;

		int priority = 0;

		std::string name;

		IConstraint conX;
		IConstraint conY;
		IConstraint conW;
		IConstraint conH;

		std::string texture = "blank";

		Text text;

		void Texture(const std::string& texture);
		void Col(const Color& color);
		void Blank();
		/*
		Fit text in this element
		*/
		void Text(Font* f, const std::string& s, const Color& color);
		void Text(Font* f, const std::string& s, int charHeight, const Color& color);
		void Text(Font* f, const std::string& s, float charHeight, const Color& color);

		void SetText(const std::string& s);
		const std::string& GetText();

		void SetEditable();

		std::vector<bool*> tags;
		std::vector<std::function<bool()>> tagsF;
		/*
		Optimize this function?
		It runs on every update call...
		*/
		bool HasTags();
		void AddTag(bool*);
		void AddTag(std::function<bool()>);

		std::vector<ITransition> transitions;
		ITransition* NewTransition(bool* activator);
		ITransition* NewTransition(std::function<bool()> activator);
		/*
		True if mouse is held on element.
		*/
		bool isHolding = false;
		/*
		True if item has been clicked on. Used for typing text
		*/
		bool isSelected = false;
		/*
		True if Mouse is hovering over element.
		*/
		bool isHovering = false;

		std::function<void(int mx, int my, int button, int action)> OnClick = nullptr;
		std::function<void(double scroll)> OnScroll = nullptr;
		std::function<void(int mx, int my)> OnHover = nullptr;
		std::function<void(int key, int action)> OnKey = nullptr;
		bool ClickEvent(int mx, int my, int button, int action);
		bool ScrollEvent(double scroll);
		bool HoverEvent(int mx, int my);
		bool KeyEvent(int key, int action);
		//void ResizeEvent(int width,int height);

		void InternalUpdate(float delta);
		virtual void Update(float delta);
		virtual void Draw();

		bool Inside(float mx, float my);
	};
}