#pragma once

#include "Text.h"
#include <functional>
#include <vector>

class IElement;
class IConstraint {
public:
	IConstraint(IElement* e,bool use_xAxis);
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
	bool movePixel = false, sizePixel = false;
	float x = 0, y = 0, w = 0, h = 0;
public:
	bool fade = false, move = false, size = false;
	float r = 1, g = 1, b = 1, a = 1;
	float fadeT = 0, moveT = 0, sizeT = 0;

	ITransition(bool* act);

	ITransition* Fade(float r,float g,float b,float a,float time);
	ITransition* Fade(int r, int g, int b, float a, float time);
	ITransition* Move(int x,int y, float time);
	ITransition* Move(float x,float y, float time);
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
	IElement(const std::string& name,int priority);
	~IElement();

	// Final values
	// They get updated as things change
	float x=0, y=0, w=0, h=0;
	float r = 1, g = 1, b = 1, a = 1;

	bool isBlank=false;

	bool isEditable=false;
	
	int priority = 0;

	std::string name;

	IConstraint conX;
	IConstraint conY;
	IConstraint conW;
	IConstraint conH;

	std::string texture="blank";
	
	Text text;

	IElement* Texture(const std::string& texture);
	IElement* Color(float red,float green,float blue,float alpha);
	IElement* Color(int red, int green, int blue, float alpha);
	IElement* Blank();
	/*
	Fit text in this element
	*/
	IElement* Text(Font* f, const std::string& s, float f0, float f1, float f2, float f3);
	IElement* Text(Font* f, const std::string& s, int charHeight, float f0, float f1, float f2, float f3);
	IElement* Text(Font* f, const std::string& s, float charHeight, float f0, float f1, float f2, float f3);

	void SetText(const std::string& s);
	const std::string& GetText();

	void SetEditable();

	std::vector<bool*> tags;
	bool HasTags();
	void AddTag(bool*);

	std::vector<ITransition> transitions;
	ITransition* NewTransition(bool* activator);

	bool isHolding = false;
	bool isSelected = false;
	bool isHovering = false;

	std::function<void(int mx,int my,int button,int action)> OnClick=nullptr;
	std::function<void(double scroll)> OnScroll=nullptr;
	std::function<void(int mx,int my)> OnHover=nullptr;
	std::function<void(int key,int action)> OnKey=nullptr;
	bool ClickEvent(int mx,int my,int button,int action);
	bool ScrollEvent(double scroll);
	bool HoverEvent(int mx,int my);
	bool KeyEvent(int key,int action);
	//void ResizeEvent(int width,int height);

	void InternalUpdate(float delta);
	virtual void Update(float delta);
	virtual void Draw();

	bool Inside(float mx, float my);
};