#include "Engine/Engine.h"

#include "Objects/Goblin.h"
#include "Objects/Tutorial.h"

#include "UI/Menu.h"

float ang = 0;
float vel = 0.001;
engine::PointLight* light;
void Update(float delta) {
	using namespace engine;

	ang += vel;
	//if(ang>1||ang<0)
	//	vel *= -1;
	
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
	//light->position.x = 5 * glm::cos(glfwGetTime());// bez3 * 2;
	//light->position.z = 5 * glm::sin(glfwGetTime());// bez3 * 2;
	
	if (IsGameState(Play)) {
		if (HasFocus() && !GetPauseMode()) {
			UpdateObjects(delta);
		}
	}
	UpdateUI(delta);
}
bool pauseRender = false;
engine::BufferContainer cont;
void Render() {
	using namespace engine;
	SwitchBlendDepth(false);
	if (IsGameState(Play)) {
		if (HasFocus() && !GetPauseMode() || !pauseRender) {
			//move += 0.01f;
			glm::mat4 lightView = glm::lookAt(glm::vec3(-4,4,-4), glm::vec3(0), glm::vec3(0, 1, 0));

			glClearColor(0.05f, 0.08f, 0.08f, 1);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			// Shadow stuff
			if (BindShader(ShaderDepth)) {
				glCullFace(GL_FRONT);
				GetShader(ShaderDepth)->SetMatrix("uLightMatrix", GetLightProj()*lightView);
				glViewport(0, 0, 1024, 1024);
				GetDepthBuffer().Bind();
				glClear(GL_DEPTH_BUFFER_BIT);
				RenderRawObjects();
				GetDepthBuffer().Unbind();
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			}
			
			glViewport(0, 0, Width(), Height());
			glCullFace(GL_BACK);
			BindShader(ShaderColor);
			GetShader(ShaderColor)->SetInt("shadow_map",0);
			GetShader(ShaderColor)->SetMatrix("uLightSpaceMatrix", GetLightProj() * lightView);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, GetDepthBuffer().texture);
			//glActiveTexture(GL_TEXTURE0);
			RenderObjects();
			
			/*
			SwitchBlendDepth(true);
			BindShader(ShaderExperiment);
			GetShader(ShaderExperiment)->SetInt("uTexture", 0);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, GetDepthBuffer().texture);
			cont.Draw();
			*/
		}
	}
	RenderUI();
}
void OnKey(int key, int action) {
	if (IsGameState(Play)) {
		if (action == 1) {
			if (key == GLFW_KEY_O) {
				engine::ReadOptions();
				if (engine::GetDimension() != nullptr)
					engine::GetDimension()->UpdateAllChunks();
			}
			if (key == GLFW_KEY_G) {// AT: Magic Editor
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
				if (key == GLFW_KEY_ESCAPE) {
					engine::SetPauseMode(false);
				}
			} else {
				if (key == GLFW_KEY_ESCAPE) {
					engine::SetPauseMode(true);

				}
			}
		}
	}
}
// TODO: Do something about that const string. It's not supposed to be there.
void OnMouse(double mx,double my,int button,int action,const std::string& elementName) {
	//bug::out < elementName < (elementName=="uiFade")<bug::end;
	if (IsGameState(Play)) {
		if (action == 0) {
			if (engine::GetPauseMode()) {
				engine::SetPauseMode(false);
			}
		}
	}
}
int main() {
	engine::Initialize();

	// Remember to enable cull_face

	engine::GetEventCallbacks(OnKey,OnMouse,nullptr,nullptr);
	
	//engine::AddAnimAsset("goblin_slash");
	//engine::AddBoneAsset("goblin_skeleton");
	//engine::AddMeshAsset("goblin_body");
	//engine::AddMeshAsset("Floor");
	engine::AddMeshAsset("TestMesh");
	engine::AddMeshAsset("LightCube");
	engine::AddTextureAsset("brickwall");
	engine::AddTextureAsset("brickwall_normal");
	//engine::AddMeshAsset("Parkour");

	engine::AddObject(new Tutorial(0, 0, 0));
	engine::AddObject(new Goblin(0, 0, 0));
	engine::AddLight(new engine::SpotLight({ 0,5,0 }, {0.2,-1,0}));
	//engine::AddLight(new engine::DirLight({ -1,-1,-1 }));

	SetGameState(Play);

	UI::InitStartMenu();
	UI::InitPauseMenu();

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

	engine::Start(Update, Render);

}