#include "ObjectManager.h"

namespace oManager {
	ClientPlayer* player;
	ClientPlayer* GetPlayer() {
		return player;
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
	std::vector<GameObject*>* GetObjects() {
		return &gameObjects;
	}
	void Init() {
		player = new ClientPlayer();
		AddObject(player);
		player->SetCamera(renderer::GetCamera());
	}
}