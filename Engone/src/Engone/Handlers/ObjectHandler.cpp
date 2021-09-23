#include "gonpch.h"

#include "ObjectHandler.h"

namespace engone {

	std::unordered_map<std::string, Dimension> dimensions;
	Dimension* loadedDim = nullptr;
	void AddDimension(const std::string& s, Dimension dim) {
		dimensions[s] = dim;
	}
	Dimension* GetDimension() {
		return loadedDim;
	}
	void SetDimension(const std::string& s) {
		loadedDim = &dimensions[s];
	}
	
	Camera* engine_camera=new Camera();
	Camera* GetCamera() {
		return engine_camera;
	}

	std::vector<GameObject*> gameObjects;
	GameObject* AddObject(GameObject* object) {
		gameObjects.push_back(object);
		return object;
	}

	void DelObject(GameObject* object) {
		for (int i = 0; i < gameObjects.size(); i++) {
			if (gameObjects[i] == object) {
				gameObjects.erase(gameObjects.begin() + i);
				break;
			}
		}
	}
	void DelObject(const std::string& name) {
		for (int i = 0; i < gameObjects.size(); i++) {
			if (gameObjects[i]->GetName() == name) {
				gameObjects.erase(gameObjects.begin() + i);
				break;
			}
		}
	}
	std::vector<GameObject*>& GetObjects() {
		return gameObjects;
	}
	GameObject* GetObjectByName(const std::string& name) {
		for (GameObject* n : gameObjects) {
			if (n->name == name) {
				return n;
			}
		}
		return nullptr;
	}
}