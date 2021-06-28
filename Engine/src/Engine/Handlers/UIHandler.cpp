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
	//WorldItem* selectedWorld;
	//std::vector<WorldItem> worldItems;

	bool uiFadeBool = false;
	IElement* uiFade = nullptr;
	void UIFadeOn() {
		uiFadeBool = true;
	}
	void UIFadeOff() {
		uiFadeBool = false;
	}

	std::vector<IBase*> iBases;
	void NewBase(IBase* s) {
		iBases.push_back(s);
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
	IElement* NewElement(const std::string& name,int priority) {
		IElement* ie = new IElement(name, priority);
		elements.push_back(ie);
		std::vector<IElement*> tempElem = elements;
		elements.clear();
		while(tempElem.size()>0){
			int prio = 99999999;
			int ind = 0;
			for (int i = 0; i < tempElem.size();i++) {
				if (tempElem[i]->priority < prio) {
					prio = tempElem[i]->priority;
					ind = i;
				}
			}
			elements.push_back(tempElem[ind]);
			tempElem.erase(tempElem.begin() + ind);
		}
		return ie;
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
	void UpdateInterface(float delta) {
		//if (GetMenu() == Startup) {
		for (auto p : elements) {
			if (p->HasTags()) {
				p->HoverEvent(GetMouseX(), GetMouseY());
			}
		}
		for (auto p : iBases) {
			p->HoverEvent(GetMouseX(), GetMouseY());
		}
		for (auto p : elements) {
			if (p->HasTags()) {
				p->InternalUpdate(delta);
				p->Update(delta);
			}
		}
		for (auto p : iBases) {
			p->Update(delta);
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
					bug::out < elements[i]->name < bug::end;
				}
			}
		}
		if (keyEvent !=nullptr) {
			keyEvent(key,action);
		}
	}
	std::function<void(double, double, int, int,const std::string&)> mouseEvent;
	void MouseEvent(double mx, double my, int button, int action) {// B0 > left | B1 > right | b2 > mid
		//bug::outs < "MouseEvent" < mx < my < button < action < bug::end;
		for (auto p : iBases) {
			p->ClickEvent(mx,my,button, action);
		}
		std::string name = "";
		for (int i = elements.size() - 1; i < elements.size(); i--) {
			if (elements[i]->HasTags()) {
				if (elements[i]->ClickEvent(mx, my, button, action)) {
					name = elements[i]->name;
					//bug::out < "gone" <bug::end;
					break;
				}
			}
		}
		if (mouseEvent != nullptr) {
			mouseEvent(mx, my, button, action,name);
		}
	}
	std::function<void(double)> scrollEvent = nullptr;
	void ScrollEvent(double yoffset) {// B0 > left | B1 > right | b2 > mid
		//bug::outs < "ScrollEvent" < xoffset < yoffset < bug::end;
		for (auto p : iBases) {
			p->ScrollEvent(yoffset);
		}
		for (int i = elements.size() - 1; i < elements.size(); i--) {
			if (elements[i]->HasTags()) {
				if (elements[i]->ScrollEvent(yoffset)) {
					//break;
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
	void RenderInterface() {
		SwitchBlendDepth(true);
		BindShader(ShaderInterface);
		
		for (auto& p : elements) {
			if (p->HasTags()) {
				p->Render();
			}
		}
		for (auto p : iBases) {
			p->Render();
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
	void EditWorldFunc() {
		if (selectedWorld == nullptr)
			return;

		IElem* e = GetElement("worldName");
		e->SetText(selectedWorld->name);
	}
	void AddWorld(std::string name, std::string id) {
		WorldItem item(name, id);
		worldItems.push_back(item);

		IElem it(id);
		//it.AddTag(1);
		it.Dimi(575, 350 - (worldItems.size() - 1) * 110, 300, 100);
		it.Tex(0.3, 0.8, 8, 1);
		it.Text(name, &font1, 1, 1, 1, 1);
		IAction* a = &it.ClickEvent;
		a->Fade(0.2, 0.6, 0.6, 1, 0.1);
		a->Func(EditWorldFunc, 0.15);
		// Action?
		//AddElement(it);

	}
	void JoinWorldFunc() {
		if (selectedWorld == nullptr)
			return;

		std::cout << "Joining World " << selectedWorld->name << "..." << std::endl;
		/*
		Read data from world file and dump into entities, dimensions...
		*/
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

		//dManager::SetGameState(GameState::play);
	}
	void SaveWorldFunc() {
		if (selectedWorld == nullptr)
			return;


		IElem* e = GetElement("worldName");

		IElem* w = GetElement(selectedWorld->id);
		w->SetText(e->GetText());
		selectedWorld->name = e->GetText();


	}
	void AddWorldFunc() {

		IElem* e = GetElement("worldName");
		e->SetText("World Name");
	}
	void CreateWorldFunc() {
		IElem* e = GetElement("worldName");
		AddWorld(e->GetText(), SanitizeString(e->GetText()));

	}
	void DeleteWorldFunc() {
		if (selectedWorld == nullptr)
			return;

		// Delete WorldItem
		// Delete Save File
	}
	void MainMenuFunc() {

	}
	*/
	void InitInterface() {
		SetInterfaceCallbacks(KeyEvent, MouseEvent, ScrollEvent, DragEvent, ResizeEvent, FocusEvent);

		//font1.Data("verdana38");
		font1.Data("consolas42");

		uiFade = NewElement("uiFade",100);
		uiFade->Color({ 0, 0 });
		uiFade->conX.Center(0)->conY.Center(0)->conW.Center(1.f)->conH.Center(1.f);
		uiFade->NewTransition(&uiFadeBool)->Fade({ 0, 1 }, 1.f);

		SetPauseMode(true);
		/*
		IAction* a;

		//AddTag(0);
		IElem play("play");
		play.AddTag(0);
		play.Dimi(0, 0, 200, 100);
		play.Tex(0.3, 0.5, 0.8, 1);
		play.Text("Play", &font1, 0.7, 0.7, 0.7, 1);
		a = &play.ClickEvent;
		a->Fade(0.2, 0.3, 0.6, 1, 0.1);
		a->Func(PlayFunc, 0.15);
		AddElement(play);

		IElem wb("worldBack");
		wb.AddTag(1);
		wb.Dimi(575, 50, 400, 750);
		wb.Tex(1, 1, 1, 0.7);
		AddElement(wb);

		// LOAD WORLDS
		int err = 0;
		std::vector<std::string> worldList = fManager::ReadFileList("data/worlds", &err);
		for (std::string s : worldList) {
			std::vector<std::string> set = SplitString(s, " ");
			if (set.size() > 1) {
				AddWorld(set[0], SanitizeString(set[1]));
			}
		}

		IElem wa("worldAdd");
		wa.AddTag(1);
		wa.Dimi(575, -400, 300, 100);
		wa.Tex(0.8, 0.8, 0.8, 1);
		wa.Text("Add World", &font1, 0.5, 0.5, 0.5, 1);
		a = &wa.ClickEvent;
		a->Fade(0.7, 0.7, 0.7, 1, 0.1);
		a->Func(AddWorldFunc, 0.15);
		AddElement(wa);

		IElem mb("middleBack");
		mb.AddTag(2);
		mb.AddTag(3);
		mb.Dimi(0, 50, 600, 750);
		mb.Tex(1, 1, 1, 0.7);
		AddElement(mb);

		IElem bm("backToMenu");
		bm.AddTag(1);
		bm.Dimi(0, -380, 300, 100);
		bm.Tex(1, 1, 1, 0.7);
		bm.Text("Back", &font1, 0.2, 0.2, 0.2, 1);
		a = &bm.ClickEvent;
		a->Fade(0.8, 0.8, 0.8, 0.7, 0.1);
		a->Func(MainMenuFunc, 0.15);
		AddElement(bm);

		IElem wn("worldName");
		wn.AddTag(2);
		wn.AddTag(3);
		wn.Dimi(0, 350, 300, 100);
		wn.Tex(1, 1, 1, 1);
		wn.Text(20, &font1, 0.2, 0.3, 0.3, 1);
		wn.Typing();
		AddElement(wn);

		IElem ws("worldSettings");
		ws.AddTag(2);
		ws.Dimi(0, 100, 500, 300);
		ws.Tex(1, 1, 1, 1);
		ws.Text(100, &font1, 0.8, 0.3, 0.3, 1);
		ws.Typing();
		AddElement(ws);

		IElem wc("worldCreate");
		wc.AddTag(2);
		wc.Dimi(0, -250, 400, 100);
		wc.Tex(1, 1, 1, 1);
		wc.Text("Create World", &font1, 0.3, 0.3, 0.3, 1);
		a = &wc.ClickEvent;
		a->Fade(0.8, 0.8, 0.8, 1, 0.1);
		a->Func(CreateWorldFunc, 0.15);
		AddElement(wc);

		IElem wj("worldJoin");
		wj.AddTag(3);
		wj.Dimi(0, -10, 400, 100);
		wj.Tex(1, 1, 1, 1);
		wj.Text("Join World", &font1, 0.3, 0.3, 0.3, 1);
		a = &wj.ClickEvent;
		a->Fade(0.8, 0.8, 0.8, 1, 0.1);
		a->Func(JoinWorldFunc, 0.15);
		AddElement(wj);

		IElem wsa("worldSave");
		wsa.AddTag(3);
		wsa.Dimi(0, -130, 400, 100);
		wsa.Tex(1, 1, 1, 1);
		wsa.Text("Save World", &font1, 0.3, 0.3, 0.3, 1);
		a = &wsa.ClickEvent;
		a->Fade(0.8, 0.8, 0.8, 1, 0.1);
		a->Func(SaveWorldFunc, 0.15);
		AddElement(wsa);

		IElem wd("worldDelete");
		wd.AddTag(3);
		wd.Dimi(0, -250, 400, 100);
		wd.Tex(1, 1, 1, 1);
		wd.Text("Delete World", &font1, 0.3, 0.3, 0.3, 1);
		a = &wd.ClickEvent;
		a->Fade(0.8, 0.8, 0.8, 1, 0.1);
		a->Func(DeleteWorldFunc, 0.15);
		AddElement(wd);

		selectedWorld = &worldItems.at(0);
		JoinWorldFunc();
		*/
	}
}
