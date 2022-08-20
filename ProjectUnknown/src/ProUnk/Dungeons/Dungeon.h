#pragma once

class Dungeon {

	Dungeon();
	// vector<GameObject> content
	// vector<GameObject> enemies
	void Render();
	/*
		render visible content (corridors,rooms)
		render enemies
	*/
	void Update(float delta);
	/*
		Depending on where the players is, new rooms and enemies should be loaded
		as the player explores the dungeon.
		Unload the dungeon when the player walks out.
	*/
};