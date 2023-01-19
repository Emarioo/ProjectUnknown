
#include "Tutorial.h"

Tutorial::Tutorial(float x, float y, float z) : GameObject("Tutorial",x,y,z) {
	//renderComponent.SetModel("Terrain");
	//collisionComponent.SetCollider(renderComponent.model->colliderName);

}
//void Tutorial::Update(float delta) {
//	
//}
/*
void CustomDimension()
{
	using namespace engone;
	float a = 5 * GetOptionf("scale") / 500;
	Dimension classic;
	Biome waterD(0.3, -6 * a, 2 * a);
	Biome waterS(0.4, 2 * a, 6 * a);
	Biome sand(0.45, 6 * a, 6.6 * a);
	Biome grass(0.55, 6.65 * a, 7.2 * a);
	Biome grass2(0.6, 7.2 * a, 8 * a);
	Biome rock(0.7, 8 * a, 13 * a);
	Biome rock2(0.9, 13 * a, 25 * a);
	Biome snow(1, 25 * a, 28 * a);

	a = 10;
	waterD.ColorD(10, a, 67, a, 255, a);
	waterS.ColorD(13, a, 95, a, 255, a);
	sand.ColorD(255, a, 236, a, 159, a);
	grass.ColorD(74, a, 190, a, 31, a);
	grass2.ColorD(60, a, 165, a, 49, a);
	rock.ColorD(99, a, 65, a, 49, a);
	rock2.ColorD(75, a, 48, a, 18, a);
	snow.ColorD(240, a, 253, a, 255, a);

	waterD.Flat(0.4, 1, 0.1, 0.3);

	classic.AddBiome(waterD);
	classic.AddBiome(waterS);
	classic.AddBiome(sand);
	classic.AddBiome(grass);
	classic.AddBiome(grass2);
	classic.AddBiome(rock);
	classic.AddBiome(rock2);
	classic.AddBiome(snow);

	Dimension classic2;

	Biome dwater(0.2, 0, 5);
	dwater.Color(10, 67, 255);
	classic2.AddBiome(dwater);
	Biome water(0.35, 5, 10);
	water.Color(13, 95, 255);
	classic2.AddBiome(water);
	Biome sandy(0.5, 10, 15);
	sandy.Color(255, 236, 159);
	classic2.AddBiome(sandy);
	Biome plains(0.7, 15, 20);
	plains.Color(74, 190, 31);
	classic2.AddBiome(plains);
	Biome mount(0.8, 20, 25);
	mount.Color(60, 55, 18);
	classic2.AddBiome(mount);
	Biome snowy(1, 25, 30);
	snowy.Color(240, 253, 255);
	classic2.AddBiome(snowy);

	Dimension noise;

	Biome gray(1, 0, 10);
	gray.Color(100, 256, 50, 256, 20, 256);
	gray.Flat(0.5, 1, 1, 0);
	noise.AddBiome(gray);
	Dimension& dim = noise;
	dim.Init(game::GetPlayer());
	engone::AddDimension("classic", dim);
	//engine::SetDimension("classic");
}*/