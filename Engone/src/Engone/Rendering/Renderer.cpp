#include "gonpch.h"

#include "Renderer.h"

#include "../Handlers/AssetHandler.h"
#include "../Handlers/ObjectHandler.h"

namespace engine {
	
	WindowTypes windowType = NONE;
	WindowTypes GetWindowType() {
		return windowType;
	}
	int winX = 0;
	int winY = 0;
	int winW = 0;
	int winH = 0;

	GLFWwindow* window=nullptr;
	GLFWwindow* GetWindow() {
		return window;
	}
	void GetWindowPos(int* x, int* y) {
		if(window!=nullptr)
			glfwGetWindowPos(window, x, y);
	}
	void GetWindowSize(int* w, int* h) {
		if (window != nullptr)
			glfwGetWindowSize(window, w, h);
	}
	void SetWindowPos(int x, int y) {
		winX = x;
		winY = y;
		if(window!=nullptr)
			glfwSetWindowPos(window,x, y);
	}
	void SetWindowSize(int w, int h) {
		winW = w;
		winH = h;
		if (window != nullptr)
			glfwSetWindowSize(window, w, h);
	}
	/*
	Width of game screen not window!
	*/
	float Width() {
		int temp;
		glfwGetWindowSize(window, &temp, nullptr);
		return temp;
	}
	/*
	Height of game screen not window!
	*/
	float Height() {
		int temp;
		glfwGetWindowSize(window, nullptr, &temp);
		return temp;
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
	bool windowHasFocus = true;
	bool HasFocus() {
		return windowHasFocus;
	}
	std::function<void(int, int)> KeyEvent = nullptr;
	void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
		// This is for debug purposes
		if (key == GLFW_KEY_1) {
			SetWindowType(Windowed);
		}
		else if (key == GLFW_KEY_2) {
			SetWindowType(Fullscreen);
		}
		else if (key == GLFW_KEY_3) {
			SetWindowType(BorderlessFullscreen);
		}

		if (KeyEvent != nullptr) {
			KeyEvent(key, action);
		}
	}
	std::function<void(double, double, int, int)> MouseEvent = nullptr;
	void MouseCallback(GLFWwindow* window, int action, int button, int mods) {
		if (MouseEvent != nullptr)
			MouseEvent(renMouseX, renMouseY, action, button);
	}
	std::function<void(double)> ScrollEvent = nullptr;
	void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
		if (ScrollEvent != nullptr) {
			ScrollEvent(yoffset);
		}
	}
	float cameraSensitivity = 0.1f;// TODO: MOVE THIS SOMEWHERE
	std::function<void(double, double)> DragEvent = nullptr;
	void DragCallback(GLFWwindow* window, double mx, double my) {
		if (IsCursorLocked()) {
			GetCamera()->rotation.y -= (mx - renMouseX) * (3.14159f / 360) * cameraSensitivity;
			GetCamera()->rotation.x -= (my - renMouseY) * (3.14159f / 360) * cameraSensitivity;
		}
		if (DragEvent != nullptr)
			DragEvent(mx, my);
		renMouseX = mx;
		renMouseY = my;
	}
	std::function<void(int, int)> ResizeEvent = nullptr;
	void ResizeCallback(GLFWwindow* window, int width, int height) {
		glViewport(0, 0, width, height);
		if (windowType == Windowed) {
			winW = width;
			winH = height;
		}
		if (ResizeEvent != nullptr)
			ResizeEvent(width, height);
	}
	std::function<void(int)> FocusEvent = nullptr;
	void WindowFocusCallback(GLFWwindow* window, int focus) {
		windowHasFocus = focus;
		if (FocusEvent != nullptr) {
			FocusEvent(focus);
		}
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
	void MakeWindow(const char* title) {
		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);

		if (window != nullptr) {
			glfwDestroyWindow(window);
		}
		else {
			winX = mode->width / 6;
			winY = mode->height / 6;
			winW = mode->width / 1.5;
			winH = mode->height / 1.5;
		}

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		if (windowType == Windowed) {
			glfwWindowHint(GLFW_DECORATED, true);
			window = glfwCreateWindow(winW, winH, title, NULL, NULL);
			if (!window) {
				glfwTerminate();
				std::cout << "Terminate Window!" << std::endl;
				return;
			}

		}
		else if (windowType == Fullscreen) {
			glfwWindowHint(GLFW_DECORATED, true);
			window = glfwCreateWindow(mode->width, mode->height, title, monitor, NULL);
			if (!window) {
				glfwTerminate();
				std::cout << "Terminate Window!" << std::endl;
				return;
			}

		}
		else if (windowType == BorderlessFullscreen) {
			glfwWindowHint(GLFW_DECORATED, false);
			window = glfwCreateWindow(mode->width, mode->height, title, NULL, NULL);
			if (!window) {
				glfwTerminate();
				std::cout << "Terminate Window!" << std::endl;
				return;
			}
		}
		else {
			std::cout << "WindowType is None" << std::endl;
			return;
		}
		glfwMakeContextCurrent(window);

		glfwSetKeyCallback(window, KeyCallback);
		glfwSetMouseButtonCallback(window, MouseCallback);
		glfwSetScrollCallback(window, ScrollCallback);
		glfwSetCursorPosCallback(window, DragCallback);
		glfwSetWindowFocusCallback(window, WindowFocusCallback);
		glfwSetWindowSizeCallback(window, ResizeCallback);
	}
	void SetWindowType(WindowTypes t) {
		if (t == windowType)
			return;
		if (window == nullptr) {
			windowType = t;
			return;
		}
		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);
		
		if (windowType == Windowed && t == Fullscreen) {
			windowType = t;// do it before so the resize event has the current windowType and not the last one
			glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
		}
		else if (windowType == Windowed && t == BorderlessFullscreen) {
			windowType = t;
			MakeWindow("Project Unknown");
		}
		else if (windowType == Fullscreen && t == Windowed) {
			windowType = t;
			glfwSetWindowMonitor(window, NULL, winX, winY, winW, winH, mode->refreshRate);
		}
		else if (windowType == Fullscreen && t == BorderlessFullscreen) {
			windowType = t;
			MakeWindow("Project Unknown");
		}
		else if (windowType == BorderlessFullscreen && t == Windowed) {
			windowType = t;
			MakeWindow("Project Unknown");
		}
		else if (windowType == BorderlessFullscreen && t == Fullscreen) {
			windowType = t;
			MakeWindow("Project Unknown");
		}
		int wid;
		int hei;
		GetWindowSize(&wid, &hei);
		glViewport(0, 0, wid, hei);

		/*
		int wid = 0;
		int hei = 0;
		if (t == Windowed) {
			windowType = t;
			GLFWmonitor* monitor = glfwGetPrimaryMonitor();
			const GLFWvidmode* mode = glfwGetVideoMode(monitor);
			glfwSetWindowMonitor(window, NULL, winX, winY, winW, winH, mode->refreshRate);
			GetWindowSize(&wid, &hei);
			glViewport(0, 0, wid, hei);
		}
		else if (t == Fullscreen) {
			windowType = t;
			GLFWmonitor* monitor = glfwGetPrimaryMonitor();
			const GLFWvidmode* mode = glfwGetVideoMode(monitor);
			glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
			GetWindowSize(&wid, &hei);
			glViewport(0, 0, wid, hei);
		}
		else if (t == BorderlessFullscreen) {
			windowType = t;
			//glfwWindowHint(GLFW_DECORATED, false);
			GLFWmonitor* monitor = glfwGetPrimaryMonitor();
			const GLFWvidmode* mode = glfwGetVideoMode(monitor);
			glfwSetWindowMonitor(window, NULL, 0, 0, mode->width, mode->height, mode->refreshRate);
			GetWindowSize(&wid, &hei);
			glViewport(0, 0, wid, hei);
		}
		return;*/
	}
	
	glm::mat4 projMatrix;
	glm::mat4 viewMatrix;
	float fov;
	float zNear;
	float zFar;
	void SetProjection(float ratio) {
		projMatrix = glm::perspective(fov, ratio, zNear, zFar);
	}
	void UpdateViewMatrix(double lag) {
		viewMatrix = glm::inverse(
			glm::translate(glm::mat4(1.0f), GetCamera()->position+ GetCamera()->velocity*(float)lag) *
			glm::rotate(GetCamera()->rotation.y, glm::vec3(0, 1, 0)) *
			glm::rotate(GetCamera()->rotation.x, glm::vec3(1, 0, 0))
		);
	}
	
	const int TEXT_BATCH = 40;
	float verts[4 * 4 * TEXT_BATCH];
	TriangleBuffer textContainer;
	TriangleBuffer rectContainer;
	TriangleBuffer uvRectContainer;

	Shader shaders[MAX_CUSTOM_SHADERS+MAX_ENGINE_SHADERS];
	unsigned char boundShader = 0;
	bool BindShader(unsigned char shader) {
		if (shaders[shader].isInitialized) {
			boundShader = shader;
			shaders[shader].Bind();
			return true;
		}
		return false;
	}
	void AddShader(unsigned char shader, const std::string& _path) {
		std::string path = "assets/shaders/" + _path;
		if (FileExist(path + ".shader")) {
			shaders[shader].Init(path);
		} else {
			bug::out <bug::RED< "Cannot find '" < path < ".shader'" < bug::end;
		}
	}
	void AddShaderFromInclude(unsigned char shader, const std::string& text)
	{
		//std::string path = "assets/shaders/" + _path;
		//if (FileExist(path + ".shader")) {
		//	shaders[shader].Init(path);
		//}
		//else {
			//bug::out < bug::RED < "Cannot find '" < path < ".shader'" < bug::end;
		//}
	}
	Shader* GetShader(unsigned char shader) {
		if(shaders[shader].isInitialized)
			return &shaders[shader];
		return &shaders[ShaderType::NONE];
	}
	Shader* GetBoundShader() {
		if(shaders[boundShader].isInitialized)
			return &shaders[boundShader];
		return &shaders[ShaderType::NONE];
	}
	void InitRenderer() {
		if (!glfwInit()) {
			std::cout << "Not Init Window!" << std::endl;
			return;
		}

		windowType = Windowed;
		MakeWindow("Project Unknown");
		
		if (glewInit() != GLEW_OK) {
			std::cout << "Error!" << std::endl;
			return;
		}

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
		
		// These shaders from the engine are placed in the assets/shaders folder where custom shaders are placed.
		//  They should be compiled with the game in strings instead of loaded from files when starting game.
		AddShader(ShaderType::Color, "color");
		AddShader(ShaderType::ColorBone, "colorWeight");
		AddShader(ShaderType::UV, "texture");
		AddShader(ShaderType::UVBone, "textureWeight");
		AddShader(ShaderType::Interface, "interface");
		AddShader(ShaderType::Outline, "outline");
		AddShader(ShaderType::Depth, "simpleDepth");
		
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
		textContainer.Init(true,nullptr,4*4*TEXT_BATCH,indes,6*TEXT_BATCH);
		textContainer.SetAttrib(0, 4, 4, 0);
		float temp[]{
			0, 1, 0, 1,
			0, 0, 0, 0,
			1, 0, 1, 0,
			1, 1, 1, 1
		};
		unsigned int temp2[]{
			0,1,2,
			2,3,0
		};
		float temp3[]{
			-.5, -.5, 0, 0,
			-.5, .5, 0, 1,
			.5, .5, 1, 1,
			.5, -.5, 1, 0
		};
		unsigned int temp4[]{
			2,1,0,
			0,3,2
		};
		rectContainer.Init(false, temp,16, temp2, 6);
		rectContainer.SetAttrib(0, 4, 4, 0);

		uvRectContainer.Init(true, temp3, 16, temp4, 6);
		uvRectContainer.SetAttrib(0, 4, 4, 0);
		
		BindShader(ShaderType::Interface);
		SetSize(1, 1);
		SetRenderArea(-1, -1, 2, 2);
		
		fov = 90.f;
		zNear = 0.1f;
		zFar = 400.f;

		SetProjection((float)Width() / Height());
	}

	bool isCursorVisible = true;
	bool IsCursorVisible() {
		return isCursorVisible;
	}
	bool isCursorLocked = false;
	bool IsCursorLocked() {
		return isCursorLocked;
	}
	void SetCursorVisibility(bool f) {
		if (f) glfwSetInputMode(GetWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		else glfwSetInputMode(GetWindow(), GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
		isCursorVisible = f;
	}
	void LockCursor(bool f) {
		if (!f) {
			SetCursorVisibility(isCursorVisible);
			if (glfwRawMouseMotionSupported())
				glfwSetInputMode(GetWindow(), GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);
		} else {
			glfwSetInputMode(GetWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			if (glfwRawMouseMotionSupported())
				glfwSetInputMode(GetWindow(), GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
		}
		isCursorLocked = f;
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

	void SetTransform(glm::mat4 m) {
		if (GetBoundShader() != nullptr)
			GetBoundShader()->SetMatrix("uTransform", m);
	}
	void SetColor(float r, float g, float b, float a) {
		if (GetBoundShader() != nullptr)
			GetBoundShader()->SetVec4("uColor", r, g, b, a);
	}
	void UpdateProjection() {
		if (GetBoundShader() != nullptr)
			GetBoundShader()->SetMatrix("uProj", projMatrix * viewMatrix);
	}

	void SetTransform(float x, float y) {
		// Bind Shader
		Shader* shad = GetShader(ShaderType::Interface);
		if (shad!=nullptr)
			shad->SetVec2("uTransform", { x, y });
	}
	void SetSize(float w, float h) {
		// Bind Shader
		Shader* shad = GetShader(ShaderType::Interface);
		if (shad != nullptr)
			shad->SetVec2("uSize", { w, h });
	}
	void SetRenderArea(float f0, float f1, float f2, float f3) {
		// Bind Shader
		Shader* shad = GetShader(ShaderType::Interface);
		if (shad != nullptr)
			shad->SetVec4("uRenderArea", f0, f1, f2, f3);
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
	std::unordered_map<std::string, Font*> fonts;
	void AddFont(const std::string& name) {
		if (fonts.count(name) == 0) {
			fonts[name] = new Font(name);
		} else {
			bug::out < bug::RED < "The font '" < name < "' already exists\n";
		}
	}
	Font* GetFont(const std::string& name) {
		if (fonts.count(name)>0) {
			return fonts[name];
		}
		return nullptr;
	}
	
	void DrawString(const std::string& _font, const std::string& text, bool center,float charHeight,int atChar) {
		// Setup
		
		//bug::out < atChar < bug::end;
		if (text.length() == 0 && atChar==-1)
			return;

		Font* font = GetFont(_font);
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
			float markerX = atX+wid * ((charHeight / (16 / 9.f)) / font->charSize);
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
				textContainer.ModifyVertices(0, 4 * 4 * TEXT_BATCH, verts);
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
		textContainer.ModifyVertices(0, 4 * 4 * TEXT_BATCH, verts);
		textContainer.Draw();
	}
	void DrawString(const std::string& _font, const std::string& text, bool center, float charWidth, float charHeight, int atChar) {
		// Setup

		//bug::out < atChar < bug::end;
		if (text.length() == 0 && atChar == -1)
			return;
		Font* font = GetFont(_font);
		if (font != nullptr) {
			if (font->texture != nullptr)
				font->texture->Bind();
			else
				return;
		}
		else
			return;

		//float charWidth = charHeight* (font->charWid[65] / (float)font->charSize)/(renderer::Wid()/renderer::Hei());

		float ratio = charHeight * ((float)Height() / Width());
		float maxHeight = charHeight;
		float maxWidth = 0;
		float temp = 0;
		for (int i = 0; i < text.size(); i++) {
			unsigned char cha = text[i];
			if (cha == '\n') {
				if (temp > maxWidth)
					maxWidth = temp;
				temp = 0;
				maxHeight += charHeight;
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
			float markerX = atX + wid * ((charHeight / (16 / 9.f)) / font->charSize);
			float markerY = atY + hei * charHeight;
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
			}
			else {

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
			if (i == TEXT_BATCH) {
				textContainer.ModifyVertices(0, 4 * 4 * TEXT_BATCH, verts);
				textContainer.Draw();
				i = 0;
			}
		}
		// Fill the rest with space
		for (int i = ((atChar != -1) + text.length()) % TEXT_BATCH; i < TEXT_BATCH; i++) {
			for (int j = 0; j < 16; j++) {// stop if 10 characters are zeros (aka. \n) - might be more work than filling the rest with zeros
				verts[i * 16 + j] = 0;
			}
		}

		///bug::out < "end" < bug::end;
		textContainer.ModifyVertices(0, 4 * 4 * TEXT_BATCH, verts);
		textContainer.Draw();
	}
	void DrawRect() {
		rectContainer.Draw();
	}
	void DrawRect(float x, float y) {
		SetTransform(x, y);
		rectContainer.Draw();
	}
	void DrawRect(float x, float y, float w, float h) {
		SetTransform(x, y);
		SetSize(w, h);
		rectContainer.Draw();
	}
	void DrawRect(float x, float y, float w, float h, float r, float g, float b, float a) {
		SetTransform(x, y);
		SetSize(w, h);
		SetColor(r, g, b, a);
		rectContainer.Draw();
	}
	void DrawUVRect(float x, float y, float xw, float yh, float u, float v, float uw, float vh) {
		SetTransform(0, 0);
		SetSize(1, 1);
		SetColor(1, 1, 1, 1);
		float vertices[16]{
			x,y,u,v,
			x,y+yh,u,v+vh,
			x + xw,y + yh,u + uw,v + vh,
			x+xw,y,u+uw,v
		};
		uvRectContainer.ModifyVertices(0, 16, vertices);
		uvRectContainer.Draw();
	}

	std::vector<Light*> lights;
	void AddLight(Light* l) {
		lights.push_back(l);
	}
	void RemoveLight(Light* l) {
		for (int i = 0; i < lights.size(); i++) {
			if (lights[i] == l) {
				lights.erase(lights.begin() + i);
				break;
			}
		}
	}
	/*
	Binds light to current shader
	 If one of the four closest light are already bound then don't rebind them [Not added]
	*/
	void BindLights(glm::vec3 objectPos) {
		if (GetBoundShader() != nullptr) {
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
			GetBoundShader()->SetIVec3("uLightCount", lightCount);
		}
	}
	std::vector<Light*>& GetLights() {
		return lights;
	}
	void PassLight(DirLight* light) {
		if (light != nullptr) {
			GetBoundShader()->SetVec3("uDirLight.ambient", light->ambient);
			GetBoundShader()->SetVec3("uDirLight.diffuse", light->diffuse);
			GetBoundShader()->SetVec3("uDirLight.specular", light->specular);
			GetBoundShader()->SetVec3("uDirLight.direction", light->direction);
		}
	}
	void PassLight(int index, PointLight* light) {
		std::string u = "uPointLights[" + index + (std::string)"].";
		if (light != nullptr) {
			GetBoundShader()->SetVec3(u + "ambient", light->ambient);
			GetBoundShader()->SetVec3(u + "diffuse", light->diffuse);
			GetBoundShader()->SetVec3(u + "specular", light->specular);
			GetBoundShader()->SetVec3(u + "position", light->position);
			GetBoundShader()->SetFloat(u + "constant", light->constant);
			GetBoundShader()->SetFloat(u + "linear", light->linear);
			GetBoundShader()->SetFloat(u + "quadratic", light->quadratic);
		}
	}
	void PassLight(int index, SpotLight* light) {
		std::string u = "uSpotLights[" + index + (std::string)"].";
		if (light!=nullptr) {
			GetBoundShader()->SetVec3(u + "ambient", light->ambient);
			GetBoundShader()->SetVec3(u + "diffuse", light->diffuse);
			GetBoundShader()->SetVec3(u + "specular", light->specular);
			GetBoundShader()->SetVec3(u + "position", light->position);
			GetBoundShader()->SetVec3(u + "direction", light->direction);
			GetBoundShader()->SetFloat(u + "cutOff", light->cutOff);
			GetBoundShader()->SetFloat(u + "outerCutOff", light->outerCutOff);
		}
	}
	void PassMaterial(int index, Material* material) {
		if (GetBoundShader() != nullptr) {
			if (!material->diffuse_map.empty()) {
				BindTexture(index + 1, material->diffuse_map);// + 1 because of shadow_map on 0
				GetBoundShader()->SetInt("uMaterials[" + std::to_string(index) + "].diffuse_map", index + 1);
			}
			GetBoundShader()->SetVec3("uMaterials[" + std::to_string(index) + "].diffuse_color", material->diffuse_color);
			GetBoundShader()->SetVec3("uMaterials[" + std::to_string(index) + "].specular", material->specular);
			GetBoundShader()->SetFloat("uMaterials[" + std::to_string(index) + "].shininess", material->shininess);
		}
	}
}