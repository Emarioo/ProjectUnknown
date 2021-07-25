#include "UIHandler.h"

namespace engine {

	bool pauseMode=false;
	bool GetPauseMode() {
		return pauseMode;
	}
	bool* GetPauseModeRef() {
		return &pauseMode;
	}
	/*
	Escape pause menu off or on
	*/
	void SetPauseMode(bool f) {
		pauseMode = f;
		SetCursorMode(f);
	}
	// Events
	double mouseX, mouseY;
	void FocusEvent(int focus) {
		if (!focus) {
			SetPauseMode(true);
		}
	}
	Font font1;
	Font* GetFont() {
		return &font1;
	}

	std::vector<IBase*> iBases;
	void AddBase(IBase* s) {
		iBases.push_back(s);
		std::vector<IBase*> tempBase = iBases;
		iBases.clear();
		while (tempBase.size() > 0) {
			int prio = 99999999;
			int ind = 0;
			for (int i = 0; i < tempBase.size(); i++) {
				if (tempBase[i]->priority < prio) {
					prio = tempBase[i]->priority;
					ind = i;
				}
			}
			iBases.push_back(tempBase[ind]);
			tempBase.erase(tempBase.begin() + ind);
		}
	}
	IBase* GetBase(const std::string& s) {
		for (IBase* o : iBases) {
			if (o->name == s) {
				return o;
			}
		}
		return nullptr;
	}

	std::vector<IElement*> elements;
	IElement* AddElement(const std::string& name,int priority) {
		return AddElement(new IElement(name,priority));
	}
	IElement* AddElement(IElement* element){
		elements.push_back(element);
		std::vector<IElement*> tempElem = elements;
		elements.clear();
		while (tempElem.size() > 0) {
			int prio = 99999999;
			int ind = 0;
			for (int i = 0; i < tempElem.size(); i++) {
				if (tempElem[i]->priority < prio) {
					prio = tempElem[i]->priority;
					ind = i;
				}
			}
			elements.push_back(tempElem[ind]);
			tempElem.erase(tempElem.begin() + ind);
		}
		return element;
	}
	IElement* GetElement(const std::string& s) {
		for (IElement* e : elements) {
			if (e->name == s) {
				return e;
			}
		}
		return nullptr;
		/* Unordered List
		if (elements.count(s) > 0)
			return elements[s];
		return nullptr;
		*/
	}
	void UpdateInterface(double delta) {
		//if (GetMenu() == Startup) {
		for (auto p : elements) {
			if (p->IsActive()) {
				if (p->HasTags()) {
					p->HoverEvent(GetMouseX(), GetMouseY());
				}
			}
		}
		for (auto p : iBases) {
			if (p->IsActive())
				p->HoverEvent(GetMouseX(), GetMouseY());
		}
		for (auto p : elements) {
			if (p->IsActive()) {
				if (p->HasTags()) {
					p->InternalUpdate(delta);
					p->Update(delta);
				}
			}
		}
		for (auto p : iBases) {
			if (p->IsActive()) {
				p->Update(delta);
			}
		}
		//}
	}
	std::function<void(int, int)> keyEvent = nullptr;
	void KeyEvent(int key, int action) {
		//bug::outs < "KeyEvent" < key < action < bug::end;
		
		for (auto p : iBases) {
			p->KeyEvent(key,action);
		}
		for (int i = elements.size() - 1; i < elements.size(); i--) {
			if (elements[i]->HasTags()) {
				if (elements[i]->KeyEvent(key, action)) {
					//bug::out < elements[i]->name < bug::end;
				}
			}
		}
		if (keyEvent !=nullptr) {
			keyEvent(key,action);
		}
	}
	std::function<void(double, double, int, int,const std::string&)> mouseEvent;
	void MouseEvent(double mx, double my, int action, int button) {// B0 > left | B1 > right | b2 > mid
		//bug::outs < "MouseEvent" < mx < my < button < action < bug::end;
		for (auto p : iBases) {
			if (p->IsActive())
				p->MouseEvent(mx,my,action, button);
		}
		std::string name = "";
		for (int i = elements.size() - 1; i < elements.size(); i--) {
			if (elements[i]->IsActive()) {
				if (elements[i]->HasTags()) {
					if (elements[i]->MouseEvent(mx, my, action, button)) {
						name = elements[i]->name;
						//bug::out < "gone" <bug::end;
						break;
					}
				}
			}
		}
		if (mouseEvent != nullptr) {
			mouseEvent(mx, my, action, button,name);
		}
	}
	std::function<void(double)> scrollEvent = nullptr;
	void ScrollEvent(double yoffset) {// B0 > left | B1 > right | b2 > mid
		//bug::outs < "ScrollEvent" < xoffset < yoffset < bug::end;
		for (auto p : iBases) {
			if (p->IsActive())
				p->ScrollEvent(yoffset);
		}
		for (int i = elements.size() - 1; i < elements.size(); i--) {
			if(elements[i]->IsActive()){
				if (elements[i]->HasTags()) {
					if (elements[i]->ScrollEvent(yoffset)) {
						//break;
					}
				}
			}
		}
	}
	std::function<void(double, double)> dragEvent = nullptr;
	void DragEvent(double mx, double my) {
		//bug::outs < "DragEvent" < mx < my < bug::end;
		//if (GetMenu() == Startup) {
		/* Moved to update function
		for (int i = elements.size() - 1; i < elements.size(); i--) {
			if (elements[i]->HasTags()) {
				if (elements[i]->HoverEvent(mx, my)) {

				}
			}
		}*/
		
		//}
	}
	void ResizeEvent(int width, int height) {
		//bug::outs < "ResizeEvent" < width < height < std::endl;
		/*for (int i = elements.size() - 1; i < elements.size(); i--) {
			elements[i]->Resize(width, height); // NOT USED
		}*/
	}
	void RenderInterface(double delta) {
		SwitchBlendDepth(true);
		BindShader(ShaderInterface);
		
		for (auto p : elements) {
			if (p->IsActive()) {
				if (p->HasTags()) {
					p->PreRender();
					p->Render();
				}
			}
		}
		for (auto p : iBases) {
			if (p->IsActive()) {
				p->PreRender();
				p->Render();
			}
		}
		
		GuiColor(0.9, 0.8, 0.7, 1);
		GuiTransform(-1+0.01, -1+0.06);
		GuiSize(1, 1);
		glm::vec3 pos = GetCamera()->position;
		glm::vec3 rot = GetCamera()->rotation;
		
		DrawString(&font1,
			std::to_string(pos.x) + " " + std::to_string(pos.y) + " " + std::to_string(pos.z) + "\n" +
			std::to_string(rot.x) + " " + std::to_string(rot.y) + " " + std::to_string(rot.z),
			false,0.05,-1);
	}
	void GetEventCallbacks(
		std::function<void(int, int)> key,
		std::function<void(double, double, int, int,const std::string&)> mouse,
		std::function<void(double)> scroll,
		std::function<void(double, double)> drag) {
		keyEvent = key;
		mouseEvent = mouse;
		scrollEvent = scroll;
		dragEvent = drag;
	}
		/*
		Dimension dim;
		float a = 5*GetOptionf("scale")/500;
		Biome waterD(0.3, -6*a, 2*a);
		Biome waterS(0.4, 2*a, 6*a);
		Biome sand(0.45, 6*a, 6.6*a);
		Biome grass(0.55, 6.65*a, 7.2*a);
		Biome grass2(0.6, 7.2*a, 8*a);
		Biome rock(0.7, 8*a, 13*a);
		Biome rock2(0.9, 13*a, 25*a);
		Biome snow(1, 25*a, 28*a);

		a = 10;
		waterD.ColorD(10,a, 67,a, 255,a);
		waterS.ColorD(13,a, 95,a, 255,a);
		sand.ColorD(255,a, 236,a, 159,a);
		grass.ColorD(74,a, 190,a, 31,a);
		grass2.ColorD(60,a ,165,a, 49,a);
		rock.ColorD(99,a, 65,a, 49,a);
		rock2.ColorD(75,a, 48,a, 18,a);
		snow.ColorD(240,a, 253,a, 255,a);

		waterD.Flat(0.4,1,0.1,0.3);

		Biome gray(1, 0, 0);

		Biome dwater(0.2, 0, 5);
		Biome water(0.35, 5, 10);
		Biome sandy(0.5, 10, 15);
		Biome plains(0.7, 15,20);
		Biome mount(0.8, 20,25);
		Biome snowy(1, 25,30);

		gray.Color(0, 1, 0, 1, 0, 1);

		dwater.Color(10,67,255);
		water.Color(13,95,255);
		sandy.Color(255,236,159);
		plains.Color(74,190,31);
		mount.Color(60,55,18);
		snowy.Color(240,253,255);

		dim.AddBiome(dwater);
		dim.AddBiome(water);
		dim.AddBiome(sandy);
		dim.AddBiome(plains);
		dim.AddBiome(mount);
		dim.AddBiome(snowy);
		/*dim.AddBiome(waterD);
		dim.AddBiome(waterS);
		dim.AddBiome(sand);
		dim.AddBiome(grass);
		dim.AddBiome(grass2);
		dim.AddBiome(rock);
		dim.AddBiome(rock2);
		dim.AddBiome(snow);*//*
		//dim.AddBiome(gray);
		dim.Init(&GetPlayer()->position);
		AddDimension("classic",dim);
		SetDimension("classic");
		*/
	void InitInterface() {
		SetInterfaceCallbacks(KeyEvent, MouseEvent, ScrollEvent, DragEvent, ResizeEvent, FocusEvent);

		//font1.Data("verdana38");
		font1.Data("consolas42");

		SetPauseMode(true);
	}
}
