#include "gonpch.h"

#include "Engone/EventHandler.h"

#include "GameHandler.h"
#include "UI/InterfaceManager.h"
#include "Keybindings.h"

static const std::string experimentGLSL = {
#include "Shaders/experiment.glsl"
};
static const std::string depthGLSL = {
#include "Shaders/depth.glsl"
};

// Temporary includes
#include "Objects/Goblin.h"
#include "Objects/Gnorg.h"
#include "Objects/Tutorial.h"
#include "Objects/Player.h"
#include "Objects/ModelObject.h"

#include "Engone/Handlers/ObjectHandler.h"

namespace game
{
	static bool initGameAssets = false;
	static Player* _player;
	void SetPlayer(Player* player)
	{
		_player = player;
	}
	Player* GetPlayer()
	{
		return _player;
	}
	void TestScene();
	
	engone::EventType OnKey(engone::Event& e)
	{
		if (engone::CheckState(GameState::RenderGame)) {
			if (e.action == 1) {
				if (e.key == GLFW_KEY_O) {
					engone::ReadOptions();
					if (engone::GetDimension() != nullptr)
						engone::GetDimension()->UpdateAllChunks();
				}
				if (e.key == GLFW_KEY_G) {// what in the world does this mean ->   AT: Magic Editor
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
				if (engone::IsKeybindingDown(KeyPause)) {
					if (engone::CheckState(GameState::Paused)) {
						engone::SetState(GameState::Paused, false);
						engone::LockCursor(true);
					}
					else {
						engone::SetState(GameState::Paused, true);
						engone::LockCursor(false);
					}
				}
			}
		}
		return engone::EventType::None;
	}

	//  Do something about that const string. It's not supposed to be there.
	engone::EventType OnMouse(engone::Event& e)
	{
		//bug::out < elementName < (elementName=="uiFade")<bug::end;
		/*if (IsGameState(Play)) {
			if (action == 0) {
				if (engine::GetPauseMode()) { // Unpause in pause menu if clicking on anything
					engine::SetPauseMode(false);
				}
			}
		}*/
		return engone::EventType::None;
	}
	void Update(double delta)
	{
		using namespace engone;

		//melody.UpdateStream();

		//light->position.x = 5 * glm::cos(engone::GetPlayTime());// bez3 * 2;
		//light->position.z = 5 * glm::sin(engone::GetPlayTime());// bez3 * 2;

		//UpdateEngine(delta);

	}
	void Render(double lag)
	{
		using namespace engone;

		//RenderEngine(lag);

		// Render custom things like items dragged by the mouse
		ui::Render(lag);
	}
	void InitGame()
	{
		using namespace engone;
		LockCursor(true);
		if (!initGameAssets) {
			InitItemHandler();
			//interfaceManager.SetupGameUI();
			initGameAssets = true;
		}

		if(false) {
			SetState(GameState::RenderGame, false);
			SetState(GameState::Intro, true);
			//ui::SetupIntro();

			Panel* intro = new Panel("intro");
			intro->CenterX(0)->CenterY(0)->Width(1.f)->Height(1.f);
			AddPanel(intro);

			SetCursorVisible(false);
		}

		//-- Assets
		AddAsset<Shader>("depth", new Shader(depthGLSL, true));
		AddAsset<Shader>("experiment", new Shader(experimentGLSL, true));
		AddAsset<Font>("consolas", "fonts/consolas42");

		//-- Event and game loop functions
		AddListener(new Listener(EventType::Key | EventType::GLFW, OnKey));
		AddListener(new Listener(EventType::Click | EventType::GLFW, OnMouse));

		if (LoadKeybindings("data/keybindings.dat") < KEY_COUNT) {
			CreateDefualtKeybindings();
		}

		TestScene();
	}

	void TestScene()
	{
		using namespace engone;
		//bug::out < a->Get(0)->Get(0)->frames[1].value < bug::end;
		//engine::AddAnimationAsset("ArmatureAction");
		//engine::GetModelAsset("Player")->AddAnimation("ArmatureAction");

		//engine::AddTextureAsset("brickwall");
		//engine::AddTextureAsset("brickwall_normal");

		//engine::AddTextureAsset("items0");
		//engine::AddTextureAsset("magicstaff_fireball");
		{
			// Remove some of the quaternion values and see if it works

			/*AddAnimationAsset("Quater");
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
			*/
			//std::cout << "obs "<< quater->objects.size() << std::endl;
			//std::cout << "channs "<< quater->objects[0].fcurves.size() << std::endl;
		}

		//-- Game World setup - debug purpose at the moment
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

		//ModelObject* axis = new ModelObject(0, 0, 0, engone::GetAsset<ModelAsset>("Axis/Axis"));
		//AddObject(axis);

		//ModelObject* charles=new ModelObject(3, 0, 0, "Charles");
		//ModelObject* tom=new ModelObject(0, 4, 0, engone::GetAsset<ModelAsset>("Plant/Plant"));
		ModelObject* tom = new ModelObject(0, 4, 0, engone::GetAsset<ModelAsset>("Plant.001/Plant.001"));
		
		//ModelObject* test = new ModelObject(0, 0, 0, "VertexColor");
		//AddObject(charles);
		AddObject(tom);
		//AddObject(test);
		//ob->quaternion = {1,0.5,0,0};

		game::SetPlayer(new Player(0, 0, -5));
		GetCamera()->position.x = 0;
		GetCamera()->position.z = 5;
		game::GetPlayer()->flight = true;
		AddObject(game::GetPlayer());
		SetState(GameState::CameraToPlayer, false);
		//engine::GetCamera()->SetPosition(0,0,-5);

		DirLight* l = new DirLight({ 2,-4,1 });

		//l->linear = 0.25;
		//l->quadratic = 0.059;
		AddLight(l);
		//engine::AddLight(new engine::DirLight({ -1,-1,-1 }));
	}
}