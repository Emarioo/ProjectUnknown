#include "RefManager.h"

namespace rManager {

	Menu currentMenu = Startup;
	Menu GetMenu() {
		return currentMenu;
	}
	void SetMenu(Menu m) {
		currentMenu = m;
	}

	ClientPlayer player;
	ClientPlayer& GetPlayer() {
		return player;
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
	std::vector<GameObject> gameObjects;
	void AddObject(GameObject& object) {
		gameObjects.push_back(object);
	}
	void DelObject(GameObject& object) {
		for (int i = 0; i < gameObjects.size(); i++) {
			if (&gameObjects[i] == &object) {
				gameObjects.erase(gameObjects.begin() + i);
				break;
			}
		}
	}
	void DelObject(const std::string& name) {
		for (int i = 0; i < gameObjects.size(); i++) {
			if (gameObjects[i].GetName() == name) {
				gameObjects.erase(gameObjects.begin() + i);
				break;
			}
		}
	}
	/*
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
	std::vector<GameObject&> texRender;
	std::vector<GameObject&> colorRender;
	void AddUpdateO(std::string, GameObject&);
	void DelUpdateO(std::string, GameObject&);
	*/
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
	/*
	Have a seperate list for objects which should be rendered?
	*/
	void RenderGame() {
		renderer::SwitchBlendDepth(false);
		renderer::BindShader(MaterialType::ColorMat);
		for (GameObject& o : gameObjects) {
			if (o.renderMesh) {
				if (o.meshData != nullptr) {
					if (o.meshData->material == MaterialType::ColorMat) {
						o.Draw();
					}
				}
			}
		}
		renderer::BindShader(MaterialType::TextureMat);
		for (GameObject& o : gameObjects) {
			if (o.renderMesh) {
				if(o.meshData!=nullptr) {
					if (o.meshData->material == MaterialType::TextureMat) {
						o.Draw();
					}
				}
			}
			
		}
		renderer::BindShader(MaterialType::OutlineMat);
		glLineWidth(2.f);
		for (GameObject& o : gameObjects) {
			if(o.renderHitbox) {
				//if (o.collision.) { TODO: Check if collision has draw object?
					o.collision.Draw();
				//}
			}
		}
		if (GetPlayer().renderHitbox/*&&GetPlayer()->freeCam*/) {
			GetPlayer().collision.Draw();
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
	void Init() {
		player.SetCamera(GetCamera());
	}
}