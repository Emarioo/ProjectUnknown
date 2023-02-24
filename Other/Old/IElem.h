#pragma once

#include "Text.h"
#include "Utility/Keyboard.h"
#include "Utility/ExtraFunc.h"
#include <functional>
#include <unordered_map>

/*
count is the same size as out (error will occur otherwise :/ )
Can cause error if format is incorrect
Using w will divide value by 1920, h by 1080
Example of formatting: "0.5,0.5,200w,100h" will place the IElem the center of upper right with a width of 200/1920 and height of 100/1080
*/
void DimFormat(const std::string& s, float* out,int count); // TODO: Some way of checking errors before they happen?

/*
Action is run on events: Resize, Type, Click, Hover
 Action types: Change color with fade, change dimensions of element
 When a certain time has passed run a function which is linked with this action
*/
class IAction { // TODO: move some of these functions into IElem.cpp or not
private:
	std::function<void(int)> func=nullptr;
	float funcS=0,funcT=0;
	bool fade=false,move=false,size=false;
	float fadeS = 0, moveS = 0, sizeS=0;

public:
	float r=1, g=1, b=1, a=1;
	float fadeT=0, moveT=0, sizeT=0;
	float x=1, y=1, w=1, h=1;
	void Func(std::function<void(int)> f, float time);
	void Fade(float f0, float f1, float f2, float f3, float time);
	void Move(const std::string& xy, float time);
	void Size(const std::string& wh, float time);
	void Update(float delta, bool b, int funcData);
	void ResetFunc();
	IAction(){}
};
/*
Deprecated class since 2020-12-13
*/
class IElem {
public:
	std::string name;
	float x=0,y=0,w=0,h=0;
	std::vector<bool*> tags;
	std::string texture = "blank";
	float r = 1, g = 1, b = 1, a = 1;
	int funcData=0;
	//BufferContainer cont;

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
	virtual void UpdateCont();
	/*
	Update dimensions for container and text
	*/
	void UpdateCont(float w,float h);

	Text text;
	
	void Text(Font* f);
	void Text(Font* f, const std::string& s);
	void Text(Font* f, float f0, float f1, float f2, float f3);
	void Text(Font* f, const std::string&, float f0, float f1, float f2, float f3);
	void Text(Font* f, const std::string&, const std::string& height, float f0, float f1, float f2, float f3);
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
	bool typing = false; // Rest product of IInput.h : this can't be placed anywhere else due to how i designed it
	std::function<void(int mx, int my)> OnHover=nullptr;
	virtual void HoverEvent(int mx,int my);
	std::function<void(int mx, int my)> OnClick=nullptr;
	virtual bool ClickEvent(int mx,int my, int action);
	//std::function<void(int mx, int my)> OnKey=nullptr;
	//virtual bool KeyEvent(int mx,int my, int action);
	//std::function<void(int mx, int my)> OnScroll=nullptr;
	//virtual bool ScrollkEvent(int mx,int my, int action);
	/*
	Only used if the element always has 200p in size no matter the windows size
	This is currently deprecated
	*/
	void Resize(int wi,int he);

	bool Inside(float mx, float my);
};

