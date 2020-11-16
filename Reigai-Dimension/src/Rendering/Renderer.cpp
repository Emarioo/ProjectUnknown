#include "Renderer.h"

WindowTypes windowType = WindowBorderless;
WindowTypes windowType2 = WindowBorderless;
WindowTypes GetWindowType() {
	return windowType;
}
WindowTypes GetLastWindowType() {
	return windowType2;
}
bool IsFullscreen() {
	return windowType == Fullscreen;
}
bool IsMaximized() {
	return windowType == Maximized;
}
bool IsMinimized() {
	return windowType == Minimized;
}
int winX = 0;
int winY = 0;
int winW = 0;
int winH = 0;
int lastX = 0;
int lastY = 0;
int lastW = 0;
int lastH = 0;
/*
	-1 to ignore a paramter
*/
void SetWinSize(int x,int y,int w,int h) {
	if (!(x < 0))
		winX = x;
	if (!(y < 0))
		winY = y;
	if (!(w < 0))
		winW = w;
	if (!(h < 0))
		winH = h;
}
float Wid() {
	return 1000;
}
float Hei() {
	return 1000/(16.f/9);
}
/*
Width of game screen not window!
*/
float Width() {
	if (windowType==WindowBorder) {
		return winW-8-8;
	} else {
		return winW;
	}
}
/*
Height of game screen not window!
*/
float Height() {
	if (windowType == WindowBorder) {
		return winH - 8 - 31;
	} else if (windowType==Maximized){
		return winH-31;
	} else {
		return winH;
	}
}
float WWidth() {
	return winW;
}
float WHeight() {
	return winH;
}
bool hasFocus=true;
bool HasFocus() {
	return hasFocus;
}

float AlterW(float w) {
	return 2.f*(w) / Width();
}
float AlterH(float h) {
	return 2.f*(h) / Height();
}
float AlterX(float x) {
	return 2.f*(x) / Width()-1;
}
float AlterY(float y) {
	return 1-2.f*(y) / Height();
}

glm::mat4 projMatrix;
float fov;
float zNear;
float zFar;
Camera camera;
Camera* GetCamera() {
	return &camera;
}
void SetProjection(float ratio) {
	projMatrix = glm::perspective(fov, ratio, zNear, zFar);
}
void UpdateProj() {
	glm::mat4 trans = glm::translate(glm::mat4(1.0f), camera.position) *
		glm::rotate(glm::mat4(1.0f), camera.rotation.y, glm::vec3(0, 1, 0))*
		glm::rotate(glm::mat4(1.0f), camera.rotation.x, glm::vec3(1, 0, 0));

	BindShader("tex");
	ObjectProjection(projMatrix * glm::inverse(trans));
	BindShader("color");
	ObjectProjection(projMatrix * glm::inverse(trans));
	BindShader("terrain");
	ObjectProjection(projMatrix * glm::inverse(trans));
	BindShader("outline");
	ObjectProjection(projMatrix * glm::inverse(trans));
}

// Dragging

// Resize
int resizingWin=0;

double renMouseX;
double renMouseY;

GLFWwindow* window;
GLFWwindow* GetWindow() {
	return window;
}
bool InsideBorder() {
	if(windowType==WindowBorder)
		return (renMouseX>=8&&renMouseX<WWidth()-8&&renMouseY>=31&&renMouseY<WHeight()-8);
	else if (windowType == Maximized)
		return (renMouseY >= 31);
	return true;
}
BufferContainer border;
void (*KeyEvent)(int, int);
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods){
	if (KeyEvent != nullptr) {
		KeyEvent(key,action);
	}
}
void(*MouseEvent)(double,double,int, int);
void MouseCallback(GLFWwindow* window, int button, int action, int mods) {
	if (windowType == Fullscreen||windowType==WindowBorderless) {
		if (MouseEvent != nullptr) 
			MouseEvent(renMouseX, renMouseY, button, action);
		
	} else if (windowType == WindowBorder) {
		if (button == 0) {
			if (action == 1) {
				// Check Buttons
				if (renMouseY < 8) {
					resizingWin = 1;
					return;
				}else if (renMouseX < 8) {
					resizingWin = 2;
					return;
				} else if (renMouseY > WHeight() - 8) {
					resizingWin = 3;
					return;
				} else if (renMouseX > WWidth() - 8) {
					resizingWin = 4;
					return;
				}
			} else if(action==0){
				resizingWin = 0;
			}
		}
		if (MouseEvent != nullptr) {
			if (InsideBorder()) {
				MouseEvent(renMouseX - 8, renMouseY - 31, button, action);
			}
		}
	} else if (windowType == Maximized) {
		if (button == 0) {
			if (action == 1) {
				// Check buttons
			}
		}
		if (MouseEvent != nullptr) {
			if (InsideBorder()) {
				MouseEvent(renMouseX, renMouseY - 31, button, action);
			}
		}
	}
}
void(*DragEvent)(double, double);
void DragCallback(GLFWwindow* window, double mx, double my) {
	if (!GetCursorMode()) {
		camera.rotation.y -= (mx - renMouseX) * (3.14159f / 360)*0.18;
		camera.rotation.x -= (my - renMouseY) * (3.14159f / 360)*0.18;
	}
	if (DragEvent != nullptr)
		DragEvent(mx, my);
	renMouseX = mx;
	renMouseY = my;
}
void(*ResizeEvent)(int, int);
void ResizeCallback(GLFWwindow* window, int width, int height) {
	if (windowType != Minimized)
		SetWinSize(-1,-1,width,height);
	glViewport(0, 0, Width(), Height());
	if (ResizeEvent != nullptr) {
		ResizeEvent(width, height);
	}
}
void(*FocusEvent)(int);
void WindowFocusCallback(GLFWwindow* window, int focus) {
	if (FocusEvent != nullptr) {
		hasFocus = focus;
		FocusEvent(focus);
	}
}
//void(*PosEvent)(int);
void WindowPosCallback(GLFWwindow* window, int x, int y) {
	//if(windowType==1)
	SetWinSize(x, y, -1, -1);
}
void SetCallbacks(
	void(*key)(int,int),
	void(*mouse)(double,double,int,int),
	void(*drag)(double,double),
	void(*resize)(int,int),
	void(*focus)(int)) {
	KeyEvent = key;
	MouseEvent = mouse;
	DragEvent = drag;
	ResizeEvent = resize;
	FocusEvent = focus;
}
void RenderInit() {
	if (!glfwInit()) {
		std::cout << "Not Init Window!" << std::endl;
		return;
	}
	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	
	SetWinSize(mode->width/6,mode->height/6, mode->width / 1.5, mode->height / 1.5);
	
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);
	//glfwWindowHint(GLFW_DECORATED, false);

	window = glfwCreateWindow(winW, winH, "Reigai Dimension", NULL, NULL);
	if (!window) {
		glfwTerminate();
		std::cout << "Terminate Window!" << std::endl;
		return;
	}

	//SetWindowType(WindowBorder);
	SetWindowType(WindowBorderless);

	glfwMakeContextCurrent(window);
	if (glewInit() != GLEW_OK) {
		std::cout << "Error!" << std::endl;
		return;
	}

	glfwSetKeyCallback(window, KeyCallback);
	glfwSetMouseButtonCallback(window, MouseCallback);
	glfwSetCursorPosCallback(window, DragCallback);
	glfwSetWindowFocusCallback(window, WindowFocusCallback);
	glfwSetWindowSizeCallback(window, ResizeCallback);
	glfwSetWindowPosCallback(window, WindowPosCallback);
	// Remember to SetCallbacks in other class file
	
	SwitchBlendDepth(true);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
	// Done with GLFW and OpenGl Initializing

	// Border
	/*
	unsigned int ind[]{
		0,1,2,
		2,3,0
	};
	float ver[]{
		0,-1,0,0,
		2,-1,1,0,
		2,1,1,1,
		0,1,0,1
	};
	border.Setup(true,ver,4*4,ind,6);
	border.SetAttrib(0, 4, 4, 0);
	*/
	// Presets

	AddShader("gui","assets/shaders/interface");
	AddShader("tex","assets/shaders/tex");
	AddShader("terrain","assets/shaders/terrain");
	AddShader("color","assets/shaders/color");
	AddShader("outline","assets/shaders/outline");

	AddTexture("blank","assets/textures/blank");

	fov = 90.f;
	zNear = 0.1f;
	zFar = 400.f;
	SetProjection((float)Width() / Height());
}
bool cursorMode=true;
bool GetCursorMode() {
	return cursorMode;
}
void SetCursorMode(bool f) {
	if (cursorMode != f) {
		if (f) {
			glfwSetInputMode(GetWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			if (glfwRawMouseMotionSupported()) {
				glfwSetInputMode(GetWindow(), GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);
			}
		} else {
			glfwSetInputMode(GetWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			if (glfwRawMouseMotionSupported()) {
				glfwSetInputMode(GetWindow(), GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
			}
		}
		cursorMode = f;
	}
}
void RenderClearScreen(float r, float g, float b, float a) {
	glClearColor(r, g, b, a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
bool RenderRunning() {
	return !glfwWindowShouldClose(window);
}
void RenderTermin() {
	glfwTerminate();
}
void SetWindowTitle(const char* title) {
	glfwSetWindowTitle(window, title);
}
void SwitchBlendDepth(bool b) {
	if (b) {
		glEnable(GL_BLEND);
		glDisable(GL_DEPTH_TEST);
	} else {
		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
	}
}
void SetWindowType(WindowTypes t) {
	if (t == Fullscreen) {
		lastX = winX;
		lastY = winY;
		lastW = winW;
		lastH = winH;

		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);
		glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
	} else {
		if (windowType != WindowBorderless) {
			SetWinSize(lastX, lastY, lastW, lastH);
		}
		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);
		glfwSetWindowMonitor(window, NULL, winX, winY, winW, winH, mode->refreshRate);
	}
	windowType = t;
	return;

	// Own border
	if (t == windowType)
		return;
	
	if (t == Minimized) {
		lastX = winX;
		lastY = winY;
		lastW = winW;
		lastH = winH;

		glfwIconifyWindow(window);
	} else if (t == WindowBorder) {
		if (windowType != WindowBorder && windowType!=WindowBorderless) {
			SetWinSize(lastX, lastY, lastW, lastH);
			glfwSetWindowPos(window, winX, winY);
			glfwSetWindowSize(window, winW, winH);
		}
	} else if (t == WindowBorderless) {
		/*
		// Removed when using my own Border
		SetWinSize(lastX, lastY, lastW, lastH);
		glfwSetWindowPos(window, winX, winY);
		glfwSetWindowSize(window, winW, winH);
		*/
	} else if (t == Maximized) {
		lastX = winX;
		lastY = winY;
		lastW = winW;
		lastH = winH;

		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);

		glfwSetWindowPos(window, 0, 0);
		glfwSetWindowSize(window, mode->width, mode->height-40);
	} else if (t == Fullscreen) {
		lastX = winX;
		lastY = winY;
		lastW = winW;
		lastH = winH;
		
		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);
		
		glfwSetWindowPos(window, 0, 0);
		glfwSetWindowSize(window, mode->width, mode->height-1);
		
	}
	windowType2 = windowType;
	windowType = t;
}
/*
void RenderBorder() {
	BindShader("gui");
	SwitchBlendDepth(true);
	if (windowType == WindowBorder) {
		glViewport(0, 0, WWidth(), WHeight());

		GuiTransform(-1, 0);
		GuiColor(1, 1, 1, 1);
		BindTexture("blank");
		border.Draw();

		glViewport(8, 8, Width(), Height());

	} else if (windowType == Maximized) {
		glViewport(0, 0, WWidth(), WHeight());

		GuiTransform(-1, 0);
		GuiColor(1, 1, 1, 1);
		BindTexture("blank");
		border.Draw();

		glViewport(0, 31, Width(), Height());
	} else if (windowType == Fullscreen||windowType==WindowBorderless) {
		glViewport(0, 0, Width(), Height());
	}
}*/

Shader* currentShader;
std::map<std::string, Shader*> shaders;
void BindShader(std::string name) {
	Shader* shad = GetShader(name);
	if (shad != nullptr) {
		currentShader = shad;
		shad->Bind();
	}
}
void AddShader(std::string name, std::string path) {
	if(FHFileExist(path + ".shader")){
		shaders[name] = new Shader(path);
	} else {
		std::cout << "[Shader] Not Found: " << path <<".shader"<< std::endl;
	}
}
Shader* GetShader(std::string name){
	if (name == "current") {

		if (currentShader != nullptr)
			return currentShader;
	}else{
		Shader* shad = shaders[name];
		if (shad != nullptr)
			return shad;
	}
	return nullptr;
}
void GuiTransform(float x, float y) {
	// Bind Shader
	Shader* shad = shaders["gui"];
	if (shad != nullptr)
		shad->SetUniform2f("uTransform", x, y);// 2.f*x / sWidth() - 1, 1 - 2.f*y / sHeight());
}
void GuiColor(float f0, float f1,float f2,float f3) {
	// Bind Shader
	Shader* shad = shaders["gui"];
	if (shad != nullptr)
		shad->SetUniform4f("uColor", f0, f1, f2, f3);
}
void ObjectTransform(glm::mat4 m) {
	// Bind Shader
	if (currentShader != nullptr)
		currentShader->SetUniformMat4fv("uTransform", m);
}
void ObjectProjection(glm::mat4 m) {
	// Bind Shader
	if(currentShader !=nullptr)
		currentShader->SetUniformMat4fv("uProj", m);
}
void ObjectColor(float r,float g,float b,float a) {
	// Bind Shader
	if (currentShader != nullptr)
		currentShader->SetUniform4f("uColor",r,g,b,a);
}

std::map<std::string, Mesh*> meshes;
/*
Closest Lights need to be bound before drawing mesh
*/
void DrawMesh(std::string meshid, glm::mat4 trans) {
	Mesh* me = meshes[meshid];
	if (me != nullptr) {
		ObjectTransform(trans);
		BindTexture(me->tex);
		me->container.Draw();
	}
}
void AddMesh(std::string name, std::string path) {
	Mesh* me = new Mesh();
	int err = FHLoadMesh(me, path);
	if (err == FHSuccess){
		meshes[name] = me;
	} else if (err==FHNotFound){
		std::cout << "[Mesh] Not Found: " << path<<".mesh" << std::endl;;
		delete me;
	} else if (err == FHCorrupt) {
		std::cout << "[Mesh] Corrupt: " << path << ".mesh" << std::endl;;
		delete me;
	}
}
glm::vec3 GetMeshPos(std::string name) {
	Mesh* me = meshes[name];
	if (me != nullptr)
		return me->position;
	else
		return glm::vec3(0, 0, 0);
}
glm::vec3 GetMeshRot(std::string name) {
	Mesh* me = meshes[name];
	if (me != nullptr)
		return me->rotation;
	else
		return glm::vec3(0, 0, 0);
}

std::map<std::string, AnimationData*> animations;
void AddAnimation(std::string name, std::string path) {
	AnimationData* an = new AnimationData();
	int err = FHLoadAnim(an, path);
	if (err == FHSuccess){
		animations[name] = an;
	}else if(err==FHNotFound) {
		std::cout << "[Animation] Not Found: " << path<<".anim" << std::endl;;
		delete an;
	} else if (err == FHCorrupt) {
		std::cout << "[Animation] Corrupt: " << path << ".anim" << std::endl;;
		delete an;
	}
}
AnimationData* GetAnimation(std::string name) {
	if (animations[name] == nullptr)
		std::cout << "[Animation] '" << name << "' does not exist" << std::endl;
	
	return animations[name];
}

std::map<std::string, Texture*> textures;
void BindTexture(std::string name) {
	if (textures[name] != nullptr) {
		textures[name]->Bind();
	}
}
void AddTexture(std::string name, std::string path) {
	if (FHFileExist(path+".png")) {
		textures[name] = new Texture(path);
	} else {
		std::cout << "[Texture] Not Found: " << path+".png" << std::endl;
	}
}

std::vector<Light*> lights;
void AddLight(Light* l) {
	lights.push_back(l);
}
void DelLight(Light* l) {
	for (int i = 0; i < lights.size(); i++) {
		if (lights[i] == l) {
			lights.erase(lights.begin() + i);
			break;
		}
	}
}
void BindLights(std::string name,glm::vec3 pos) {
	Shader* shad=GetShader(name);
	if (shad != nullptr) {
		if (lights.size() > 4) {
			Light* lits[4];
			float dist[4]{ 9999,9999,9999,9999 };
			for (int i = 0; i < lights.size(); i++) {
				for (int j = 0; j < 4; j++) {
					if (lits[j] == nullptr) {
						lits[j] = lights[i];
						break;
					} else {
						float dis = glm::length(lights[i]->position - pos);
						if (dis < dist[j]) {
							lits[j] = lights[i];
							dist[j] = dis;
							break;
						}
					}
				}
			}
			if (lights.size() > 0) {
				for (int i = 0; i < 4; i++) {
					glm::vec3 p = lits[i]->position;
					glm::vec3 d = lits[i]->diffuse;
					glm::vec3 a = lits[i]->attune;
					std::string u = "uLights[" + i + (std::string)"].";
					shad->SetUniform3f(u + "position", p.x, p.y, p.z);
					shad->SetUniform1f(u + "range", lights[i]->range);
					shad->SetUniform3f(u + "diffuse", d.x, d.y, d.z);
					shad->SetUniform3f(u + "attune", a.x, a.y, a.z);
				}
			}
		} else {
			for (int i = 0; i < lights.size(); i++) {
				glm::vec3 p = lights[i]->position;
				glm::vec3 d = lights[i]->diffuse;
				glm::vec3 a = lights[i]->attune;
				std::string u = "uLights[" + i + (std::string)"].";
				shad->SetUniform3f(u + "position", p.x, p.y, p.z);
				shad->SetUniform1f(u + "range", lights[i]->range);
				shad->SetUniform3f(u + "diffuse", d.x, d.y, d.z);
				shad->SetUniform3f(u + "attune", a.x, a.y, a.z);
			}
		}
	}
}