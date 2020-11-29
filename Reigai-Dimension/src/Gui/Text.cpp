#include "Text.h"

#include <iostream>

/*
 max : max length of text
 size : standard is 64 / Window Height 
*/

void Text::Setup(Font* f,int max, bool cent) {
	//elemSize = s;
	font=f;
	maxChar = max;
	center = cent;
	container.Setup(true, nullptr, 4 * 4 * maxChar, nullptr, 6 * maxChar);
	container.SetAttrib(0, 4, 4, 0);
	unsigned int i[]{
		2,1,0,
		0,3,2
	};
	marker.Setup(true, nullptr, 4 * 4, i, 6);
	marker.SetAttrib(0, 4, 4, 0);
}
void Text::SetXY(IPos p) {
	xy = p;
}
void Text::SetPos(float x,float y) {
	xpos = x;
	ypos = y;
}
void Text::Center(bool f) {
	center = f;
}
void Text::SetFont(Font* f) {
	font = f;
}
void Text::SetHeight(float s) {
	charHeight = s;
}
void Text::SetCol(float r, float g, float b, float a) {
	this->r = r;
	this->g = g;
	this->b = b;
	this->a = a;

}
void Insert4(float* ar, int ind, float f0, float f1, float f2, float f3) {
	ar[ind] = f0;
	ar[ind+1] = f1;
	ar[ind+2] = f2;
	ar[ind+3] = f3;
}
float Text::PixelWidth(bool f) {
	float wid=0;
	float max=0;
	for (char cha : text) {
		if (cha == '\n') {
			wid = 0;
			continue;
		}
		int charInd = cha;
		if (charInd < 0)
			charInd += 256;
		wid += font->charWid[charInd];
		if (wid > max) {
			max = wid;
		}
	}
	if (!f) {
		max *= (charHeight / (renderer::Wid() / renderer::Hei())) / font->charSize;
	}
	return max;
}
float Text::PixelHeight(bool f) {
	float hei = font->charSize;
	for (char cha : text) {
		if (cha == '\n') {
			hei += font->charSize;
		}
	}
	if (!f) {
		hei *= charHeight / font->charSize;
	}
	return hei; 
}
float Text::ScreenPosX(int c) {
	float wid = 0;
	for (int i = 0; i < c;i++) {
		int cha = text.at(i);
		if ((char)cha == '\n') {
			wid = 0;
			continue;
		}
		if (cha < 0) {
			cha += 256;
		}
		wid += font->charWid[cha];
	}
	return wid*((charHeight / (renderer::Wid() / renderer::Hei())) / font->charSize);
}

int Text::PixelPosX(int c) {
	float wid = 0;
	for (int i = 0; i < c; i++) {
		char cha = text.at(i);
		if (cha == '\n') {
			wid = 0;
			continue;
		}
		//std::cout << cha <<" "<< (int)cha << " "<<(int)(cha+256)<<std::endl;
		if (cha < 0)
			cha += 256;
		wid += font->charWid[cha];
	}
	return wid;
}
float Text::ScreenPosY(int c) {
	float hei = 0;
	for (int i = 0; i < c; i++) {
		char cha = text.at(i);
		if (cha == '\n') {
			hei++;
		}
	}
	return hei*charHeight;
}
int Text::CharOnLine(int c) {
	int n = 0;
	for (int i = 0; i < c;i++) {
		if (text.at(i) == '\n') {
			n = 0;
			continue;
		}
		n++;
	}
	return n;
}
void Text::ElemWH(float w, float h) {
	elemW = w;
	elemH = h;
}
void Text::SetText(std::string atext) {
	text = atext.substr(0, maxChar);

	// Dynamic
	charHeight = ((elemW / 2 * renderer::Width()) / PixelWidth(true)) * renderer::AlterH(64);
	float a = elemH / PixelHeight(false);
	if(a<1)
		charHeight *= a;

	unsigned int* indes = new unsigned int[6 * maxChar];
	float* verts = new float[4 * 4 * maxChar];

	float atX = 0;
	float atY = 0;
	float ratio = charHeight /(renderer::Wid()/ renderer::Hei());
	
	int i=0;
	for (char cha : text) {
		indes[0 + 6 * i] = 2 + 4 * i;
		indes[1 + 6 * i] = 1 + 4 * i;
		indes[2 + 6 * i] = 0 + 4 * i;
		indes[3 + 6 * i] = 0 + 4 * i;
		indes[4 + 6 * i] = 3 + 4 * i;
		indes[5 + 6 * i] = 2 + 4 * i;

		int charInd = cha;
		if (cha == '\n') {
			atY -= charHeight;
			atX = 0;
			i++;
			continue;
		}
		if (charInd < 0)
			charInd += 256;
		float wp = font->charWid[charInd] / (float)font->imgSize;
		float wi = ratio*(font->charWid[charInd]/(float)font->charSize);
		float xp = (charInd % 16);
		float yp = 15-(charInd / 16);
		
		Insert4(verts,   16 * i, atX, atY, (xp) / 16, (yp) / 16);
		Insert4(verts, 4+16 * i, atX, atY+ charHeight, (xp) / 16, (yp+1) / 16);
		Insert4(verts, 8+16 * i, atX+wi, atY+ charHeight, (xp) / 16+wp, (yp+1) / 16);
		Insert4(verts,12+16 * i, atX+wi, atY, (xp) / 16+wp, (yp) / 16);
		
		atX += wi;// +charSpacing;
		
		i++;
	}
	for (int i = text.length(); i < maxChar; i++) {
		indes[0 + 6 * i] = 0;
		indes[1 + 6 * i] = 0;
		indes[2 + 6 * i] = 0;
		indes[3 + 6 * i] = 0;
		indes[4 + 6 * i] = 0;
		indes[5 + 6 * i] = 0;
		for (int j = 0; j < 16;j++) {
			verts[j + 16 * i] = 0;
		}
	}

	container.SubVB(0,4*4*maxChar,verts);
	container.SubIB(0, 6 * maxChar,indes);

	//std::cout << charHeight <<" "<< (ratio / 16) << std::endl;

	float v[]{
		0,0,0,0,
		0,charHeight,0,0,
		ratio/16,charHeight,0,0,
		ratio/16,0,0,0
	};

	marker.SubVB(0, 4 * 4, v);

	delete verts;
	delete indes;
}
void Text::DrawString(float alpha,bool f) {
	float x = 0;
	float y = 0;
	if (center) {
		x = xy.GetX() + xpos - PixelWidth(false) / 2;
		y = xy.GetY() + ypos - charHeight + PixelHeight(false) / 2;
	} else {
		x = xy.GetX() + xpos - elemW / 2;
		y = xy.GetY() + ypos - charHeight + elemH / 2;
		
	}
	renderer::GuiTransform(x,y);
	if(font!=nullptr)
		if (font->texture != nullptr)
			font->texture->Bind();
	
	renderer::GuiColor(r, g, b, a);
	container.Draw();
	
	if (f) {
		//std::cout << (x + PixelPosX(atChar)) << " " << (y + PixelPosY(atChar)) << std::endl;
		renderer::GuiTransform(x+ScreenPosX(atChar), y-ScreenPosY(atChar));
		renderer::GuiColor(r, g, b, a);
		renderer::BindTexture("blank");
		marker.Draw();
	}
}