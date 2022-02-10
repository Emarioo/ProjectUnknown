#include "gonpch.h"

#include "Engone/EventHandler.h"
#include "Engone/Engone.h"

#include "GameHandler.h"
#include "UI/InterfaceManager.h"
#include "Keybindings.h"

static const std::string experimentGLSL = {
#include "Shaders/experiment.glsl"
};
static const std::string depthGLSL = {
#include "Shaders/depth.glsl"
};

#include "Objects/Goblin.h"
#include "Objects/Gnorg.h"
#include "Objects/Tutorial.h"
#include "Objects/Player.h"
#include "Objects/ModelObject.h"

#include "Engone/Handlers/ObjectHandler.h"

namespace game
{
	static Player* player=nullptr;
	Player* GetPlayer() {
		return player;
	}
	void SetPlayer(Player* _player) {
		player = _player;
	}
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
	void Update(double delta) {
		using namespace engone;

		//melody.UpdateStream();

		//light->position.x = 5 * glm::cos(engone::GetPlayTime());// bez3 * 2;
		//light->position.z = 5 * glm::sin(engone::GetPlayTime());// bez3 * 2;

		UpdateEngine(delta);

	}

	static bool initGameAssets = false;
	void InitGame() {
		using namespace engone;

		LockCursor(true);
		if (!initGameAssets) {
			InitItemHandler();
			//interfaceManager.SetupGameUI();
			initGameAssets = true;
		}

		if (false) {
			SetState(GameState::RenderGame, false);
			SetState(GameState::Intro, true);
			//ui::SetupIntro();

			IElement* cover = new IElement("darkCover", 9999);
			cover->CenterX(0)->CenterY(0)->Width(1.f)->Height(1.f)->Color({ 0.f });
			cover->add("fade").Fade({ 0.f,0.f }, .5f);

			IElement* intro = new IElement("introTexture");
			intro->Left(0)->CenterY(0)->Width(1.f)->Height(1.f)->Fixed(800 / 669.f)->Color({ 1.f })->Texture(GetAsset<Texture>("textures/intro"));
			IElement* blank = new IElement("introBlank", 9990);
			blank->CenterX(0)->CenterY(0)->Width(1.f)->Height(1.f)->Color({ 1.f });
			blank->add(intro);

			AddTimer(2.f, [=] {
				cover->setTransition("fade", true);
				AddTimer(.5 + 2.f, [=] {
					cover->setTransition("fade", false);
					AddTimer(.5f, [=] {
						RemoveElement(blank);
						cover->setTransition("fade", true);
						SetState(GameState::RenderGame, true);
						SetState(GameState::Intro, false);
						LockCursor(true);
						});
					});
				});

			AddElement(blank);
			AddElement(cover);
		}

		//-- Assets
		AddAsset<Shader>("depth", new Shader(depthGLSL, true));
		AddAsset<Shader>("experiment", new Shader(experimentGLSL, true));
		AddAsset<Font>("consolas", "fonts/consolas42");

		//-- Event and game loop functions
		AddListener(new Listener(EventType::Key, OnKey));
		AddListener(new Listener(EventType::Click, OnMouse));

		if (LoadKeybindings("data/keybindings.dat") < KEY_COUNT) {
			CreateDefualtKeybindings();
		}

		TestScene();
	}
	static float rot = 0;
	static float frame = 0;
	void Render(double lag) {
		using namespace engone;

		ModelAsset* model = GetAsset<ModelAsset>("Snake/Snake");
		Shader* shader = GetAsset<Shader>("armature");
		shader->Bind();

		glm::mat4 fin = glm::mat4(1);

		glm::mat4 axis = glm::mat4(1);

		axis[1][1] = 0;
		axis[2][1] = 1;
		axis[1][2] = -1;
		axis[2][2] = 0;

		glm::vec3 pos = glm::vec3(1), euler, scale;
		glm::mat4 quater = glm::mat4(1);


		AnimationAsset* anim = GetAsset<AnimationAsset>("Snake/SnakeAnim");
		frame += anim->defaultSpeed * 1.f / 60;

		if (frame > 60) {
			frame = 0;
		}

		short usedChannels = 0;
		anim->objects[0].GetValues(frame, 1,
			pos, euler, scale, quater, &usedChannels);

		glm::mat4 gameObject = glm::mat4(1);

		AssetInstance& aInst = model->instances[0];
		AssetInstance& mInst = model->instances[1];
		ArmatureAsset* armature = aInst.asset->cast<ArmatureAsset>();
		MeshAsset* mesh = mInst.asset->cast<MeshAsset>();
		
		glm::mat4 inve = glm::inverse((mInst.localMat));
		
		glm::mat4 bone = armature->bones[0].localMat*quater*armature->bones[0].invModel*mInst.localMat;// *(armature->bones[0].localMat * quater);

		//shader->SetMatrix("uBoneTransforms[0]", bone);
		//shader->SetMatrix("uTransform",gameObject* axis*aInst.localMat);

		//PassMaterial(shader, 0, mesh->materials[0]);
		//mesh->buffer.Draw();
		RenderEngine(lag);

		// Render custom things like items dragged by the mouse
		ui::Render(2);
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
		ModelObject* tom=new ModelObject(0, 4, 0, engone::GetAsset<ModelAsset>("Ground/Ground"));
		 
		//ModelObject* tom = new ModelObject(0, 0, 0, engone::GetAsset<ModelAsset>("Snake/Snake"));
		AddObject(tom);
		
		//ModelObject* test = new ModelObject(0, 0, 0, "VertexColor");
		//AddObject(charles);
		//AddObject(test);
		//ob->quaternion = {1,0.5,0,0};

		game::SetPlayer(new Player(0, 0, -5));
		//GetCamera()->position.x = 0;
		//GetCamera()->position.z = 5;
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