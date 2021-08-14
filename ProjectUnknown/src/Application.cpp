#include "propch.h"

#include "Engone/Engine.h"

#include "Objects/Goblin.h"
#include "Objects/Gnorg.h"
#include "Objects/Tutorial.h"
#include "Objects/Player.h"
#include "Objects/ModelObject.h"

#include "UI/InterfaceManager.h"
#include "Items/ItemHandler.h"
#include "GameHandler.h"
#include "GameStateEnum.h"
#include "Keybindings.h"

#include "ShaderType.h"

#include "Other/GameStatistics.h"
#include "Other/InformativeTips.h"

float ang = 0;
float vel = 1;
engine::Light* light;
engine::SoundStream melody;

engine::Animation* quater = nullptr;
engine::Model* tommy = nullptr;
engine::Channels channel;
float n = -1;
unsigned short lastN = -1;
void Update(double delta) {
	using namespace engine;

	//melody.UpdateStream();
	
	light->position.x = 5 * glm::cos(engine::GetPlayTime());// bez3 * 2;
	light->position.z = 5 * glm::sin(engine::GetPlayTime());// bez3 * 2;
	
	if (CheckState(GameState::Game)) {
		if (HasFocus() && !CheckState(GameState::Paused)) {
			UpdateObjects(delta);
		}
	}
	
	if (quater != nullptr) {
		if (tommy != nullptr) {
			if ((unsigned short)lastN != (unsigned short)n) {
				if ((unsigned short) n < tommy->armature->bones.size()) {
					//std::cout << "change "<< (unsigned short)n<<" > "<<((unsigned short)n + 1) << std::endl;
					//std::cout << quater->objects.size() << std::endl;
					quater->objects[(unsigned short)n+1]=channel;
					quater->objects.erase((unsigned short)n);
					//std::cout << quater->objects[(unsigned short)n + 1].fcurves.size() << std::endl;

				}
				if (n > 12)
					n = -1;
				lastN = n;
			}
		}
		else {
			tommy = engine::GetModelAsset("Tom");
			std::cout << tommy << std::endl;
		}

		n += 24*delta/60;
	}

	/*ang += vel * delta;
	if (ang > 1 || ang < 0)
		vel *= -1;
	float bez = bezier(ang, 0, 1) - 3.14159 / 3;
	float bez2 = bezier(ang * 0.3, 0, 0.3) * 0.3 - 0.5;
	float bez3 = bezier(ang, 0, 1);
	engine::GetCamera()->rotation.y = bez;// +3.14159 / 2.f;
	engine::GetCamera()->rotation.x = bez2;
	engine::GetCamera()->SetPosition((
		glm::translate(glm::vec3(0, 4, 0)) *
		glm::rotate(bez, glm::vec3(0, 1, 0)) *
		glm::rotate(bez2, glm::vec3(1, 0, 0)) *
		glm::translate(glm::vec3(0, 0, 5.2))
		)[3]);
	*/
	UpdateUI(delta);
}
engine::BufferContainer cont;
engine::BufferContainer itemContainer2;

float coloring = 0;
void Render(double lag) {
	using namespace engine;
	SwitchBlendDepth(false);
	if (CheckState(GameState::Game)) {
		if (HasFocus()) {
			//move += 0.01f;
			glm::mat4 lightView = glm::lookAt({-2,4,-1}, glm::vec3(0), { 0,1,0 });
			
			glClearColor(0.05f, 0.08f, 0.08f, 1);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			coloring += lag;
			if (coloring>1) {
				coloring = 0;
			}

			// All this should be automated and customizable in the engine.

			// Shadow stuff
			if (BindShader(ShaderType::Depth)) {
				glCullFace(GL_BACK);
				GetShader(ShaderType::Depth)->SetMatrix("uLightMatrix", GetLightProj()*lightView);
				glViewport(0, 0, 1024, 1024);
				GetDepthBuffer().Bind();
				glClear(GL_DEPTH_BUFFER_BIT);
				RenderRawObjects(lag);
				GetDepthBuffer().Unbind();
			}
			
			glViewport(0, 0, Width(), Height());
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glCullFace(GL_BACK);
			BindShader(ShaderType::Color);
			GetShader(ShaderType::Color)->SetInt("shadow_map",0);
			GetShader(ShaderType::Color)->SetMatrix("uLightSpaceMatrix", GetLightProj() * lightView);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, GetDepthBuffer().texture);
			RenderObjects(lag);
			
			if (engine::IsKey(GLFW_KEY_K)) {
				SwitchBlendDepth(true);
				BindShader(ShaderType::Experiment);
				GetShader(ShaderType::Experiment)->SetInt("uTexture", 0);
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, GetDepthBuffer().texture);
				cont.Draw();
			}
		}
	}

	// Render IBase, IElement...
	engine::RenderUI(lag);
	// Render custom things like items dragged by the mouse
	interfaceManager.Render(lag);
}
void OnKey(int key, int action) {
	if (engine::CheckState(GameState::Game)) {
		if (action == 1) {
			if (key == GLFW_KEY_O) {
				engine::ReadOptions();
				if (engine::GetDimension() != nullptr)
					engine::GetDimension()->UpdateAllChunks();
			}
			if (key == GLFW_KEY_G) {// what in the world does this mean ->   AT: Magic Editor
				/*IElement* el = GetElement("editor_script");
				if (el != nullptr) {
					if (!el->selected) {

						if (HasTag(8)) {
							//DelTag(8);

							renderer::SetCursorMode(false);
							// Pause game?
						} else {
							//AddTag(8);
							renderer::SetCursorMode(true);
							// Other things?
						}

					}
				}*/
			}
			if (engine::TestActionKey(KeyPause, key)) {
				if (engine::CheckState(GameState::Paused)) {
					engine::SetState(GameState::Paused, false);
					engine::LockCursor(true);
					
				} else {
					engine::SetState(GameState::Paused, true);
					engine::LockCursor(false);
				}
			}
		}
	}
}

//  Do something about that const string. It's not supposed to be there.
void OnMouse(double mx,double my,int action,int button,const std::string& elementName) {
	//bug::out < elementName < (elementName=="uiFade")<bug::end;
	/*if (IsGameState(Play)) {
		if (action == 0) {
			if (engine::GetPauseMode()) { // Unpause in pause menu if clicking on anything
				engine::SetPauseMode(false);
			}
		}
	}*/
}

// Is this really supposed to be here?
void CustomDimension() {
	using namespace engine;
	float a = 5 * engine::GetOptionf("scale") / 500;
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
	gray.Flat(0.5,1,1,0);
	noise.AddBiome(gray);
	Dimension& dim = noise;
	dim.Init(gameHandler.player);
	engine::AddDimension("classic",dim);
	//engine::SetDimension("classic");
}
void runApp(bool isDebugBuild) {
	// Init the absolutely neccessary stuff
	engine::SetState(GameState::DebugLog,isDebugBuild);
	engine::Initialize();
	engine::GetEventCallbacks(OnKey, OnMouse, nullptr, nullptr);
	if (engine::LoadKeybindings("data/keybindings.dat")<KEY_COUNT) {
		CreateDefualtKeybindings();
	}

	bool startGame = true;

	if (startGame) {
		gameHandler.StartGame();
	}
	else if (!isDebugBuild) {
		engine::SetState(GameState::Intro, true);
		interfaceManager.SetupIntro();
		engine::SetCursorVisibility(false);
	} else {
		 engine::SetState(GameState::Menu, true);
		 engine::SetupDebugPanel();
	}
	//interfaceManager.SetupMainMenu();

	// Debug Options - move this else where. like the debugpanel
	/*
	bug::set("LoadMesh", 1);
	bug::set("LoadMesh.Weights", 1);
	bug::set("LoadMesh.Triangles", 1);
	bug::set("LoadMesh.Vectors", 1);
	bug::set("LoadMesh.Colors", 1);
	bug::set("LoadMesh.Normals", 1);
	//bug::set("LoadMesh.Buffer", 1);
	*/
	bug::set("LoadAnimation", 1);
	bug::set("LoadAnimation.Frames", 1);
		
	//bug::set("LoadCollider", 1);
	//bug::set("LoadCollider.Vectors", 1);
	//bug::set("LoadCollider.Quads", 1);
		 
	//bug::set("LoadArmature", 1);
	//bug::set("LoadArmature.Bones", 1);
	//bug::set("LoadArmature.Matrix", 1);
		 
	bug::set("LoadModel", 1);
	//bug::set("LoadModel.Mesh", 1);
	//bug::set("LoadModel.Matrix", 1);

	//bug::set("LoadMaterial", 1);

	// Assets should be loaded else where. Maybe where you start the game. 
	// Only if they havent't been loaded though.
	{
		using namespace engine;

		AddShader(ShaderType::Light, "lightSource");
		AddShader(ShaderType::Experiment, "experiment");
		AddShader(ShaderType::Terrain, "terrain");

		AddFont("consolas42");

		//AddAnimAsset("goblin_slash");
		//AddBoneAsset("goblin_skeleton");
		//AddMeshAsset("goblin_body");
		//AddTextureAsset("noise");
		//AddMeshAsset("Plane.001");
		//AddMeshAsset("Plane.013");
		//AddModelAsset("Tutorial");

		//AddModelAsset("TestMesh");
		//AddMeshAsset("LightCube");
		//AddModelAsset("LightCube");
		//AddTextureAsset("Test");
		//AddModelAsset("Player");
		// 
		//AddModelAsset("Player");
		//AddModelAsset("Tutorial");
		//AddModelAsset("Terrain");
		//AddModelAsset("Gnorg");
	}
	//bug::out < a->Get(0)->Get(0)->frames[1].value < bug::end;
	//engine::AddAnimationAsset("ArmatureAction");
	//engine::GetModelAsset("Player")->AddAnimation("ArmatureAction");

	//engine::AddTextureAsset("brickwall");
	//engine::AddTextureAsset("brickwall_normal");

	//engine::AddTextureAsset("items0");
	//engine::AddTextureAsset("magicstaff_fireball");
	{
		// Remove some of the quaternion values and see if it works
		using namespace engine;
		AddAnimationAsset("Quater");
		quater = GetAnimationAsset("Quater");
		
		quater->Modify(0, 60);

		FCurve x,y,z,w;

		z.Add({ Bezier, 0, 0 });
		z.Add({ Bezier, 60, 0.5 });

		//cha.Add(PosX, x);
		channel.Add(QuaX, z);
		//cha.Add(QuaZ, z);
		//cha.Add(QuaW, w);

		quater->AddObjectChannels(0, channel);

		//std::cout << "obs "<< quater->objects.size() << std::endl;
		//std::cout << "channs "<< quater->objects[0].fcurves.size() << std::endl;
	}

	/*
	melody.Init("assets/sounds/melody.wav");

	engine::SoundBuffer trumpet;
	trumpet.Init("assets/sounds/trumpet.wav");

	engine::SoundSource trumpetS;
	trumpetS.Init();
	trumpetS.Bind(trumpet);

	//trumpetS.Play();
	//melody.source.Play();

	melody.source.Gain(0.25);
	*/

	// Game World setup - debug purpose at the moment
	//engine::AddObject(new Tutorial(0, 0, 0));
	//engine::AddObject(new Goblin(0, -3, 0));
	//engine::AddObject(new Gnorg(5, 0,5));

	//gameHandler.player = new Player(0, 15, 0);
	//gameHandler.player->flight = true;
	//player->renderHitbox = true;
	//engine::AddObject(gameHandler.player);
	//Tutorial* tutorial = new Tutorial(0, 0, 0);
	//tutorial->renderHitbox = true;
	//engine::AddObject(tutorial);


	ModelObject* ob;
	engine::AddObject(ob=new ModelObject(3, 0, 0,"Charles"));
	engine::AddObject(ob=new ModelObject(-3, 0, 0,"Tom"));
	//ob->quaternion = {1,0.5,0,0};


	gameHandler.player = new Player(0, 0, 0);
	engine::GetCamera()->position.x = 0;
	engine::GetCamera()->position.z = 5;
	gameHandler.player->flight = true;
	engine::AddObject(gameHandler.player);
	engine::SetState(GameState::CameraToPlayer,false);
	//engine::GetCamera()->SetPosition(0,0,-5);

	engine::DirLight* l = new engine::DirLight({ 2,-4,1 });
	
	//l->linear = 0.25;
	//l->quadratic = 0.059;
	engine::AddLight(light=l);
	//engine::AddLight(new engine::DirLight({ -1,-1,-1 }));

	//CustomDimension();
	
	// Shadow framebuffer
	float v[]{
		-1,-1,0,0,
		1,-1,1,0,
		1,1,1,1,
		-1,1,0,1
	};
	unsigned int i[]{
		0,1,2,
		2,3,0
	};
	cont.Init(false,v,16,i,6);
	cont.SetAttrib(0,2,4,0);
	cont.SetAttrib(1,2,4,2);

	engine::Start(Update, Render, 60);
}
INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR lpCmdLine, INT nCmdShow)
{
	runApp(false);
	return 0;
}
int main(int argc, char* argv[]) {

	//std::cout << informativeTips.GetDeathTip() << std::endl;
	//std::cin.get();
	runApp(true);
	return 0;
}