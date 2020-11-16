#pragma once

#include "MagicEditor.h"

#include "Objects/ClientPlayer.h"

// This class handles the updating, gameloop

class Gamecore {
public:
	Gamecore();
	void Update(float delta);
	void Render();
	std::vector<GameObject*> updateObjects;
	void AddUpdateO(GameObject*);
	void DelUpdateO(GameObject*);
	void LoadGameData();
	
	int GetTime();

private:

};