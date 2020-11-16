#include "RefsManager.h"

Menu currentMenu=Startup;
Menu GetMenu() {
	return currentMenu;
}
void SetMenu(Menu m) {
	currentMenu = m;
}

ClientPlayer player;
ClientPlayer* GetPlayer() {
	return &player;
}

/*
std::map<std::string,Dimension> dimensions;
Dimension* loadedDim = nullptr;
void AddDimension(std::string s,Dimension dim) {
	dimensions[s]=dim;
}
Dimension* GetDimension() {
	return loadedDim;
}
void SetDimension(std::string s) {
	loadedDim = &dimensions[s];
}
*/
std::vector<GameObject*> texRender;
std::vector<GameObject*> colorRender;
void AddRenderO(std::string s, GameObject* o) {
	if (s == "tex") {
		texRender.push_back(o);
	} else if (s == "color") {
		colorRender.push_back(o);
	}
}
void DelRenderO(std::string s, GameObject* o) {
	if (s == "tex") {
		for (int i = 0; i < texRender.size(); i++) {
			if (texRender[i] == o) {
				texRender.erase(texRender.begin() + i);
				break;
			}
		}
	} else if (s == "color") {
		for (int i = 0; i < colorRender.size(); i++) {
			if (colorRender[i] == o) {
				colorRender.erase(colorRender.begin() + i);
				break;
			}
		}
	}
}
/*
std::vector<IElem> elements;
std::vector<IElem>* GetElements() {
	return &elements;
}
IElem* AddElement() {
	elements.push_back(IElem());
	return &elements.at(elements.size()-1);
}
void DelElement(IElem* e) {
	/*for (int i = 0; i < elements.size(); i++) {
		if (elements[i] == e) {
			elements.erase(elements.begin() + i);
			break;
		}
	}
}
*/
void RenderGame() {
	SwitchBlendDepth(false);
	BindShader("tex");
	for (GameObject* o : texRender) {
		if (o->renderMesh) {
			o->Draw();
		}
	}
	BindShader("color");
	for (GameObject* o : colorRender) {
		if (o->renderMesh) {
			o->Draw();
		}
	}
	BindShader("outline");
	glLineWidth(2.f);
	for (GameObject* o : colorRender) {
		if (o->renderHitbox) {
			o->collision.Draw();
		}
	}
	if (GetPlayer()->renderHitbox/*&&GetPlayer()->freeCam*/) {
		GetPlayer()->collision.Draw();
	}
	/*
	for (GameObject* o : texRender) {
		o->Draw();
	}*/

	/*
	BindShader("terrain");
	if (loadedDim != nullptr) {
		BindTexture("blank");
		for (Chunk c : loadedDim->loadedChunks) {
			Location base;
			base.Translate(glm::vec3(c.x*(loadedDim->chunkSize), 0, c.z*(loadedDim->chunkSize)));
			ObjectTransform(base.mat());
			c.con.Draw();
		}
	}*/
}
/*
void UpdateInterface() {
	for (int i = 0; i < elements.size();i++) {
		elements.at(i).Update();
	}
}
void RenderInterface() {
	SwitchBlendDepth(true);
	BindShader("gui");

	for (int i = 0; i < elements.size(); i++) {
		elements.at(i).Draw();
	}
}
*/
void InitManager() {
	player.SetCamera(GetCamera());
}