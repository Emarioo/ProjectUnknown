#include "IElem.h"

std::vector<int> currentTags;
void AddTag(int t) {
	for (int i = 0; i < currentTags.size(); i++) {
		if (t == currentTags.at(i))
			return;
	}
	currentTags.push_back(t);
}
bool HasTag(int t) {
	for (int i = 0; i < currentTags.size(); i++) {
		if (t == currentTags.at(i))
			return true;
	}
	return false;
}
void DelTag(int t) {
	for (int i = 0; i < currentTags.size();i++) {
		if (t==currentTags.at(i)) {
			currentTags.erase(currentTags.begin() + i);
			return;
		}
	}
}
void IElem::AddTag(int t) {
	tags.push_back(t);
}
bool IElem::HasTag() {
	for (int t1 : currentTags) {
		for (int t2 : tags) {
			if (t1 == t2) {
				return true;
			}
		}
	}
	return false;
}

void IElem::Dim(float x,float y,float w,float h) {
	pos.Data(x, y);
	size.Data(w, h);
	SetCont();
	text.ElemWH(size.GetW(), size.GetH());
}
void IElem::Dimi(int x,int y,int w,int h) {
	pos.Data(false, x, y);
	size.Data(false, w, h);
	SetCont();
	text.ElemWH(size.GetW(), size.GetH());
}
void IElem::Dimp(int x,int y,int w,int h) {
	pos.Data(true, x, y);
	size.Data(true, w, h);
	SetCont();
	text.ElemWH(size.GetW(),size.GetH());
}

void IElem::Tex(std::string name) {
	texture = name;
}
void IElem::Tex(float r, float g, float b, float a) {
	this->r = r;
	this->g = g;
	this->b = b;
	this->a = a;
}
void IElem::Tex(std::string name, float r, float g, float b, float a) {
	texture = name;
	this->r = r;
	this->g = g;
	this->b = b;
	this->a = a;
}
void IElem::Text(std::string s,Font* f) {
	text.Setup(f,s.size(),true);
	text.SetText(s);
}
void IElem::Text(std::string s, Font* f, float f0, float f1, float f2, float f3) {
	text.Setup(f, s.size(), true);
	text.SetCol(f0, f1, f2, f3);
	text.SetText(s);
}
void IElem::Text(int max, Font* f) {
	text.Setup(f, max, true);
}
void IElem::Text(int max, Font* f, float f0, float f1, float f2, float f3) {
	text.Setup(f, max, true);
	text.SetCol(f0, f1, f2, f3);
}
void IElem::Text(int max, std::string s,Font* f, float f0, float f1, float f2, float f3) {
	text.Setup(f, max, true);
	text.SetCol(f0, f1, f2, f3);
	text.SetText(s);
}
void IElem::SetText(std::string s) {
	if (text.font!=nullptr) {
		text.SetText(s);
	}
}
void IElem::Typing() {
	typing = true;
}
std::string IElem::GetText() {
	return text.text;
}
float IElem::GetX() {
	if(origin!=nullptr)
		return origin->GetX()+ pos.GetX();
	return pos.GetX();
}
float IElem::GetY() {
	if(origin!=nullptr)
		return origin->GetY() +pos.GetY();
	return pos.GetY();
}
float IElem::GetW() {
	return size.GetW();
}
float IElem::GetH() {
	return size.GetH();
}
void IElem::Resize(int wi, int he) {
	if (size.pixelScreen) {
		SetCont();
	}
}
void IElem::SetCont() {
	float tw = GetW() / 2;
	float th = GetH() / 2;

	unsigned int t[]{
		2,1,0,
		0,3,2
	};
	float v[]{
		-tw,-th,0,0,
		-tw,th,0,1,
		tw,th,1,1,
		tw,-th,1,0
	};
	cont.Setup(true, v, 4 * 4, t, 6);
	cont.SetAttrib(0, 4, 4, 0);
}
void IElem::SetCont(float w,float h) {
	float tw = w / 2;
	float th = h / 2;

	float v[]{
		-tw,-th,0,0,
		-tw,th,0,1,
		tw,th,1,1,
		tw,-th,1,0
	};
	cont.SubVB(0, 4 * 4, v);
}
void IElem::Update() {
	if (!HasTag())
		return;
	HoverEvent.Update(hovering);
	ClickEvent.Update(clicking);
}
void IElem::Draw() { // TODO: Optimize this masterpiece of junk
	if (!HasTag())
		return;
	float x = GetX() + HoverEvent.xy.GetX()*HoverEvent.moveT + ClickEvent.xy.GetX()*ClickEvent.moveT;
	float y = GetY() + HoverEvent.xy.GetY()*HoverEvent.moveT + ClickEvent.xy.GetY()*ClickEvent.moveT;
	GuiTransform(x,y);
	float red = r * (1 - HoverEvent.fadeT)*(1 - ClickEvent.fadeT) + HoverEvent.r*HoverEvent.fadeT*(1 - ClickEvent.fadeT/2) + ClickEvent.r*ClickEvent.fadeT*(1 - HoverEvent.fadeT/2);
	float green = g * (1 - HoverEvent.fadeT)*(1 - ClickEvent.fadeT) + HoverEvent.g*HoverEvent.fadeT*(1 - ClickEvent.fadeT/2) + ClickEvent.g*ClickEvent.fadeT*(1 - HoverEvent.fadeT/2);
	float blue = b * (1 - HoverEvent.fadeT)*(1 - ClickEvent.fadeT) + HoverEvent.b*HoverEvent.fadeT*(1 - ClickEvent.fadeT/2) + ClickEvent.b*ClickEvent.fadeT*(1 - HoverEvent.fadeT/2);
	float alpha = a * (1 - HoverEvent.fadeT)*(1 - ClickEvent.fadeT) + HoverEvent.a*HoverEvent.fadeT*(1 - ClickEvent.fadeT/2) + ClickEvent.a*ClickEvent.fadeT*(1 - HoverEvent.fadeT/2);
	GuiColor(red, green, blue, alpha);

	float w = GetW() + HoverEvent.wh.GetW()*HoverEvent.sizeT + ClickEvent.wh.GetW()*ClickEvent.sizeT;
	float h = GetH() + HoverEvent.wh.GetH()*HoverEvent.sizeT + ClickEvent.wh.GetH()*ClickEvent.sizeT;
	
	if (w != GetW()||h!=GetH()) {
		SetCont(w,h);
	}
	BindTexture(texture);
	cont.Draw();

	if (text.text.length() > 0) {
		text.SetPos(x,y);
		text.DrawString(alpha,typing&&selected);
	}
}
void IElem::Hover(int mx, int my) {
	if (!HasTag())
		return;
	if (Inside(mx, my)) {
		hovering = true;
	} else {
		hovering = false;
	}
}
bool IElem::Click(int mx, int my, int action) {
	if (!HasTag())
		return false;
	if (Inside(mx, my)) {
		clicking = action == 1;
		selected = true;
	} else {
		clicking = false;
		selected = false;
	}
	return clicking;
}
bool elemShiftL = false,elemShiftR=false, elemAltR=false;
void IElem::Type(int key, int action) {
	if (!HasTag())
		return;
	if (typing&&selected) {
		text.ElemWH(size.GetW(), size.GetH());
		if (key == GLFW_KEY_LEFT_SHIFT) {
			elemShiftL = action;
		} if (key == GLFW_KEY_RIGHT_SHIFT) {
			elemShiftR = action;
		} else if (key == GLFW_KEY_RIGHT_ALT) {
			elemAltR = action;
		} else if (key==GLFW_KEY_UP) {
			if (action) {
				int c = text.atChar;
				int l = text.CharOnLine(c);
				int newC = -1;
				//std::cout << "start " << c << " " << l << std::endl;
			
				for (int i = c; i >= 0; i--) {
					if (i >= text.text.length())
						continue;
					if (c==i&&text.text.at(c) == '\n')
						continue;
					//std::cout << ">" << text.text.at(i) <<" "<<i<< std::endl;
					if (text.text.at(i) == '\n') {
						int o = 0;
						for (int j = i - 1; j >= 0;j--) {
							//std::cout <<" >" <<text.text.at(j)<<" "<<j << std::endl;
							if (text.text.at(j) == '\n') {
								o = j+1;
								break;
							}
							if (j == 0) {
								o = 0;
							}
						}
						if (o+l > i)
							newC = i;
						else
							newC = o+l;
						break;
					}
				}
				if (newC < 0) {
					newC = 0;
				}
				//std::cout << "out " << newC << std::endl;
				text.atChar = newC;
				/* For uneven width on characters - TODO: Use the closest character instead of the last one
				int c = text.atChar;
				int newC = c;
				float widO = text.PixelPosX(text.atChar);
				float wid = -1;
				for (int i = c; i >= 0; i--) {
					char cha = text.text.at(i);
					if (cha == '\n') {
						if (wid > 0) {
							newC = i;
							break;
						}
						wid = 0;
						if (i != 0) {
							for (int j = i-1; j >= 0; j--) {
								char cha2 = text.text.at(j);
								if (cha2 == '\n') {
									break;
								}
								wid += text.font->charWid[cha2];
							}
						}
						continue;
					}
					if (cha < 0)
						cha += 256;
					if (wid >= 0) {
						wid -= text.font->charWid[cha];
						//std::cout << cha << " " << i << " " << widO << " < " << wid << std::endl;
						if (widO > wid) {
							newC = i;
							if(text.text.length()>i+1){
								if (text.text.at(i + 1)=='\n') {
									newC++;
								}
							}
							break;
						}
					}
				}
				text.atChar = newC;
				*/
			}
		} else if (key==GLFW_KEY_LEFT) {
			if (action) {
				if (text.atChar > 0)
					text.atChar--;
			}
		} else if (key == GLFW_KEY_DOWN) {
			if (action) {
				int c = text.atChar;
				int l = text.CharOnLine(c);
				int newC = -1;
				int o = 0;
				//std::cout << "start " << c <<" "<<l<< std::endl;
				
				for (int i = c; i < text.text.length(); i++) {
					char cha = text.text.at(i);
					//std::cout << "ch " << cha << std::endl;
					if (newC >= 0) {
						if (newC == l) {
							newC = l + o + 1;
							//std::cout << "end1 " << newC << std::endl;
							break;
						}
						newC++;
					}
					if (cha == '\n') {
						if (newC >= 0) {
							if (newC < l+1) {
								newC = i;
								break;
							}
							newC += o + 1;
							//std::cout << "end2 " << newC << std::endl;
							break;
						}
						o = i;
						newC = 0;
						continue;
					}
					if (i == text.text.length() - 1) {
						if (newC < 0) {
							break;
						}
						newC += o + 1;
					}
				}
				if (newC < 0) {
					newC = text.text.length();
				}
				//std::cout << "out " << newC << std::endl;
				text.atChar = newC;

				/* characters width different widths
				int c = text.atChar;
				int newC = c;
				float widO = text.PixelPosX(text.atChar);
				float wid = -1;
				for (int i = c; i < text.text.length(); i++) {
					char cha = text.text.at(i);
					if (cha == '\n') {
						if (wid > 0) {
							newC = i;
							break;
						}
						wid = 0;
						continue;
					}
					if (cha < 0)
						cha += 256;
					if(wid>=0)
						wid += text.font->charWid[cha];
					//std::cout << cha << " " << i << " " << widO << " < " << wid << std::endl;
					if (widO < wid) {
						newC = i;
						break;
					}
				}
				text.atChar = newC;
				*/
			}
		} else if (key == GLFW_KEY_RIGHT) {
			if (action) {
				if (text.atChar < text.text.size())
					text.atChar++;
			}
		} else if (key == GLFW_KEY_DELETE) {
			if (action) {
				if (text.text.size() > 0 && text.atChar < text.text.size()) {
					if (text.atChar == 0) {
						text.SetText(text.text.substr(1));
					} else {
						text.SetText(text.text.substr(0, text.atChar) + text.text.substr(text.atChar+1));
					}
				}
			}
		} else if (key == GLFW_KEY_BACKSPACE) {
			if (action) {
				if (text.text.length() > 0&&text.atChar>0) {
					if (text.atChar == text.text.size()) {
						text.SetText(text.text.substr(0, text.atChar - 1));
						text.atChar--;
					} else {
						text.SetText(text.text.substr(0, text.atChar - 1) + text.text.substr(text.atChar));
						text.atChar--;
					}
				}
			}
		} else if (key == GLFW_KEY_ENTER) {
			if (action) {
				if (text.text.size() < text.maxChar) {
					text.SetText(text.text.substr(0, text.atChar) + '\n' + text.text.substr(text.atChar));
					text.atChar++;
				}
			}
		} else {
			if (action) {
				char cha = GetChar(key, elemShiftL || elemShiftR, elemAltR);
				if (cha != 0) {
					if (text.text.size() < text.maxChar) {
						text.SetText(text.text.substr(0, text.atChar) + cha + text.text.substr(text.atChar));
						text.atChar++;
					}
				}
			}
		}
	}
}
bool IElem::Inside(float mx, float my) {
	mx = AlterX(mx);
	my = AlterY(my);
	return mx > GetX()-GetW()/2&&mx<GetX() + GetW()/2 && my>GetY()-GetH()/2&&my < GetY() + GetH()/2;
}