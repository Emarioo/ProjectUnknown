#include "IElem.h"

void DimFormat(std::string s, float* out) { // TODO: IElem Dim check for errors in format before they happen
	std::vector<std::string> split = SplitString(s, ",");
	if (split.size() == 4) {
			if (split[0].back() == 'p') {
				split[0].pop_back();
				out[0] = 2.f * atoi(split[0].c_str()) / 1920.f;
			} else {
				split[0].pop_back();
				out[0] = atof(split[0].c_str());
			}
			if (split[1].back() == 'p') {
				split[1].pop_back();
				out[1] = 2.f * atoi(split[1].c_str()) / 1080.f;
			} else {
				split[1].pop_back();
				out[1] = atof(split[1].c_str());
			}
			if (split[2].back() == 'p') {
				split[2].pop_back();
				out[2] = 2.f * atoi(split[2].c_str()) / 1920.f;
			} else {
				split[2].pop_back();
				out[2] = atof(split[2].c_str());
			}
			if (split[3].back() == 'p') {
				split[3].pop_back();
				out[3] = 2.f * atoi(split[3].c_str()) / 1080.f;
			} else {
				split[3].pop_back();
				out[3] = atof(split[3].c_str());
			}
	} else {
		bug::out < bug::RED < "Error in Element Format: " < s < bug::end;
		out[0] = 0;
		out[1] = 0;
		out[2] = 0;
		out[3] = 0;
	}
}
void Dim2Format(std::string s, float* out) { // TODO: IElem Dim check for errors in format before they happen
	std::vector<std::string> split = SplitString(s, ",");
	if (split.size() == 2) {
		if (split[0].back() == 'p') {
			split[0].pop_back();
			out[0] = 2.f * atoi(split[0].c_str()) / 1920.f;
		} else {
			split[0].pop_back();
			out[0] = atof(split[0].c_str());
		}
		if (split[1].back() == 'p') {
			split[1].pop_back();
			out[1] = 2.f * atoi(split[1].c_str()) / 1080.f;
		} else {
			split[1].pop_back();
			out[1] = atof(split[1].c_str());
		}
	} else {
		bug::out < bug::RED < "Error in Element Format: " < s < bug::end;
		out[0] = 0;
		out[1] = 0;
	}
}
void IAction::Func(std::function<void()> f, float time) {
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
	Dim2Format(xy, arr);
	x = arr[0];
	y = arr[1];
	moveS = 1 / time;
}
void IAction::Size(const std::string& wh, float time) {
	size = true;
	float arr[2];
	Dim2Format(wh, arr);
	w = arr[0];
	h = arr[1];
	sizeS = 1 / time;
}
void IAction::Update(float delta, bool b) {
	if (func != nullptr) {
		if (b || funcT > 0)
			funcT += funcS * delta;
		if (funcT >= 1 && funcT < 1 + funcS * delta)
			func();
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
	text.ElemWH(GetW(), GetH());
}
void IElem::UpdateCont(float w,float h) {
	float tw = w / 2;
	float th = h / 2;

	float v[]{
		-tw,-th,0,0,
		-tw,th,0,1,
		tw,th,1,1,
		tw,-th,1,0
	};
	cont.SubVB(0, 4 * 4, v);
	text.ElemWH(GetW(), GetH());
}


void IElem::Text(const std::string& s, Font* f) {
	text.Setup(f, s.size(), true);
	text.SetText(s);
}
void IElem::Text(const std::string& s, Font* f, float f0, float f1, float f2, float f3) {
	text.Setup(f, s.size(), true);
	text.SetCol(f0, f1, f2, f3);
	text.SetText(s);
}
void IElem::Text(int max, Font* f) {
	text.Setup(f, max, true);
}
void IElem::Text(int max, const std::string& s, Font* f) {
	text.Setup(f, max, true);
	text.SetText(s);
}
void IElem::Text(int max, Font* f, float f0, float f1, float f2, float f3) {
	text.Setup(f, max, true);
	text.SetCol(f0, f1, f2, f3);
}
void IElem::Text(int max, const std::string& s, Font* f, float f0, float f1, float f2, float f3) {
	text.Setup(f, max, true);
	text.SetCol(f0, f1, f2, f3);
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
	Hover.Update(delta,hovering);
	Click.Update(delta,clicked);
	for (auto& p : customActions) {
		//bug::outs < *p.first < bug::end;
		p.second.Update(delta,*p.first);
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
float lastW=0, lastH=0;
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
	//bug::outs < Hover.w < Hover.sizeT < bug::end;
	if (w != lastW||h!=lastH) {
		UpdateCont(w,h);
		lastW = w;
		lastH = h;
	}
	//bug::outs < name < bug::end;
	//bug::out +red+" "+green+" "+blue+" "+alpha+" "+GetX()+" "+GetY()+ bug::end;
	renderer::BindTexture(texture);
	cont.Draw();
	if (text.text.length() > 0) {
		text.SetPos(x, y);
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