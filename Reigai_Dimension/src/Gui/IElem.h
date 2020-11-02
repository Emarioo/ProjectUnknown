#pragma once

#include "Text.h"
#include "Utility/Keyboard.h"

/*
Action is run on events: Resize, Type, Click, Hover
 Action types: Change color with fade, change dimensions of element
 When a certain time has passed run a function which is linked with this action
*/

void AddTag(int t);
bool HasTag(int t);
void DelTag(int t);

class IElem;
class IAction { // TODO: move some of these functions into IElem.cpp
private:
	void(*func)()=nullptr;
	float funcS=0,funcT=0;
	bool fade=false,move=false,size=false;
	float fadeS = 0, moveS = 0, sizeS=0;

public:
	float r=1, g=1, b=1, a=1;
	float fadeT=0, moveT=0, sizeT=0;
	IPos xy;
	ISize wh;
	void Func(void(*f)(),float time) {
		func = f;
		funcS = 1/(time*60);
	}
	
	void Fade(float f0,float f1,float f2,float f3,float time) {
		fade = true;
		r = f0;
		g = f1;
		b = f2;
		a = f3;
		fadeS = 1/(time*60);
	}
	/*
	Add upon pos
	*/
	void Move(IPos p, float time) {
		move = true;
		xy = p;
		moveS = 1/(time*60);
	}
	/*
	Add upon size
	*/
	void Size(ISize s, float time) {
		size = true;
		wh = s;
		sizeS = 1/(time*60);
	}
	void Update(bool b) {
		if (func!=nullptr) {
			if (b||funcT>0)
				funcT += funcS;
			if (funcT >= 1 && funcT < 1+funcS)
				func();
			if (funcT > 1&&!b) 
				funcT = 0;
		}
		if (fade) {
			if (b) {
				fadeT += fadeS;
			} else {
				fadeT -= fadeS;
			}
			if (fadeT < 0) {
				fadeT = 0;
			} else if (fadeT>1) {
				fadeT = 1;
			}
		}
		if (move) {
			if (b) {
				moveT += moveS;
			} else {
				moveT -= moveS;
			}
			if (moveT < 0) {
				moveT = 0;
			} else if (moveT > 1) {
				moveT = 1;
			}
		}
		if (size) {
			if (b) {
				sizeT += sizeS;
			} else {
				sizeT -= sizeS;
			}
			if (sizeT < 0) {
				sizeT = 0;
			} else if (sizeT > 1) {
				sizeT = 1;
			}
		}
	}
	IAction() {}
};

class IElem {
private:
	IPos pos;
	ISize size;

	std::vector<int> tags;

	std::string texture = "blank";
	float r = 1, g = 1, b = 1, a = 1;

	Text text;

	BufferContainer cont;
public:
	IPos* origin=nullptr;
	IElem(std::string n) {
		name = n;
	}
	std::string name;

	bool HasTag();
	void AddTag(int t);

	float GetX();
	float GetY();
	float GetW();
	float GetH();

	/* 
		float on screen 
	*/
	void Dim(float, float, float, float);
	/* 
		centered pixels on current screen
	*/
	void Dimi(int, int, int, int);
	/* 
		pixels on screen forever
	*/
	void Dimp(int, int, int, int);
	/*
		Update dimensions for cont 
	*/
	void SetCont();
	void SetCont(float w,float h);

	// Coloring
	void Tex(std::string name);
	void Tex(float r, float g, float b, float a);
	void Tex(std::string name, float r, float g, float b, float a);

	// Text
	void Text(std::string,Font*);
	void Text(std::string,Font*,float f0,float f1,float f2,float f3);
	void Text(int max,Font*);
	void Text(int max,Font*,float f0,float f1,float f2,float f3);
	void Text(int max,std::string,Font*,float f0,float f1,float f2,float f3);
	void SetText(std::string);
	std::string GetText();

	// Drawing
	void Draw();

	// Update
	void Update();
	
	// Actions
	IAction HoverEvent;
	IAction ClickEvent;
	IAction TypeEvent;

	// Events
	bool hovering=false,clicking=false,typing=false,selected=false;
	void Hover(int mx,int my);
	bool Click(int mx,int my, int action);
	void Type(int key, int action);// key?
	void Resize(int wi,int he);
	void Typing();

	bool Inside(float mx, float my);
};

