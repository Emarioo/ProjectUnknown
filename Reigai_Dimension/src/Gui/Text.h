#pragma once

#include "Rendering/BufferContainer.h"
#include "Rendering/Renderer.h"
#include "Font.h"
#include "IDimensions.h"
/*
Center or Edge with float
String or maxchar
Dynamic textsizing or not



*/
class Text {
public:
	Text(){}
	int maxChar=0;
	bool center = false;
	void Center(bool f);
	void Setup(Font* f,int max, bool cent);

	Font* font=nullptr;
	void SetFont(Font* f);

	std::string text;
	/*
	Updates container
	*/
	//void SetText(std::string text,float h);
	void SetText(std::string text);

	void ElemWH(float, float);
	float elemW=0, elemH=0;// Temporary
	IPos xy;
	void SetXY(IPos);

	float xpos=0,ypos=0;
	void SetPos(float x,float y);
	/*
	In pixels or not
	*/
	float PixelWidth(bool f);
	float PixelHeight(bool f);
	/*
	Argument can't be more than size of text
	*/
	float ScreenPosX(int c);
	/*
	Argument can't be more than size of text
	*/
	float ScreenPosY(int c);
	/*
	Argument can't be more than size of text
	*/
	int PixelPosX(int c);

	int CharOnLine(int c);

	float charWidth = 0.1;
	float charHeight = 0.1;
	void SetHeight(float s);

	float r=1,g=1,b=1,a=1;
	void SetCol(float r, float g, float b, float a);
	/*
	Update SetPos(x,y)
	*/
	void DrawString(float alpha,bool marker);
	BufferContainer container;
	/*
	Can be altered without complications
	*/
	int atChar=0;
	BufferContainer marker;
};