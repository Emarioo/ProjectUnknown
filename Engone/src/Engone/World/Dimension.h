#pragma once

#include "../Rendering/Buffer.h"

#include "../Utility/GameOptions.h"

#include "Biome.h"
#include "../Objects/GameObject.h"

namespace engine {

	class Chunk {
	public:
		Chunk(int x, int z) {
			this->x = x;
			this->z = z;
		}
		int x, z;
		TriangleBuffer con;
	};

	class Dimension {
	public:
		Dimension();
		Dimension(int s);
		int seed = 0;
		int* randomMap;
		int randomMapSize = 10;

		GameObject* player;
		/*
		Minimum of 2
		*/
		float viewDistance = 15;

		// float chunkDetail = 1;
		int chunkSize = 20;
		/*
		Set in Init() and is always (chunkSize+1)/chunkDetail
		*/
		int chunkPoints = 0;
		// Map used in updateChunk
		float* vMap;
		std::vector<Chunk> loadedChunks;
		std::vector<Biome> biomes;

		void Init(GameObject* player);

		void UpdateAllChunks();
		void CleanChunks(); // Reload chunks
		void UpdateChunk(Chunk* chunk);

		void GetBiome(float* v);
		float GetNoise(float x, float y);

		void SetRandomMap(int seed);
		int GetRandom(int x, int y);

		/*
		Terrain Height at player;
		*/
		float TerHeightAtPlayer();

		void AddBiome(Biome b);

		// GetMonsterAreas()
		// GetCities()
		// GetDungeons()
		// GetRuins
		// GetPathMap

	};
	/*
	Dimensions control the layout of the world
	  Terrain, Mountains, Cities, Ruins, Dungeons, Monster Spawning Areas, Monster Movement(AI pathfinding)
	  Interactable objects are stored in Dimension and updated in management. Like monsters, players, chests, workshops
	  Minerals and materials from the world is stored in Dimension?
	  Dimension need a network to the server
	 Creating A dimension is done by giving a function a few parameters like
	  Seed, World Settings, Pathfinding layout,

	  World Settings:
	   Rarity of biomes: Mountains, Deep forest, Ocean, Islands
	   Color shading
	   Day night
	   weather: rain, fog, clouds

	// The dimension consists of loaded chunks which are being loaded and

	*/
}