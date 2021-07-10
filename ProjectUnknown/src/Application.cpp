#include "Engine/Engine.h"

#include "Objects/Goblin.h"
#include "Objects/Gnorg.h"
#include "Objects/Tutorial.h"
#include "Objects/Player.h"

#include "UI/InterfaceManager.h"

#include "Items/ItemHandler.h"

#include "GameHandler.h"

#include "Engine/Handlers/FileHandler.h"

#include "KeyBinding.h"

float ang = 0;
float vel = 0.001;
engine::Light* light;
engine::SoundStream melody;
void Update(float delta) {
	using namespace engine;

	ang += vel;
	//if(ang>1||ang<0)
	//	vel *= -1;

	melody.UpdateStream();
	
	float bez = bezier(ang, 0, 1) - 3.14159/3;
	float bez2 = bezier(ang*0.3,0, 0.3)*0.3-0.5;
	float bez3 = bezier(ang,0, 1);
	/*engine::GetCamera()->rotation.y = bez;// +3.14159 / 2.f;
	engine::GetCamera()->rotation.x = bez2;
	engine::GetPlayer()->SetPosition((
		glm::translate(glm::vec3(0, 4,0))*
		glm::rotate(bez, glm::vec3(0, 1, 0)) *
		glm::rotate(bez2, glm::vec3(1, 0, 0)) *
		glm::translate(glm::vec3(0, 0, 5.2))
		)[3]);
	*/
	light->position.x = 5 * glm::cos(glfwGetTime());// bez3 * 2;
	light->position.z = 5 * glm::sin(glfwGetTime());// bez3 * 2;
	
	if (CheckState("PLAY")) {
		if (HasFocus() && !GetPauseMode()) {
			UpdateObjects(delta);
		}
	}
	UpdateUI(delta);
}
bool pauseRender = false;
engine::BufferContainer cont;
engine::BufferContainer itemContainer2;
void Render() {
	using namespace engine;
	SwitchBlendDepth(false);
	if (CheckState("PLAY")) {
		if (HasFocus() && !GetPauseMode() || !pauseRender) {
			//move += 0.01f;
			
			glm::mat4 lightView = glm::lookAt({-2,4,-1}, glm::vec3(0), { 0,1,0 });
			
			glClearColor(0.05f, 0.08f, 0.08f, 1);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			// Shadow stuff
			if (BindShader(ShaderDepth)) {
				glCullFace(GL_BACK);
				GetShader(ShaderDepth)->SetMatrix("uLightMatrix", GetLightProj()*lightView);
				glViewport(0, 0, 1024, 1024);
				GetDepthBuffer().Bind();
				glClear(GL_DEPTH_BUFFER_BIT);
				RenderRawObjects();
				GetDepthBuffer().Unbind();
			}
			
			glViewport(0, 0, Width(), Height());
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glCullFace(GL_BACK);
			BindShader(ShaderColor);
			GetShader(ShaderColor)->SetInt("shadow_map",0);
			GetShader(ShaderColor)->SetMatrix("uLightSpaceMatrix", GetLightProj() * lightView);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, GetDepthBuffer().texture);
			RenderObjects();
			
			if (engine::IsKey(GLFW_KEY_K)) {
				SwitchBlendDepth(true);
				BindShader(ShaderExperiment);
				GetShader(ShaderExperiment)->SetInt("uTexture", 0);
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, GetDepthBuffer().texture);
				cont.Draw();
			}
		}
	}
	// Render IBase, IElement...
	RenderUI();
	// Render custom things like items dragged by the mouse
	UI::Render();
}
void OnKey(int key, int action) {
	if (engine::CheckState("PLAY")) {
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
			if (engine::GetPauseMode()) {
				if (TestActionKey(KeyPause,key)) {
					engine::SetPauseMode(false);
				}
			} else {
				if (TestActionKey(KeyPause, key)) {
					engine::SetPauseMode(true);

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
	dim.Init(GetPlayer());
	engine::AddDimension("classic",dim);
	//engine::SetDimension("classic");
}
int main(int argc, char*argv[]) {
	// debug override
	if (argc > 0) {
		for (int i = 0; i < argc;i++) {
			if (argv[i] == "debug") bug::ENABLE_DEBUG_OVERRIDE();
		}
	}
	bug::Enable(true);
	engine::Initialize();
	InitItemHandler();

	engine::GetEventCallbacks(OnKey,OnMouse,nullptr,nullptr);

	// Debug Options
	//bug::set("load_mesh_info", 1);
	//bug::set("load_mesh_vectors", 1);
	//bug::set("load_mesh_colors", 1);
	//bug::set("load_mesh_normals", 1);
	//bug::set("load_mesh_buffer", 1);
		
	//bug::set("load_anim_info", 1);
	//bug::set("load_anim_frames", 1);
		
	//bug::set("load_coll_info", 1);
	//bug::set("load_coll_vectors", 1);
	//bug::set("load_coll_quads", 1);
		 
	//bug::set("load_arma_info", 1);
	//bug::set("load_arma_bone", 1);
//	bug::set("load_arma_matrix", 1);
		 
	//bug::set("load_model_info", 1);
	//bug::set("load_model_mesh", 1);
//	bug::set("load_model_matrix", 1);

	//bug::set("load_mat_info", 1);

	InitKeyBindings();

	//engine::AddAnimAsset("goblin_slash");
	//engine::AddBoneAsset("goblin_skeleton");
	//engine::AddMeshAsset("goblin_body");
	//engine::AddTextureAsset("noise");
	//engine::AddMeshAsset("Plane.001");
	//engine::AddMeshAsset("Plane.013");
	//engine::AddModelAsset("Tutorial");

	//engine::AddModelAsset("TestMesh");
	//engine::AddMeshAsset("LightCube");
	//engine::AddModelAsset("LightCube");
	//engine::AddTextureAsset("Test");
	//engine::AddModelAsset("Player");
	engine::AddModelAsset("Player");
	engine::AddModelAsset("Tutorial");
	engine::AddModelAsset("Terrain");
	engine::AddModelAsset("Gnorg");

	//bug::out < a->Get(0)->Get(0)->frames[1].value < bug::end;
	//engine::AddAnimationAsset("ArmatureAction");
	//engine::GetModelAsset("Player")->AddAnimation("ArmatureAction");

	//engine::AddTextureAsset("brickwall");
	//engine::AddTextureAsset("brickwall_normal");

	//engine::AddTextureAsset("items0");
	//engine::AddTextureAsset("magicstaff_fireball");

	melody.Init("assets/sounds/melody.wav");

	engine::SoundBuffer trumpet;
	trumpet.Init("assets/sounds/trumpet.wav");

	engine::SoundSource trumpetS;
	trumpetS.Init();
	trumpetS.Bind(trumpet);

	//trumpetS.Play();
	//melody.source.Play();

	melody.source.Gain(0.25);

	//engine::AddObject(new Tutorial(0, 0, 0));
	//engine::AddObject(new Goblin(0, -3, 0));
	engine::AddObject(new Goblin(0, 15, 0));
	//engine::AddObject(new Gnorg(5, 0,5));

	Player* player = new Player(0, 15, 0);
	player->flight = true;
	//player->renderHitbox = true;
	SetPlayer(player);
	engine::AddObject(player);
	//Tutorial* tutorial = new Tutorial(0, 0, 0);
	//tutorial->renderHitbox = true;
	//engine::AddObject(tutorial);

	engine::DirLight* l = new engine::DirLight({ 2,-4,1 });
	
	//l->linear = 0.25;
	//l->quadratic = 0.059;
	engine::AddLight(light=l);
	//engine::AddLight(new engine::DirLight({ -1,-1,-1 }));

	//CustomDimension();
	//bug::out < "Here" < bug::end;
	engine::AddState("PLAY");

	UI::InitializeUI();

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

	cont.Setup(false,v,16,i,6);
	cont.SetAttrib(0,2,4,0);
	cont.SetAttrib(1,2,4,2);
	//bug::out < "Here" < bug::end;
	engine::Start(Update, Render);
}