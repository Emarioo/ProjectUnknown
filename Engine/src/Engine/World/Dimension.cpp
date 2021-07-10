#include "Dimension.h"
#include <random>
#include <iostream>
#include "../Utility/Utilities.h"

namespace engine {

	Dimension::Dimension() {}
	Dimension::Dimension(int s) {
		seed = s;
	}

	std::uniform_int_distribution<> distr(0, 100000);
	std::mt19937 randomSeed;
	void Dimension::SetRandomMap(int seed) {
		randomSeed = std::mt19937(seed);
		for (int i = 0; i < randomMapSize; i++) {
			for (int j = 0; j < randomMapSize; j++) {
				randomMap[i * randomMapSize + j] = distr(randomSeed);
			}
		}
	}
	int Dimension::GetRandom(int x, int z) {// TODO: NICE RANDOM THING
		int ind = 0;
		/*
		Earth Map: -2 * x + z * z - z;

		*/
		int sum = -5 * x*x*x + z * z - z;

		ind = (int)(sum) % ((int)pow(randomMapSize, 2));
		if (ind < 0)
			ind *= -1;
		//ind = (x%randomMapSize) + (z%randomMapSize)*randomMapSize;
		int val = randomMap[ind];
		//std::cout << x << " "<<z<<" "<<ind<<" "<<val <<" : "<< one << " "<<two << std::endl;
		return val;
	}
	int tick = 0;
	void Dimension::CleanChunks() {
		int px = player->position.x / chunkSize, pz = player->position.z / chunkSize;
		int befTime = engine::GetTime();
		for (int i = 0; i < loadedChunks.size(); i++) {
			Chunk* c = &loadedChunks.at(i);
			float dx = c->x - px;
			float dz = c->z - pz;

			/*if (abs(dx) > viewDistance|| abs(dz) > viewDistance) {
				std::cout << "Change " << c->x << " " << c->z << " diff "<<dx << " "<<dz<< std::endl;

				c->x -= 2 * dx -abs(dx) / dx;

				c->z -= 2 * dz-abs(dz) / dz;

				UpdateChunk(c);
			} else */

			if (abs(dx) > viewDistance) {
				c->x -= 2 * dx - abs(dx) / dx;
				UpdateChunk(c);
			} else if (abs(dz) > viewDistance) {
				c->z -= 2 * dz - abs(dz) / dz;
				UpdateChunk(c);
			}
			if (engine::GetTime()-befTime>40) {
				break;
			}
		}
		//int aftTime = engine::GetTime();
		//bug::out < (aftTime - befTime) < bug::end;
		/*
		for each chunk
			if chunk is outside player range
				Set pos to opposite side of player range
				Update chunk mesh to new position
		*/
	}
	void Dimension::UpdateAllChunks() {
		for (Chunk c : loadedChunks) {
			UpdateChunk(&c);
		}
	}

	/*
	Add Biomes before INIT
	*/
	void Dimension::Init(GameObject* player) {
		chunkPoints = chunkSize + 1;
		randomMap = new int[pow(randomMapSize, 2)];
		vMap = new float[pow(chunkPoints, 2) * 8];
		SetRandomMap(seed);
		this->player = player;

		int chunkArea = viewDistance;

		for (int z = -chunkArea; z < chunkArea + 0.5; z++) {
			for (int x = -chunkArea; x < chunkArea + 0.5; x++) {
				//if (sqrt(pow(x, 2) + pow(z, 2)) > chunkArea) continue; // For Circle

				Chunk c(x, z);

				int iw = chunkPoints - 1;
				unsigned int* iMap = new unsigned int[pow(iw, 2) * 6];
				for (int i = 0; i < iw; i++) {
					for (int j = 0; j < iw; j++) {
						if (((1 + i) % 2 && j % 2) || (i % 2 && (j + 1) % 2)) {// Spike/Square
							iMap[(i * iw + j) * 6 + 0] = chunkPoints * (i)+(j);
							iMap[(i * iw + j) * 6 + 1] = chunkPoints * (i + 1) + (j);
							iMap[(i * iw + j) * 6 + 2] = chunkPoints * (i)+(j + 1);
							iMap[(i * iw + j) * 6 + 3] = chunkPoints * (i)+(j + 1);
							iMap[(i * iw + j) * 6 + 4] = chunkPoints * (i + 1) + (j);
							iMap[(i * iw + j) * 6 + 5] = chunkPoints * (i + 1) + (j + 1);
						} else {
							iMap[(i * iw + j) * 6 + 0] = chunkPoints * (i)+(j);
							iMap[(i * iw + j) * 6 + 1] = chunkPoints * (i + 1) + (j);
							iMap[(i * iw + j) * 6 + 2] = chunkPoints * (i + 1) + (j + 1);
							iMap[(i * iw + j) * 6 + 3] = chunkPoints * (i)+(j);
							iMap[(i * iw + j) * 6 + 4] = chunkPoints * (i + 1) + (j + 1);
							iMap[(i * iw + j) * 6 + 5] = chunkPoints * (i)+(j + 1);
						}
					}
				}
				c.con.Setup(true, nullptr, pow(chunkPoints, 2) * 8, iMap, pow(iw, 2) * 6);
				c.con.SetAttrib(0, 3, 8, 0);
				c.con.SetAttrib(1, 3, 8, 3);
				c.con.SetAttrib(2, 2, 8, 6);

				UpdateChunk(&c);
				loadedChunks.push_back(c);
				delete iMap;
			}
		}
	}
	/*
	 Done after cordinates have been changed.
	*/
	void Dimension::UpdateChunk(Chunk* chunk) {
		//float* vMap = new float[pow(chunkPoints, 2) * 8];

		for (int z = 0; z < chunkPoints; z++) {
			for (int x = 0; x < chunkPoints; x++) {
				float v[]{ (int)(chunk->x * chunkSize + x - chunkSize / 2),0,(int)(chunk->z * chunkSize + z - chunkSize / 2),1,1,1 };
				GetBiome(v);
				//v[1] = GetRandom((int)v[0], (int)v[2]) / 100000.f;
				vMap[(z * chunkPoints + x) * 8 + 0] = v[0] - chunk->x * chunkSize;
				vMap[(z * chunkPoints + x) * 8 + 1] = v[1];
				vMap[(z * chunkPoints + x) * 8 + 2] = v[2] - chunk->z * chunkSize;
				
				vMap[(z * chunkPoints + x) * 8 + 3] = v[3];
				vMap[(z * chunkPoints + x) * 8 + 4] = v[4];
				vMap[(z * chunkPoints + x) * 8 + 5] = v[5];
				vMap[(z * chunkPoints + x) * 8 + 6] = x % 2;
				vMap[(z * chunkPoints + x) * 8 + 7] = z % 2;
				
				// TODO: Set normals
			}
		}
		chunk->con.SubVB(0, pow(chunkPoints, 2) * 8, vMap);
		//delete vMap; vMap is reused
	}
	void Dimension::GetBiome(float* v) {
		// pers = 0.5, lacu=2, oct=5, scale=500

		float persistance = GetOptionf("persistance"); // Spikyness Higher value is smooth while lower value is spiky
		float lacunarity = GetOptionf("lacunarity");
		int octaves = GetOptioni("octaves");
		float scale = GetOptionf("scale");
		//std::cout << scale << " " << persistance << " "<<lacunarity << " "<<octaves<<" " << std::endl;

		float maxHeight = 0;
		float amplT = 1;
		for (int i = 0; i < octaves; i++) {
			maxHeight += amplT;
			amplT *= persistance;
		}
		float amplitude = 1;
		float frequency = 1;
		float totalNoise = 0;
		for (int o = 0; o < octaves; o++) {
			float noise = GetNoise(v[0] / scale * frequency, v[2] / scale * frequency);

			totalNoise += noise * amplitude;
			amplitude *= persistance;
			frequency *= lacunarity;
		}
		totalNoise /= maxHeight * 100000;

		/*amplitude = 1;
		frequency = 1;
		float totalNoise2 = 0;
		for (int o = 0; o < octaves; o++) {
			float noise = GetNoise(v[2] / scale * frequency, v[0] / scale * frequency);

			totalNoise2 += noise * amplitude;
			amplitude *= persistance;
			frequency *= lacunarity;
		}
		totalNoise2 /= maxHeight * 100000;

		totalNoise = (totalNoise + totalNoise2);*/

		// Get Biome from totalnoise
		float r0 = 0;
		for (Biome b : biomes) {
			if (b.Biomify(r0, totalNoise, v)) {
				break;
			}
			r0 = b.r;
		}
		//bug::outs < v[3] < v[4] < v[5] < bug::end;
	}
	float Dimension::TerHeightAtPlayer() {
		float v[]{ (int)(player->position.x),0,(int)(player->position.z),1,1,1 };
		GetBiome(v);
		return v[1];
	}
	void Dimension::AddBiome(Biome b) {
		biomes.push_back(b);
	}
	float Dimension::GetNoise(float x, float y) {
		int nSampleX1 = (int)x;
		int nSampleY1 = (int)y;

		int nSampleX2, nSampleY2;
		float fBlendX, fBlendY;

		if (x < 0) {
			nSampleX2 = (nSampleX1 - 1);
			fBlendX = (float)(nSampleX1 - x);
		} else {
			nSampleX2 = (nSampleX1 + 1);
			fBlendX = (float)(x - nSampleX1);
		}
		if (y < 0) {
			nSampleY2 = (nSampleY1 - 1);
			fBlendY = (float)(nSampleY1 - y);
		} else {
			nSampleY2 = (nSampleY1 + 1);
			fBlendY = (float)(y - nSampleY1);
		}

		float fSampleT = lerp(GetRandom(nSampleX1, nSampleY1), GetRandom(nSampleX2, nSampleY1), fBlendX);
		float fSampleB = lerp(GetRandom(nSampleX1, nSampleY2), GetRandom(nSampleX2, nSampleY2), fBlendX);

		float fNoise = (fBlendY * (fSampleB - fSampleT) + fSampleT);
		return fNoise;
	}
}