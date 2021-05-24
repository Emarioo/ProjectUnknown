#include "IElem.h"

/*
This class is deprecated since 2020-12-13
*/

void DimFormat(const std::string& s, float* out, int count) {
	std::vector<std::string> split = SplitString(s, ",");
	if (split.size() == count) {
		for (int i = 0; i < count; i++) {
			if (split[i].back() == 'w') {
				split[i].pop_back();
				out[i] = 2.f * atoi(split[i].c_str()) / 1920.f;
				
			} else if (split[i].back() == 'h') {
				split[i].pop_back();
				out[i] = 2.f * atoi(split[i].c_str()) / 1080.f;
			} else {
				split[i].pop_back();
				out[i] = atof(split[i].c_str());
			}
		}
	} else {
		bug::out < bug::RED < "Error in Element Format: " < s < bug::end;
		for (int i = 0; i < count; i++) {
			out[i] = 0;
		}
	}
}
void IAction::Func(std::function<void(int)> f, float time) {
	func = f;
	funcS = 1 / time;
}
void IAction::Fade(float f0, float f1, float f2, float f3, float time) {
	fade = true;
	r = f0;
	g = f1;
	b = f2;
	a = f3;
	fadeS = 1 / time;
}
void IAction::Move(const std::string& xy, float time) {
	move = true;
	float arr[2];
	DimFormat(xy, arr,2);
	x = arr[0];
	y = arr[1];
	moveS = 1 / time;
}
void IAction::Size(const std::string& wh, float time) {
	size = true;
	float arr[2];
	DimFormat(wh, arr,2);
	w = arr[0];
	h = arr[1];
	sizeS = 1 / time;
}
void IAction::Update(float delta, bool b,int funcData) {
	if (func != nullptr) {
		if (b || funcT > 0)
			funcT += funcS * delta;
		if (funcT >= 1 && funcT < 1 + funcS * delta)
			func(funcData);
		if (funcT > 1 && !b)
			funcT = 0;
	}
	if (fade) {
		if (b) {
			fadeT += fadeS * delta;
		} else {
			fadeT -= fadeS * delta;
		}
		if (fadeT < 0) {
			fadeT = 0;
		} else if (fadeT > 1) {
			fadeT = 1;
		}
	}
	if (move) {
		if (b) {
			moveT += moveS * delta;
			//bug::outs < (moveT) < bug::end;
		} else {
			moveT -= moveS * delta;
			//bug::outs < (-moveS* delta) < bug::end;
		}
		if (moveT < 0) {
			moveT = 0;
			//bug::outs < ("reset") < bug::end;
		} else if (moveT > 1) {
			moveT = 1;
		}
		//bug::outs < (moveT) < bug::end;
	}
	if (size) {
		if (b) {
			sizeT += sizeS * delta;
		} else {
			sizeT -= sizeS * delta;
		}
		if (sizeT < 0) {
			sizeT = 0;
		} else if (sizeT > 1) {
			sizeT = 1;
		}
	}
}
void IAction::ResetFunc() {
	funcT = 0;
}
void IElem::AddTag(bool* b) {
	tags.push_back(b);
}
bool IElem::HasTags() {
	for (bool* b : tags) {
		if (!*b) {
			return false;
		}
	}
	return true;
}
float IElem::GetX() {
	float out = x + Hover.x * Hover.moveT + Click.x * Click.moveT;
	for (auto& p : customActions) {
		//bug::outs < p.second.x<p.second.moveT < bug::end;
		out += p.second.x * p.second.moveT;
	}
	return out;
}
float IElem::GetY() {
	float out = y + Hover.y * Hover.moveT + Click.y * Click.moveT;
	for (auto& p : customActions) {
		out += p.second.y * p.second.moveT;
	}
	return out;
}
float IElem::GetW() {
	float out = w + Hover.w * Hover.sizeT + Click.w * Click.sizeT;
	for (auto& p : customActions) {
		out += p.second.w * p.second.sizeT;
	}
	return out;
}
float IElem::GetH() {
	float out = h + Hover.h * Hover.sizeT + Click.h * Click.sizeT;
	for (auto& p : customActions) {
		out += p.second.h * p.second.sizeT;
	}
	return out;
}
void IElem::Resize(int wi, int he) {
	//if (size.pixelScreen)	SetCont();
}
void IElem::UpdateCont() {
	/*
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
	*/
	//text.ElemWH(GetW(), GetH());
}
void IElem::UpdateCont(float w,float h) {
	/*
	float tw = w / 2;
	float th = h / 2;

	float v[]{
		-tw,-th,0,0,
		-tw,th,0,1,
		tw,th,1,1,
		tw,-th,1,0
	};
	cont.SubVB(0, 4 * 4, v);
	*/
	//text.ElemWH(GetW(), GetH());
}


void IElem::Text(Font* f) {
	text.Setup(f, true);
}
void IElem::Text(Font* f, const std::string& s) {
	text.Setup(f, true);
	text.SetText(s);
}
void IElem::Text(Font* f, float f0, float f1, float f2, float f3) {
	text.Setup(f, true);
	text.SetCol(f0, f1, f2, f3);
}
void IElem::Text(Font* f, const std::string& s, float f0, float f1, float f2, float f3) {
	text.Setup(f, true);
	text.SetCol(f0, f1, f2, f3);
	text.SetText(s);
}
/*
height is a format like "0.9" or "52p"
*/
void IElem::Text(Font* f, const std::string& s, const std::string& height, float f0, float f1, float f2, float f3) {
	text.Setup(f, true);
	text.SetCol(f0, f1, f2, f3);
	float he;
	DimFormat(height,&he,1);
	text.staticHeight = true;
	text.SetHeight(he);
	text.SetText(s);
}
void IElem::SetText(const std::string& s) {
	if (text.font != nullptr) {
		text.SetText(s);
	}
}
std::string IElem::GetText() {
	return text.text;
}
void IElem::Update(float delta) {
	if (!HasTags()) return;
	Hover.Update(delta,hovering,funcData);
	Click.Update(delta,clicked,funcData);
	for (auto& p : customActions) {
		//bug::outs < *p.first < bug::end;
		p.second.Update(delta,*p.first,funcData);
	}
}
IAction* IElem::NewAction(bool *b){
	return &(customActions[b] = IAction());
	
}
float IElem::GetRed() {
	std::vector<IAction*> actions;
	actions.push_back(&Hover);
	actions.push_back(&Click);
	for (auto& p : customActions) {
		actions.push_back(&p.second);
	}
	float out = 0;
	float temp = r;
	for (auto p : actions) {
		temp *= (1 - p->fadeT);
	}
	out += temp;
	for (auto p : actions){
		temp = p->r*p->fadeT;
		for (auto p2 : actions) {
			if(p==p2)
				continue;
			temp *= (1 - p2->fadeT/actions.size());
		}
		out += temp;
	}
	return out;
}
float lastW = 0, lastH = 0;
void IElem::DrawNormal() { // TODO: Optimize this masterpiece of junk
	float x = GetX();// +Hover.x * Hover.moveT + Click.x * Click.moveT;
	float y = GetY();// +Hover.y * Hover.moveT + Click.y * Click.moveT;
	renderer::GuiTransform(x,y);
	float red = GetRed();//r * (1 - Hover.fadeT)*(1 - Click.fadeT) + Hover.r*Hover.fadeT*(1 - Click.fadeT/2) + Click.r*Click.fadeT*(1 - Hover.fadeT/2);
	float green = g * (1 - Hover.fadeT)*(1 - Click.fadeT) + Hover.g*Hover.fadeT*(1 - Click.fadeT/2) + Click.g*Click.fadeT*(1 - Hover.fadeT/2);
	float blue = b * (1 - Hover.fadeT)*(1 - Click.fadeT) + Hover.b*Hover.fadeT*(1 - Click.fadeT/2) + Click.b*Click.fadeT*(1 - Hover.fadeT/2);
	float alpha = a * (1 - Hover.fadeT)*(1 - Click.fadeT) + Hover.a*Hover.fadeT*(1 - Click.fadeT/2) + Click.a*Click.fadeT*(1 - Hover.fadeT/2);
	renderer::GuiColor(red, green, blue, alpha);

	float w = GetW();// +Hover.w * Hover.sizeT + Click.w * Click.sizeT;
	float h = GetH();// +Hover.h * Hover.sizeT + Click.h * Click.sizeT;
	
	if (w != lastW || h != lastH) {
		UpdateCont(w, h);
		lastW = w;
		lastH = h;
	}

	renderer::BindTexture(texture);
	renderer::GuiSize(w, h);
	renderer::DrawRect();
	//cont.Draw();
	if (text.text.length() > 0) {
		text.SetPos(x, y);
		renderer::GuiSize(1, 1);
		text.DrawString(alpha, typing && selected);
	}
}
void IElem::Draw() {
	if (!HasTags()) return;
	DrawNormal();
}
void IElem::HoverEvent(int mx, int my) {
	if (!HasTags()) return;
	if (Inside(mx, my)) {
		hovering = true;
	} else {
		hovering = false;
	}
}
bool IElem::ClickEvent(int mx, int my, int action) {
	if (!HasTags()) return false;
	if (Inside(mx, my)) {
		clicked = action == 1;
		selected = true;
	} else {
		clicked = false;
		selected = false;
	}
	return clicked;
}
bool IElem::Inside(float mx, float my) {
	mx = renderer::AlterX(mx);
	my = renderer::AlterY(my);
	return mx > GetX()-GetW()/2&&mx<GetX() + GetW()/2 && my>GetY()-GetH()/2&&my < GetY() + GetH()/2;
}