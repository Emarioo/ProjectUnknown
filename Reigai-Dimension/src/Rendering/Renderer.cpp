#include "Renderer.h"

namespace renderer {
	
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
	void SetWinSize(int x, int y, int w, int h) {
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
		return 1920;
	}
	float Hei() {
		return 1080;
	}
	/*
	Width of game screen not window!
	*/
	float Width() {
		if (windowType == WindowBorder) {
			return winW - 8 - 8;
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
		} else if (windowType == Maximized) {
			return winH - 31;
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
	bool hasFocus = true;
	bool HasFocus() {
		return hasFocus;
	}
	float AlterW(float w) {
		return 2.f * (w) / Width();
	}
	float AlterH(float h) {
		return 2.f * (h) / Height();
	}
	float AlterSW(float w) {
		return 2 * w;
	}
	float AlterSH(float h) {
		return 2 * h;
	}
	float AlterX(int x) {
		return 2 * x / Width() - 1;
	}
	float AlterY(int y) {
		return 1 - 2 * y / Height();
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
			glm::rotate(glm::mat4(1.0f), camera.rotation.y, glm::vec3(0, 1, 0)) *
			glm::rotate(glm::mat4(1.0f), camera.rotation.x, glm::vec3(1, 0, 0));

		BindShader(MaterialType::TextureMat);
		ObjectProjection(projMatrix * glm::inverse(trans));
		BindShader(MaterialType::ColorMat);
		ObjectProjection(projMatrix * glm::inverse(trans));
		BindShader(MaterialType::InterfaceMat);
		ObjectProjection(projMatrix * glm::inverse(trans));
		BindShader(MaterialType::AnimationMat);
		ObjectProjection(projMatrix * glm::inverse(trans));
		BindShader(MaterialType::OutlineMat);
		ObjectProjection(projMatrix * glm::inverse(trans));
	}

	// Dragging

	// Resize
	int resizingWin = 0;

	double renMouseX;
	double renMouseY;
	double GetMX() {
		return renMouseX;
	}
	double GetMY() {
		return renMouseY;
	}
	GLFWwindow* window;
	GLFWwindow* GetWindow() {
		return window;
	}
	bool InsideBorder() {
		if (windowType == WindowBorder)
			return (renMouseX >= 8 && renMouseX < WWidth() - 8 && renMouseY >= 31 && renMouseY < WHeight() - 8);
		else if (windowType == Maximized)
			return (renMouseY >= 31);
		return true;
	}
	BufferContainer border;
	void (*KeyEvent)(int, int);
	void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
		if (KeyEvent != nullptr) {
			KeyEvent(key, action);
		}
	}
	void(*MouseEvent)(double, double, int, int);
	void MouseCallback(GLFWwindow* window, int button, int action, int mods) {
		if (windowType == Fullscreen || windowType == WindowBorderless) {
			if (MouseEvent != nullptr)
				MouseEvent(renMouseX, renMouseY, button, action);

		} else if (windowType == WindowBorder) {
			if (button == 0) {
				if (action == 1) {
					// Check Buttons
					if (renMouseY < 8) {
						resizingWin = 1;
						return;
					} else if (renMouseX < 8) {
						resizingWin = 2;
						return;
					} else if (renMouseY > WHeight() - 8) {
						resizingWin = 3;
						return;
					} else if (renMouseX > WWidth() - 8) {
						resizingWin = 4;
						return;
					}
				} else if (action == 0) {
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
	void (*ScrollEvent)(double, double);
	void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
		if (ScrollEvent != nullptr) {
			ScrollEvent(xoffset, yoffset);
		}
	}
	void(*DragEvent)(double, double);
	void DragCallback(GLFWwindow* window, double mx, double my) {
		if (!GetCursorMode()) {
			camera.rotation.y -= (mx - renMouseX) * (3.14159f / 360) * 0.18;
			camera.rotation.x -= (my - renMouseY) * (3.14159f / 360) * 0.18;
		}
		if (DragEvent != nullptr)
			DragEvent(mx, my);
		renMouseX = mx;
		renMouseY = my;
	}
	void(*ResizeEvent)(int, int);
	void ResizeCallback(GLFWwindow* window, int width, int height) {
		if (windowType != Minimized)
			SetWinSize(-1, -1, width, height);
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
		void(*key)(int, int),
		void(*mouse)(double, double, int, int),
		void(*scroll)(double, double),
		void(*drag)(double, double),
		void(*resize)(int, int),
		void(*focus)(int)) {
		KeyEvent = key;
		MouseEvent = mouse;
		ScrollEvent = scroll;
		DragEvent = drag;
		ResizeEvent = resize;
		FocusEvent = focus;
	}
	const int TEXT_BATCH = 30;
	float verts[4 * 4 * TEXT_BATCH];
	BufferContainer textContainer;
	BufferContainer rectContainer;
	Shader colorShader;
	Shader textureShader;
	Shader interfaceShader;
	Shader animationShader;
	Shader outlineShader;
	Shader* currentShader = nullptr;
	void Init() {
		if (!glfwInit()) {
			std::cout << "Not Init Window!" << std::endl;
			return;
		}
		const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

		SetWinSize(mode->width / 6, mode->height / 6, mode->width / 1.5, mode->height / 1.5);

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
		glfwSetScrollCallback(window, ScrollCallback);
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

		colorShader.Init("assets/shaders/color");
		textureShader.Init("assets/shaders/texture");
		interfaceShader.Init("assets/shaders/interface");
		animationShader.Init("assets/shaders/animation");
		outlineShader.Init("assets/shaders/outline");
		//terrainShader.Init("assets/shaders/terrain");

		AddTexture("blank", "assets/textures/blank");

		std::uint32_t indes[TEXT_BATCH*6];
		for (int i = 0; i < TEXT_BATCH; i++) {
			indes[0 + 6 * i] = 2 + 4 * i;
			indes[1 + 6 * i] = 1 + 4 * i;
			indes[2 + 6 * i] = 0 + 4 * i;
			indes[3 + 6 * i] = 0 + 4 * i;
			indes[4 + 6 * i] = 3 + 4 * i;
			indes[5 + 6 * i] = 2 + 4 * i;
		}
		textContainer.Setup(true,nullptr,4*4*TEXT_BATCH,indes,6*TEXT_BATCH);
		textContainer.SetAttrib(0, 4, 4, 0);
		float temp[]{
			-0.5, -0.5, 0, 0,
			-0.5, 0.5, 0, 1,
			0.5, 0.5, 1, 1,
			0.5, -0.5, 1, 0
		};
		unsigned int temp2[]{
			2,1,0,
			0,3,2
		};
		rectContainer.Setup(false, temp,16, temp2, 6);
		rectContainer.SetAttrib(0, 4, 4, 0);

		BindShader(MaterialType::InterfaceMat);
		GuiSize(1, 1);

		fov = 90.f;
		zNear = 0.1f;
		zFar = 400.f;
		SetProjection((float)Width() / Height());
	}
	bool cursorMode = true;
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
			if (windowType != WindowBorder && windowType != WindowBorderless) {
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
			glfwSetWindowSize(window, mode->width, mode->height - 40);
		} else if (t == Fullscreen) {
			lastX = winX;
			lastY = winY;
			lastW = winW;
			lastH = winH;

			GLFWmonitor* monitor = glfwGetPrimaryMonitor();
			const GLFWvidmode* mode = glfwGetVideoMode(monitor);

			glfwSetWindowPos(window, 0, 0);
			glfwSetWindowSize(window, mode->width, mode->height - 1);

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
	
	void BindShader(MaterialType type) {
		if (type == MaterialType::ColorMat) {
			currentShader = &colorShader;
		}else if (type == MaterialType::TextureMat) {
			currentShader = &textureShader;
		}else if (type == MaterialType::InterfaceMat) {
			currentShader = &interfaceShader;
		} else if (type == MaterialType::AnimationMat) {
			currentShader = &animationShader;
		} else if (type == MaterialType::OutlineMat) {
			currentShader = &outlineShader;
		}
		if (currentShader != nullptr) {
			currentShader->Bind();
		}
	}
	/*
	void AddShader(std::string name, std::string path) {
		if (FHFileExist(path + ".shader")) {
			shaders[name] = new Shader(path);
		} else {
			std::cout << "[Shader] Not Found: " << path << ".shader" << std::endl;
		}
	}*/
	/*
	Returns nullptr if error?
	*/
	Shader* GetShader(MaterialType type) {// TODO: Change into referance?
		if (type == MaterialType::ColorMat) {
			return &colorShader;
		} else if (type == MaterialType::TextureMat) {
			return &textureShader;
		} else if (type == MaterialType::InterfaceMat) {
			return &interfaceShader;
		} else if (type == MaterialType::AnimationMat) {
			return &animationShader;
		} else if (type == MaterialType::OutlineMat) {
			return &outlineShader;
		}
		return nullptr;
	}
	void GuiTransform(float x, float y) {
		// Bind Shader
		if (interfaceShader.IsInitiliazed())
			interfaceShader.SetUniform2f("uTransform", x, y);// 2.f*x / sWidth() - 1, 1 - 2.f*y / sHeight());
	}
	void GuiSize(float w, float h) {
		// Bind Shader
		if (interfaceShader.IsInitiliazed())
			interfaceShader.SetUniform2f("uSize", w, h);
	}
	void GuiColor(float f0, float f1, float f2, float f3) {
		// Bind Shader
		if(interfaceShader.IsInitiliazed())
			interfaceShader.SetUniform4f("uColor", f0, f1, f2, f3);
	}
	void ObjectTransform(glm::mat4 m) {
		// Bind Shader
		if (currentShader != nullptr)
			currentShader->SetUniformMat4fv("uTransform", m);
	}
	void ObjectProjection(glm::mat4 m) {
		// Bind Shader
		if (currentShader != nullptr)
			currentShader->SetUniformMat4fv("uProj", m);
	}
	void ObjectColor(float r, float g, float b, float a) {
		// Bind Shader
		if (currentShader != nullptr)
			currentShader->SetUniform4f("uColor", r, g, b, a);
	}

	void DrawMesh(MeshData* meshData, glm::mat4 trans) {
		if (meshData != nullptr) {
			ObjectTransform(trans);
			if(meshData->material==MaterialType::TextureMat)
				BindTexture(meshData->texture);
			
			meshData->container.Draw();
		}
	}

	std::unordered_map<std::string, Texture> textures;
	void BindTexture(const std::string& name) {
		if (textures.count(name)>0) {
			textures[name].Bind();
		} else {
			bug::out < bug::RED <  "Cannot find texture '" < name < "'\n";
		}
	}
	void AddTexture(const std::string& name, std::string path) {// Change this system so it is simular to every other system. FileExist?
		if (fManager::FileExist(path + ".png")) {
			textures[name] = Texture(path);
		} else {
			bug::out < bug::RED < "Cannot find Texture '" < path < ".png'\n";
		}
	}
	void Insert4(float* ar, int ind, float f0, float f1, float f2, float f3) {
		ar[ind] = f0;
		ar[ind + 1] = f1;
		ar[ind + 2] = f2;
		ar[ind + 3] = f3;
	}
	
	void DrawText(Font* font, const std::string& text, bool center,float charHeight,int atChar) {
		// Setup
		
			//bug::out < atChar < bug::end;
		if (text.length() == 0 && atChar==-1)
			return;
		if (font != nullptr) {
			if (font->texture != nullptr)
				font->texture->Bind();
			else
				return;
		} else
			return;
		//charHeight = 0.5;
		//float charWidth = charHeight* (font->charWid[65] / (float)font->charSize)/(renderer::Wid()/renderer::Hei());

		float ratio = charHeight * ((float)renderer::Height() / renderer::Width());
		float maxHeight = charHeight;
		float maxWidth = 0;
		float temp = 0;
		for (int i = 0; i < text.size();i++) {
			unsigned char cha = text[i];
			if (cha == '\n') {
				if (temp> maxWidth)
					maxWidth = temp;
				temp = 0;
				maxHeight+=charHeight;
				continue;
			}
			temp += ratio * (font->charWid[cha] / (float)font->charSize);
		}
		if (temp > maxWidth)
			maxWidth = temp;
		temp = 0;
		float leftX = 0, topY = 0;
		if (center) {
			leftX = -maxWidth / 2;
			topY = maxHeight / 2 - charHeight;
		}
		// Fill array with characters coords
		float atX = leftX;
		float atY = topY;
		int i = 0;
		int indChar = 0;
		if (atChar != -1) {
			float wid = 0;
			float hei = 0;
			for (int i = 0; i < atChar; i++) {
				int cha = text.at(i);
				if ((char)cha == '\n') {
					wid = 0;
					hei++;
					continue;
				}
				if (cha < 0) {
					cha += 256;
				}
				wid += font->charWid[cha];
			}
			float markerX = atX+wid * ((charHeight / (renderer::Wid() / renderer::Hei())) / font->charSize);
			float markerY = atY+hei * charHeight;
			float wuv = font->charWid[0] / (float)font->imgSize;
			float wxy = ratio * (font->charWid[0] / (float)font->charSize);
			float u = (0 % 16);
			float v = 15 - (0 / 16);

			Insert4(verts, 16 * i, markerX, markerY, (u) / 16, (v) / 16);
			Insert4(verts, 4 + 16 * i, markerX, markerY + charHeight, (u) / 16, (v + 1) / 16);
			Insert4(verts, 8 + 16 * i, markerX + wxy, markerY + charHeight, (u) / 16 + wuv, (v + 1) / 16);
			Insert4(verts, 12 + 16 * i, markerX + wxy, markerY, (u) / 16 + wuv, (v) / 16);

			i++;
		}
		for (unsigned char cha : text) {
			if (cha == '\n') {
				Insert4(verts, 16 * i, 0, 0, 0, 0);
				Insert4(verts, 4 + 16 * i, 0, 0, 0, 0);
				Insert4(verts, 8 + 16 * i, 0, 0, 0, 0);
				Insert4(verts, 12 + 16 * i, 0, 0, 0, 0);
				atY -= charHeight;
				atX = leftX;
				i++;
				continue;
			}

			float wuv = font->charWid[cha] / (float)font->imgSize;
			float wxy = ratio*(font->charWid[cha] / (float)font->charSize);
			float u = (cha % 16);
			float v = 15 - (cha / 16);
		
			Insert4(verts, 16 * i, atX, atY, (u) / 16, (v) / 16);
			Insert4(verts, 4 + 16 * i, atX, atY + charHeight, (u) / 16, (v + 1) / 16);
			Insert4(verts, 8 + 16 * i, atX + wxy, atY + charHeight, (u) / 16 + wuv, (v + 1) / 16);
			Insert4(verts, 12 + 16 * i, atX + wxy, atY, (u) / 16 + wuv, (v) / 16);
			
			atX += wxy;
			i++;
			if (i==TEXT_BATCH) {
				textContainer.SubVB(0, 4 * 4 * TEXT_BATCH, verts);
				textContainer.Draw();
				i = 0;
			}
		}
		// Fill the rest with space
		for (int i = ((atChar!=-1)+text.length())%TEXT_BATCH; i < TEXT_BATCH; i++) {
			for (int j = 0; j < 16; j++) {// stop if 10 characters are zeros (aka. \n) - might be more work than filling the rest with zeros
				verts[i * 16 + j] = 0;
			}
		}
		
		textContainer.SubVB(0, 4 * 4 * TEXT_BATCH, verts);
		textContainer.Draw();
	}
	void DrawRect() {
		rectContainer.Draw();
	}
	void DrawRect(float x, float y) {
		GuiTransform(x, y);
		rectContainer.Draw();
	}
	void DrawRect(float x, float y, float w, float h) {
		GuiTransform(x, y);
		GuiSize(w, h);
		rectContainer.Draw();
	}
	void DrawRect(float x, float y, float w, float h, float r, float g, float b, float a) {
		GuiTransform(x, y);
		GuiSize(w, h);
		GuiColor(r, g, b, a);
		rectContainer.Draw();
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
	/*
	Binds light to current shader
	*/
	void BindLights(glm::vec3 pos) {
		if (currentShader != nullptr) {
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
						currentShader->SetUniform3f(u + "position", p.x, p.y, p.z);
						currentShader->SetUniform1f(u + "range", lights[i]->range);
						currentShader->SetUniform3f(u + "diffuse", d.x, d.y, d.z);
						currentShader->SetUniform3f(u + "attune", a.x, a.y, a.z);
					}
				}
			} else {
				for (int i = 0; i < lights.size(); i++) {
					glm::vec3 p = lights[i]->position;
					glm::vec3 d = lights[i]->diffuse;
					glm::vec3 a = lights[i]->attune;
					std::string u = "uLights[" + i + (std::string)"].";
					currentShader->SetUniform3f(u + "position", p.x, p.y, p.z);
					currentShader->SetUniform1f(u + "range", lights[i]->range);
					currentShader->SetUniform3f(u + "diffuse", d.x, d.y, d.z);
					currentShader->SetUniform3f(u + "attune", a.x, a.y, a.z);
				}
			}
		}
	}
/*
void renderer::RenderMeshes() {
	renderer::SwitchBlendDepth(false);
	renderer::BindShader(MaterialType::ColorMat);
	for (MeshComponent* m : meshComponents) {
		if (m->mesh != nullptr) {
			if (m->mesh->material == MaterialType::ColorMat) {
				DrawMesh(m->mesh, m->matrix);
			}
		} else {
			if (!m->hasError) {
				bug::out + bug::RED + "MeshData in MeshComponent is nullptr\n";
				m->hasError = true;
			}
		}
	}
	renderer::BindShader(MaterialType::TextureMat);
	for (MeshComponent* m : meshComponents) {
		if (m->mesh != nullptr) {
			if (m->mesh->material == MaterialType::TextureMat) {
				DrawMesh(m->mesh, m->matrix);
			}
		}
	}
	//renderer::BindShader(MaterialType::OutlineMat);
	//glLineWidth(2.f);
	/*
	for (GameObject& o : *oManager::GetObjects()) {
		if (o.renderHitbox) {
			//if (o.collision.) { TODO: Check if collision has draw object?
			o.collision.Draw();
			//}
		}
	}
	*/
	/*
	if (oManager::GetPlayer()->renderHitbox) {
		oManager::GetPlayer()->collision.Draw();
	}
	*/
	/*
	for (GameObject* o : texRender) {
		o->Draw();
	}*/

	/*
	BindShader("terrain");
	if (loadedDim != nullptr) {
		BindTexture("blank");
		for (Chunk c : loadedDim->loadedChunks) {
			Location base;
			base.Translate(glm::vec3(c.x*(loadedDim->chunkSize), 0, c.z*(loadedDim->chunkSize)));
			ObjectTransform(base.mat());
			c.con.Draw();
		}
	}*/
}