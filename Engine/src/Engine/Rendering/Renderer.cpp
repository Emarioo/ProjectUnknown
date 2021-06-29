#include "Renderer.h"

#include "Handlers/AssetHandler.h"

namespace engine {
	
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
	float ToFloatScreenX(int x) {
		return 2 * x / Width() - 1;
	}
	float ToFloatScreenY(int y) {
		return 1 - 2 * y / Height();
	}
	float ToFloatScreenW(float w) {
		return 2.f * (w) / Width();
	}
	float ToFloatScreenH(float h) {
		return 2.f * (h) / Height();
	}
	float PercentToFloatScreenW(float w) {
		return 2 * w;
	}
	float PercentToFloatScreenH(float h) {
		return 2 * h;
	}
	glm::mat4 projMatrix;
	glm::mat4 viewMatrix;
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
	void UpdateViewMatrix() {
		viewMatrix = glm::inverse(
			glm::translate(glm::mat4(1.0f), camera.position) *
			glm::rotate(camera.rotation.y, glm::vec3(0, 1, 0)) *
			glm::rotate(camera.rotation.x, glm::vec3(1, 0, 0))
		);
	}

	// Dragging

	// Resize
	int resizingWin = 0;

	float renMouseX;
	float renMouseY;
	float GetMouseX() {
		return renMouseX;
	}
	float GetMouseY() {
		return renMouseY;
	}
	float GetFloatMouseX() {
		return ToFloatScreenX(renMouseX);
	}
	float GetFloatMouseY() {
		return ToFloatScreenY(renMouseY);
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
	std::function<void(int, int)> KeyEvent=nullptr;
	void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
		if (KeyEvent != nullptr) {
			KeyEvent(key, action);
		}
	}
	std::function<void(double,double,int,int)> MouseEvent=nullptr;
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
	std::function<void(double)> ScrollEvent=nullptr;
	void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
		if (ScrollEvent != nullptr) {
			ScrollEvent(yoffset);
		}
	}
	float cameraSensitivity = 0.1f;// TODO: MOVE THIS SOMEWHERE
	std::function<void(double,double)> DragEvent=nullptr;
	void DragCallback(GLFWwindow* window, double mx, double my) {
		if (!GetCursorMode()) {
			camera.rotation.y -= (mx - renMouseX) * (3.14159f / 360) * cameraSensitivity;
			camera.rotation.x -= (my - renMouseY) * (3.14159f / 360) * cameraSensitivity;
		}
		if (DragEvent != nullptr)
			DragEvent(mx, my);
		renMouseX = mx;
		renMouseY = my;
	}
	std::function<void(int,int)> ResizeEvent=nullptr;
	void ResizeCallback(GLFWwindow* window, int width, int height) {
		if (windowType != Minimized)
			SetWinSize(-1, -1, width, height);
		glViewport(0, 0, Width(), Height());
		if (ResizeEvent != nullptr) {
			ResizeEvent(width, height);
		}
	}
	std::function<void(int)> FocusEvent=nullptr;
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
	void SetInterfaceCallbacks(
		std::function<void(int, int)> key,
		std::function<void(double, double, int, int)> mouse,
		std::function<void(double)> scroll,
		std::function<void(double, double)> drag,
		std::function<void(int, int)> resize,
		std::function<void(int)> focus) {
		KeyEvent = key;
		MouseEvent = mouse;
		ScrollEvent = scroll;
		DragEvent = drag;
		ResizeEvent = resize;
		FocusEvent = focus;
	}
	const int TEXT_BATCH = 40;
	float verts[4 * 4 * TEXT_BATCH];
	BufferContainer textContainer;
	BufferContainer rectContainer;
	BufferContainer uvRectContainer;

	std::unordered_map<ShaderType, Shader*> shaders;
	Shader* currentShader = nullptr;
	bool BindShader(ShaderType type) {
		if (shaders.count(type) > 0) {
			currentShader = shaders[type];
			currentShader->Bind();
			return true;
		}
		return false;
	}
	void AddShader(ShaderType type, const std::string& name) {
		std::string path = "assets/shaders/" + name;
		if (FileExist(path + ".shader")) {
			shaders[type] = new Shader(path);
		} else {
			bug::out <bug::RED< "Cannot find '" < path < ".shader'" < bug::end;
		}
	}
	Shader* GetShader(ShaderType type) {
		if (type == ShaderCurrent)
			return currentShader;
		if (shaders.count(type) > 0)
			return shaders[type];
		return nullptr;
	}
	Shader* GetShader() {
		return currentShader;
	}

	void InitRenderer() {
		std::cout << "Waiting for glfw" << std::endl;
		if (!glfwInit()) {
			std::cout << "Not Init Window!" << std::endl;
			return;
		}
		std::cout << "Done with GLFW" << std::endl;
		const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

		SetWinSize(mode->width / 6, mode->height / 6, mode->width / 1.5, mode->height / 1.5);
		//SetWinSize(mode->width / 6, mode->height / 6, 500, 500);

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
		std::cout << "Created window" << std::endl;

		//SetWindowType(WindowBorder);
		SetWindowType(WindowBorderless);

		glfwMakeContextCurrent(window);
		if (glewInit() != GLEW_OK) {
			std::cout << "Error!" << std::endl;
			return;
		}
		std::cout << "Created Context" << std::endl;
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
		//glEnable(GL_FRAMEBUFFER_SRGB);
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

		AddShader(ShaderColor,"color");
		AddShader(ShaderColorBone,"colorWeight");
		
		AddShader(ShaderUV,"texture");
		AddShader(ShaderUVBone,"textureWeight");
		AddShader(ShaderInterface,"interface");
		AddShader(ShaderOutline,"outline");
		AddShader(ShaderLight,"lightSource");
		AddShader(ShaderDepth,"simpleDepth");
		AddShader(ShaderExperiment,"experiment");
		AddShader(ShaderTerrain,"terrain");

		AddTextureAsset("blank");

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

		uvRectContainer.Setup(true, temp, 16, temp2, 6);
		uvRectContainer.SetAttrib(0, 4, 4, 0);

		BindShader(ShaderInterface);
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
	
	void GuiTransform(float x, float y) {
		// Bind Shader
		Shader* shad = GetShader(ShaderInterface);
		if (shad!=nullptr)
			shad->SetVec2("uTransform", { x, y });
	}
	void GuiSize(float w, float h) {
		// Bind Shader
		Shader* shad = GetShader(ShaderInterface);
		if (shad != nullptr)
			shad->SetVec2("uSize", { w, h });
	}
	void GuiColor(float f0, float f1, float f2, float f3) {
		// Bind Shader
		Shader* shad = GetShader(ShaderInterface);
		if (shad != nullptr)
			shad->SetVec4("uColor", f0, f1, f2, f3);
	}
	void PassTransform(glm::mat4 m) {
		if (currentShader != nullptr)
			currentShader->SetMatrix("uTransform", m);
	}
	void PassProjection() {
		if (currentShader != nullptr)
			currentShader->SetMatrix("uProj", projMatrix * viewMatrix);
	}
	void PassColor(float r, float g, float b, float a) {
		if (currentShader != nullptr)
			currentShader->SetVec4("uColor", r, g, b, a);
	}
	void BindTexture(int slot, const std::string& name) {
		Texture* texture = GetTextureAsset(name);
		if (texture != nullptr) {
			texture->Bind(slot);
		} else {
			bug::out < bug::RED <  "Cannot find texture '" < name < "'\n";
		}
	}
	void BindTexture(int slot, Texture* texture) {
		if (texture != nullptr) {
			texture->Bind(slot);
		} else {
			bug::out < bug::RED < "Texture is null '" < texture->filepath <"'\n";
		}
	}
	
	void DrawString(Font* font, const std::string& text, bool center,float charHeight,int atChar) {
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

		//float charWidth = charHeight* (font->charWid[65] / (float)font->charSize)/(renderer::Wid()/renderer::Hei());

		float ratio = charHeight * ((float)Height() / Width());
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
			float markerX = atX+wid * ((charHeight / (Wid() / Hei())) / font->charSize);
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
			} else {

				float wuv = font->charWid[cha] / (float)font->imgSize;
				float wxy = ratio * (font->charWid[cha] / (float)font->charSize);
				float u = (cha % 16);
				float v = 15 - (cha / 16);

				Insert4(verts, 16 * i, atX, atY, (u) / 16, (v) / 16);
				Insert4(verts, 4 + 16 * i, atX, atY + charHeight, (u) / 16, (v + 1) / 16);
				Insert4(verts, 8 + 16 * i, atX + wxy, atY + charHeight, (u) / 16 + wuv, (v + 1) / 16);
				Insert4(verts, 12 + 16 * i, atX + wxy, atY, (u) / 16 + wuv, (v) / 16);

				atX += wxy;
			}
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

		///bug::out < "end" < bug::end;
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
	void DrawUVRect(float x, float y, float xw, float yh, float u, float v, float uw, float vh) {
		GuiTransform(0, 0);
		GuiSize(1, 1);
		GuiColor(1, 1, 1, 1);
		float vertices[16]{
			x,y,u,v,
			x,y+yh,u,v+vh,
			x + xw,y + yh,u + uw,v + vh,
			x+xw,y,u+uw,v
		};
		uvRectContainer.SubVB(0, 16, vertices);
		uvRectContainer.Draw();
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
	 If one of the four closest light are already bound then don't rebind them
	*/
	void BindLights(glm::vec3 objectPos) {
		if (currentShader != nullptr) {
			// List setup
			const int N_POINTLIGHTS = 4;
			const int N_SPOTLIGHTS = 2;

			DirLight* dir = nullptr;
			PointLight* points[N_POINTLIGHTS];
			float pDist[N_POINTLIGHTS];
			for (int i = 0; i < N_POINTLIGHTS; i++) {
				points[i] = nullptr;
				pDist[i] = 9999;
			}

			SpotLight* spots[N_SPOTLIGHTS];
			float sDist[N_SPOTLIGHTS];
			for (int i = 0; i < N_SPOTLIGHTS; i++) {
				spots[i] = nullptr;
				sDist[i] = 9999;
			}
			glm::ivec3 lightCount(0);

			// Grab the closest lights to the object
			for (int i = 0; i < lights.size(); i++) {
				Light* light = lights[i];
				if (light->lightType == LightType::Direction) {
					dir = reinterpret_cast<DirLight*>(light);
				} else if (light->lightType == LightType::Point) {
					PointLight* l = reinterpret_cast<PointLight*>(light);
					float distance = glm::length(l->position - objectPos);
					for (int i = 0; i < N_POINTLIGHTS; i++) {
						if (points[i] == nullptr) {
							points[i] = l;
							pDist[i] = distance;
							break;
						} else {
							if (distance < pDist[i]) {
								PointLight* pTemp = points[i];
								float dTemp = pDist[i];
								points[i] = l;
								pDist[i] = distance;
							}
						}
					}
				} else if (light->lightType == LightType::Spot) {
					SpotLight* l = reinterpret_cast<SpotLight*>(light);
					float distance = glm::length(l->position - objectPos);
					for (int i = 0; i < N_SPOTLIGHTS; i++) {
						if (spots[i] == nullptr) {
							spots[i] = l;
							sDist[i] = distance;
							break;
						} else {
							if (distance < sDist[i]) {
								SpotLight* lTemp = spots[i];
								float dTemp = sDist[i];
								spots[i] = l;
								sDist[i] = distance;
							}
						}
					}
				}
			}

			// Pass light to shader
			if (dir != nullptr) {
				PassLight(dir);
				lightCount.x++;
			}

			for (int i = 0; i < N_POINTLIGHTS; i++) {
				if (points[i] != nullptr) {
					PassLight(i, points[i]);
					lightCount.y++;
				} else
					break;
			}
			for (int i = 0; i < N_SPOTLIGHTS; i++) {
				if (spots[i] != nullptr) {
					PassLight(i, spots[i]);
					lightCount.z++;
				} else
					break;
			}
			currentShader->SetIVec3("uLightCount", lightCount);
		}
	}
	std::vector<Light*>& GetLights() {
		return lights;
	}
	void PassLight(DirLight* light) {
		if (light != nullptr) {
			currentShader->SetVec3("uDirLight.ambient", light->ambient);
			currentShader->SetVec3("uDirLight.diffuse", light->diffuse);
			currentShader->SetVec3("uDirLight.specular", light->specular);
			currentShader->SetVec3("uDirLight.direction", light->direction);
		} else {
			glm::vec3 none(0);
			currentShader->SetVec3("uDirLight.ambient", none);
			currentShader->SetVec3("uDirLight.diffuse", none);
			currentShader->SetVec3("uDirLight.specular", none);
			currentShader->SetVec3("uDirLight.direction", none);
		}
	}
	void PassLight(int index, PointLight* light) {
		std::string u = "uPointLights[" + index + (std::string)"].";
		if (light != nullptr) {
			currentShader->SetVec3(u + "ambient", light->ambient);
			currentShader->SetVec3(u + "diffuse", light->diffuse);
			currentShader->SetVec3(u + "specular", light->specular);
			currentShader->SetVec3(u + "position", light->position);
			currentShader->SetFloat(u + "constant", light->constant);
			currentShader->SetFloat(u + "linear", light->linear);
			currentShader->SetFloat(u + "quadratic", light->quadratic);
		} else {
			glm::vec3 none(0);
			currentShader->SetVec3(u + "ambient", none);
			currentShader->SetVec3(u + "diffuse", none);
			currentShader->SetVec3(u + "specular", none);
			currentShader->SetVec3(u + "position", none);
			currentShader->SetFloat(u + "constant", 0);
			currentShader->SetFloat(u + "linear", 0);
			currentShader->SetFloat(u + "quadratic", 0);
		}
	}
	void PassLight(int index, SpotLight* light) {
		std::string u = "uSpotLights[" + index + (std::string)"].";
		if (light!=nullptr) {
			currentShader->SetVec3(u + "ambient", light->ambient);
			currentShader->SetVec3(u + "diffuse", light->diffuse);
			currentShader->SetVec3(u + "specular", light->specular);
			currentShader->SetVec3(u + "position", light->position);
			currentShader->SetVec3(u + "direction", light->direction);
			currentShader->SetFloat(u + "cutOff", light->cutOff);
			currentShader->SetFloat(u + "outerCutOff", light->outerCutOff);
		} else {
			glm::vec3 none(0);
			currentShader->SetVec3(u + "ambient", none);
			currentShader->SetVec3(u + "diffuse", none);
			currentShader->SetVec3(u + "specular", none);
			currentShader->SetVec3(u + "position", none);
			currentShader->SetVec3(u + "direction", none);
			currentShader->SetFloat(u + "cutOff", 0);
			currentShader->SetFloat(u + "outerCutOff", 0);
		}
	}
	void PassMaterial(int index, Material* material) {
		if (!material->diffuse_map.empty()) {
			//bug::out < material->diffuse_map < bug::end;
			//bug::out < (currentShader==GetShader(ShaderColorBone)) < bug::end;
			BindTexture(index+1, material->diffuse_map);// + 1 because of shadow_map on 0
			currentShader->SetInt("uMaterials["+std::to_string(index)+"].diffuse_map", index+1);
		}
		currentShader->SetVec3("uMaterials[" + std::to_string(index) + "].diffuse_color", material->diffuse_color);
		currentShader->SetVec3("uMaterials[" + std::to_string(index) + "].specular", material->specular);
		currentShader->SetFloat("uMaterials[" + std::to_string(index) + "].shininess", material->shininess);
	}
}