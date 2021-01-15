#include "Text.h"

#include <iostream>

namespace engine {
	/*
	 max : max length of text
	 size : standard is 64 / Window Height
	*/
	void Text::Setup(Font* f, bool cent) {
		//elemSize = s;
		font = f;
		center = cent;
		/*
		container.Setup(true, nullptr, 4 * 4 * maxChar, nullptr, 6 * maxChar);
		container.SetAttrib(0, 4, 4, 0);
		unsigned int i[]{
			2,1,0,
			0,3,2
		};

		marker.Setup(true, nullptr, 4 * 4, i, 6);
		marker.SetAttrib(0, 4, 4, 0);
		*/
	}
	void Text::SetPos(float x, float y) {
		xpos = x;
		ypos = y;
	}
	void Text::Center(bool f) {
		center = f;
	}
	void Text::SetFont(Font* f) {
		font = f;
	}
	void Text::SetHeight(int s) {
		charHeight = s;
		isPixel = true;
		staticHeight = true;
	}
	void Text::SetHeight(float s) {
		charHeight = s;
		staticHeight = true;
	}
	void Text::SetCol(float r, float g, float b, float a) {
		this->r = r;
		this->g = g;
		this->b = b;
		this->a = a;

	}
	float Text::PixelWidth(bool f) {
		if (font == nullptr)
			return 0;
		float wid = 0;
		float max = 0;
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
			max *= (charHeight / (Wid() / Hei())) / font->charSize;
		}
		return max;
	}
	float Text::PixelHeight(bool f) {
		if (font == nullptr)
			return 0;
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
		if (font == nullptr)
			return 0;
		float wid = 0;
		for (int i = 0; i < c; i++) {
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
		return wid * ((charHeight / (Wid() / Hei())) / font->charSize);
	}

	int Text::PixelPosX(int c) {
		if (font == nullptr)
			return 0;
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
		return hei * charHeight;
	}
	void Text::EditText(int key, int action) {
		//text.ElemWH(GetW(), GetH()); this is neccessary!
		if (key == GLFW_KEY_LEFT_SHIFT) {
			elemShiftL = action;
		} if (key == GLFW_KEY_RIGHT_SHIFT) {
			elemShiftR = action;
		} else if (key == GLFW_KEY_RIGHT_ALT) {
			elemAltR = action;
		} else if (key == GLFW_KEY_UP) {
			if (action) {
				int c = atChar;
				int l = CharOnLine(c);
				int newC = -1;
				//std::cout << "start " << c << " " << l << std::endl;

				for (int i = c; i >= 0; i--) {
					if (i >= text.length())
						continue;
					if (c == i && text.at(c) == '\n')
						continue;
					//std::cout << ">" << text.text.at(i) <<" "<<i<< std::endl;
					if (text.at(i) == '\n') {
						int o = 0;
						for (int j = i - 1; j >= 0; j--) {
							//std::cout <<" >" <<text.text.at(j)<<" "<<j << std::endl;
							if (text.at(j) == '\n') {
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
				atChar = newC;
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
				if (atChar > 0)
					atChar--;
			}
		} else if (key == GLFW_KEY_DOWN) {
			if (action) {
				int c = atChar;
				int l = CharOnLine(c);
				int newC = -1;
				int o = 0;
				//std::cout << "start " << c <<" "<<l<< std::endl;

				for (int i = c; i < text.length(); i++) {
					char cha = text.at(i);
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
					if (i == text.length() - 1) {
						if (newC < 0) {
							break;
						}
						newC += o + 1;
					}
				}
				if (newC < 0) {
					newC = text.length();
				}
				//std::cout << "out " << newC << std::endl;
				atChar = newC;

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
				if (atChar < text.size())
					atChar++;
			}
		} else if (key == GLFW_KEY_DELETE) {
			if (action) {
				if (text.size() > 0 && atChar < text.size()) {
					if (atChar == 0) {
						SetText(text.substr(1));
					} else {
						SetText(text.substr(0, atChar) + text.substr(atChar + 1));
					}
				}
			}
		} else if (key == GLFW_KEY_BACKSPACE) {
			if (action) {
				if (text.length() > 0 && atChar > 0) {
					if (atChar == text.size()) {
						SetText(text.substr(0, atChar - 1));
						atChar--;
					} else {
						SetText(text.substr(0, atChar - 1) + text.substr(atChar));
						atChar--;
					}
				}
			}
		} else if (key == GLFW_KEY_ENTER) {
			if (action) {
				//if (text.text.size() < text.maxChar) {
				SetText(text.substr(0, atChar) + '\n' + text.substr(atChar));
				atChar++;
				//}
			}
		} else {
			if (action) {
				char cha = GetChar(key, elemShiftL || elemShiftR, elemAltR);
				if (cha != 0) {
					//if (text.text.size() < text.maxChar) {
					SetText(text.substr(0, atChar) + cha + text.substr(atChar));
					atChar++;
					//}
				}
			}
		}
	}

	int Text::CharOnLine(int c) {
		int n = 0;
		for (int i = 0; i < c; i++) {
			if (text.at(i) == '\n') {
				n = 0;
				continue;
			}
			n++;
		}
		return n;
	}
	void Text::ElemWH(float* w, float* h) {
		elemW = w;
		elemH = h;
	}
	void Text::SetText(const std::string& atext) {
		text = atext;
		/*

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
		delete indes;*/
	}
	void Text::DrawString(float alpha, bool f) {
		// Dynamic
		float actualHeight = AlterH(charHeight);
		if (!staticHeight) {
			actualHeight = ((*elemW / 2 * Width()) / PixelWidth(true)) * AlterH(64);
			float a = *elemH / PixelHeight(false);
			if (a < 1)
				actualHeight *= a;
		}
		float x = xpos - *elemW / 2;
		float y = ypos - actualHeight + *elemH / 2;
		/*if (center) {
			x = xpos - PixelWidth(false) / 2;
			y = ypos - charHeight + PixelHeight(false) / 2;
		}*/

		GuiTransform(xpos, ypos);
		GuiColor(r, g, b, a * alpha);
		if (f) {
			engine::DrawString(font, text, center, actualHeight, atChar);
		} else {
			engine::DrawString(font, text, center, actualHeight, -1);
		}
		/*
		if (f) {
			//std::cout << (x + PixelPosX(atChar)) << " " << (y + PixelPosY(atChar)) << std::endl;
			renderer::GuiTransform(x+ScreenPosX(atChar), y-ScreenPosY(atChar));
			renderer::GuiColor(r, g, b, a);
			renderer::BindTexture("blank");
			marker.Draw();
		}*/
	}
}