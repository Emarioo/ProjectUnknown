#include "IInput.h"

IInput::IInput(std::string name, std::string dim, const std::string& texture) {
	this->name = name;
	float arr[4];
	DimFormat(dim, arr);
	x = arr[0]; y = arr[1], w = arr[2], h = arr[3];
	this->texture = texture;
	UpdateCont();
}
IInput::IInput(std::string name, std::string dim, float r, float g, float b, float a) {
	this->name = name;
	float arr[4];
	DimFormat(dim, arr);
	x = arr[0]; y = arr[1], w = arr[2], h = arr[3];
	this->r = r; this->g = g; this->b = b; this->a = a;
	UpdateCont();
}
IInput::IInput(std::string name, std::string dim, float r, float g, float b, float a, const std::string& texture) {
	this->name = name;
	float arr[4];
	DimFormat(dim, arr);
	x = arr[0]; y = arr[1], w = arr[2], h = arr[3];
	this->r = r; this->g = g; this->b = b; this->a = a;
	this->texture = texture;
	UpdateCont();
}
/*
void IInput::Update(float delta) {

}
void IInput::Draw() {
	if (!HasTags())return;
	DrawNormal();
}*/
void IInput::Typing() {
	typing = true;
}
bool elemShiftL = false, elemShiftR = false, elemAltR = false;
void IInput::Type(int key, int action) {
	if (!HasTags()) return;
	if (typing && selected) {
		text.ElemWH(GetW(), GetH());// Might not be neccessary
		if (key == GLFW_KEY_LEFT_SHIFT) {
			elemShiftL = action;
		} if (key == GLFW_KEY_RIGHT_SHIFT) {
			elemShiftR = action;
		} else if (key == GLFW_KEY_RIGHT_ALT) {
			elemAltR = action;
		} else if (key == GLFW_KEY_UP) {
			if (action) {
				int c = text.atChar;
				int l = text.CharOnLine(c);
				int newC = -1;
				//std::cout << "start " << c << " " << l << std::endl;

				for (int i = c; i >= 0; i--) {
					if (i >= text.text.length())
						continue;
					if (c == i && text.text.at(c) == '\n')
						continue;
					//std::cout << ">" << text.text.at(i) <<" "<<i<< std::endl;
					if (text.text.at(i) == '\n') {
						int o = 0;
						for (int j = i - 1; j >= 0; j--) {
							//std::cout <<" >" <<text.text.at(j)<<" "<<j << std::endl;
							if (text.text.at(j) == '\n') {
								o = j + 1;
								break;
							}
							if (j == 0) {
								o = 0;
							}
						}
						if (o + l > i)
							newC = i;
						else
							newC = o + l;
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
		} else if (key == GLFW_KEY_LEFT) {
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
							if (newC < l + 1) {
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
						text.SetText(text.text.substr(0, text.atChar) + text.text.substr(text.atChar + 1));
					}
				}
			}
		} else if (key == GLFW_KEY_BACKSPACE) {
			if (action) {
				if (text.text.length() > 0 && text.atChar > 0) {
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