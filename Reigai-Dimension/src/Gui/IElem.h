#pragma once

#include "Text.h"
#include "Utility/Keyboard.h"
#include "Utility/ExtraFunc.h"
#include <functional>
#include <unordered_map>

/*
out is an array of 4
Can cause error if format is incorrect
Using p as suffix will divide the element by the following "x/1920,y/1080,w/1920,h/1080"
Example of formatting: "0.5,0.5,200p,100p" will place the IElem the center of upper right with a width of 200 and height of 100
*/
void DimFormat(std::string s, float* out); // TODO: IElem Dim check for errors in format before they happen
/*
out is an array of 2;
Example formatting: "0.5,100p"
*/
void Dim2Format(std::string s, float* out); // TODO: IElem Dim check for errors in format before they happen
/*
Action is run on events: Resize, Type, Click, Hover
 Action types: Change color with fade, change dimensions of element
 When a certain time has passed run a function which is linked with this action
*/
class IAction { // TODO: move some of these functions into IElem.cpp or not
private:
	std::function<void()> func=nullptr;
	float funcS=0,funcT=0;
	bool fade=false,move=false,size=false;
	float fadeS = 0, moveS = 0, sizeS=0;

public:
	float r=1, g=1, b=1, a=1;
	float fadeT=0, moveT=0, sizeT=0;
	float x=1, y=1, w=1, h=1;
	void Func(std::function<void()> f, float time);
	void Fade(float f0, float f1, float f2, float f3, float time);
	void Move(const std::string& xy, float time);
	void Size(const std::string& wh, float time);
	void Update(float delta, bool b);
	IAction(){}
};

class IElem {
public:
	std::string name;
	float x=0,y=0,w=0,h=0;
	std::vector<bool*> tags;
	std::string texture = "blank";
	float r = 1, g = 1, b = 1, a = 1;
	BufferContainer cont;

	IElem() {}

	bool HasTags();
	void AddTag(bool*);

	float GetX();
	float GetY();
	float GetW();
	float GetH();
	float GetRed();
	float GetGreen();
	float GetBlue();
	float GetAlpha();

	/*
	Initial update for container and text
	*/
	void UpdateCont();
	/*
	Update dimensions for container and text
	*/
	void UpdateCont(float w,float h);

	Text text;
	void Text(const std::string&, Font*);
	void Text(const std::string&, Font*, float f0, float f1, float f2, float f3);
	void Text(int max, Font*);
	void Text(int max, const std::string&, Font*);
	void Text(int max, Font*, float f0, float f1, float f2, float f3);
	void Text(int max, const std::string&, Font*, float f0, float f1, float f2, float f3);
	void SetText(const std::string&);
	std::string GetText();

	void DrawNormal();
	/*
	Can be customized for derived classes if neccessary.
	See IElem.cpp > void Draw() for inspiration
	*/
	virtual void Draw();
	/*
	Can be customized for derived classes if neccessary.
	See IElem.cpp > void Update(float delta) for inspiration
	*/
	virtual void Update(float delta);
	
	// Actions
	IAction Hover;
	IAction Click;
	std::unordered_map<bool*,IAction> customActions;

	IAction* NewAction(bool *b);

	// Events
	bool hovering=false,clicked=false,selected=false;
	bool typing = false; // Rest product of IInput.h : this can't be placed anywhere else due to how i have designed it
	void HoverEvent(int mx,int my);
	bool ClickEvent(int mx,int my, int action);
	/*
	Only used if the element always has 200p in size no matter the windows size
	This is currently deprecated
	*/
	void Resize(int wi,int he);

	bool Inside(float mx, float my);
};

