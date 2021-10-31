#include "gonpch.h"

#include "../Handlers/ObjectHandler.h"

//#if ENGONE_GLFW
#include "Renderer.h"

namespace engone {
	static WindowType windowType = NONE;
	static int winX = 0, winY = 0, winW = 0, winH = 0;
	static GLFWwindow* window = nullptr;
	static bool windowHasFocus = true;
	/*
	static glm::mat4 projMatrix;
	static glm::mat4 viewMatrix;
	static float fov,zNear,zFar;
	*/
	static const int TEXT_BATCH = 40;
	static float verts[4 * 4 * TEXT_BATCH];
	static TriangleBuffer textBuffer, rectBuffer;

	static bool isCursorVisible = true, isCursorLocked=false;

	static std::vector<Light*> lights;

	WindowType  GetWindowType() {
		return windowType;
	}
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
	void SetWindowTitle(const char* title) {
		glfwSetWindowTitle(window, title);
	}
	/*
	Width of game screen not window!
	*/
	int Width() {
		int temp;
		glfwGetWindowSize(window, &temp, nullptr);
		return temp;
	}
	/*
	Height of game screen not window!
	*/
	int Height() {
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
	}/*
	float GetFloatMouseX() {
		return ToFloatScreenX(GetMouseX());
	}
	float GetFloatMouseY() {
		return ToFloatScreenY(GetMouseY());
	}*/
	bool HasFocus() {
		return windowHasFocus;
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
	}
	void SetWindowType(WindowType t) {
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
	
	/*
	void SetProjection(float ratio) {
		projMatrix = glm::perspective(fov, ratio, zNear, zFar);
	}
	void UpdateViewMatrix(double lag) {
		viewMatrix = glm::inverse(
			glm::translate(glm::mat4(1.0f), GetCamera()->position+ GetCamera()->velocity*(float)lag) *
			glm::rotate(GetCamera()->rotation.y, glm::vec3(0, 1, 0)) *
			glm::rotate(GetCamera()->rotation.x, glm::vec3(1, 0, 0))
		);
	}*/
	
#if gone
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
		if (FindFile(path + ".shader")) {
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
#endif
	void InitRenderer() {
		
		if (!glfwInit()) {
			std::cout << "Glfw Init error!" << std::endl;
			return;
		}

		windowType = Windowed;
		MakeWindow("Project Unknown");
		
		if (glewInit() != GLEW_OK) {
			std::cout << "Glew Init Error!" << std::endl;
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

		std::uint32_t indes[TEXT_BATCH*6];
		for (int i = 0; i < TEXT_BATCH; i++) {
			indes[0 + 6 * i] = 0 + 4 * i;
			indes[1 + 6 * i] = 1 + 4 * i;
			indes[2 + 6 * i] = 2 + 4 * i;
			indes[3 + 6 * i] = 2 + 4 * i;
			indes[4 + 6 * i] = 3 + 4 * i;
			indes[5 + 6 * i] = 0 + 4 * i;
		}
		textBuffer.Init(true,nullptr,4*4*TEXT_BATCH,indes,6*TEXT_BATCH);
		textBuffer.SetAttrib(0, 4, 4, 0);
		float temp[]{ // This will be updated in when using DrawRect or DrawUVRect
			0, 0, 0, 0,
			0, 1, 0, 1,
			1, 1, 1, 1,
			1, 0, 1, 0
		};
		unsigned int temp2[]{
			0,1,2,
			2,3,0
		};
		rectBuffer.Init(true, temp,16, temp2, 6);
		rectBuffer.SetAttrib(0, 4, 4, 0);
	}

	bool IsCursorVisible() {
		return isCursorVisible;
	}
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
	void SwitchBlendDepth(bool b) {
		if (b) {
			glEnable(GL_BLEND);
			glDisable(GL_DEPTH_TEST);
		} else {
			glDisable(GL_BLEND);
			glEnable(GL_DEPTH_TEST);
		}
	}
#if gone
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
#endif
	static void Insert4(float* ar, int ind, float f0, float f1, float f2, float f3)
	{
		ar[ind] = f0;
		ar[ind + 1] = f1;
		ar[ind + 2] = f2;
		ar[ind + 3] = f3;
	}
	/*
	
	*/
	void DrawString(Font* font, const std::string& text, bool center, float wantedHeight, float maxWidth, float maxHeight, int atChar)
	{
		if (text.length() == 0 && atChar == -1)
			return;
		if (font == nullptr) {
			std::cout << "DrawString: Font is null\n";
			return;
		}
		else if (font->texture.error) {
			return;
		}
		else {
			font->texture.Bind();
		}

		std::vector<std::string> lines;
		std::vector<float> lineWidths;

		float reachedWidth = 0;
		float spacing = 0;
		int lineIndex = 0;
		std::string word;
		float wordWidth = 0;
		lines.push_back("");
		lineWidths.push_back(0);
		for (int i = 0; i < text.length(); i++) {
			unsigned char chr = text[i];
			//std::cout << (int)chr << "\n";

			float added = wantedHeight * (font->charWid[chr] / (float)font->charSize);
			//if (wordWidth != 0 && lineWidths[lineIndex] != 0)
			added += spacing;

			/*
			if (wordWidth==0&&lineWidths[lineIndex] == 0 && chr == ' ') {// skip space in the beginning of line
				continue;
			}
			*/

			if (chr == '\n') {// || chr == ' ' && lineWidths[lineIndex] + wordWidth + added > reachedWidth) { // new line
				//if (lineWidths[lineIndex] + wordWidth < maxWidth) {
				lines[lineIndex] += word;
				lineWidths[lineIndex] += wordWidth;
				if (chr == ' ') {
					lines[lineIndex] += ' ';
					lineWidths[lineIndex] += added;
				}
				//if (chr == '\n')
				//	lines[lineIndex] += '\n';
				//if (i!=text.length()-1) {
				lineWidths.push_back(0);
				//	if(chr=='\n')
				lines.push_back("\n");
				//	else
				//		lines.push_back("");
				//}
				//}
				//else {
				//	lineWidths.push_back(wordWidth);
				//	lines.push_back(word);
				//}
				word = "";
				wordWidth = 0;
				lineIndex++;
				continue;
			}
			/*
			if (lineWidths[lineIndex]+ wordWidth + added >maxWidth) {
				i--;

				if (word[0] == ' ') {
					word = word.substr(1);
					wordWidth -= spacing+wantedHeight * (font->charWid[chr] / (float)font->charSize);
				}

				lineWidths.push_back(wordWidth);
				lines.push_back(word);
				word = "";
				wordWidth = 0;
				lineIndex++;
				continue;
			}
			*/
			/*
			if (chr == ' ') {
				lines[lineIndex] += word;
				lineWidths[lineIndex] += wordWidth;
				word = "";
				wordWidth = 0;
			}
			*/
			word += chr;
			wordWidth += added;

			// add the last stuff if the last character is reached
			if (i == text.size() - 1) {
				lines[lineIndex] += word;
				lineWidths[lineIndex] += wordWidth;
			}
			// get the maximum width
			if (wordWidth > reachedWidth) {
				reachedWidth = wordWidth;
			}
		}
		//std::cout << "Lines " << lineWidths.size() << " " << lines.size() << "\n";

		if (reachedWidth > maxWidth) {
			wantedHeight *= maxWidth / reachedWidth;
			for (int i = 0; i < lineWidths.size(); i++) {
				//lineWidths[i] -= spacing * (lines[i].length() - 1);
				lineWidths[i] *= maxWidth / reachedWidth;
				//lineWidths[i] += spacing*(lines[i].length()-1);
				//std::cout << rowWidths[i] << "\n";
			}
		}
		if (lineWidths.size() * wantedHeight > maxHeight) {
			for (int i = 0; i < lineWidths.size(); i++) {
				//lineWidths[i] -= spacing * (lines[i].length() - 1);
				lineWidths[i] *= maxHeight / (lineWidths.size() * wantedHeight);
				//lineWidths[i] += spacing*(lines[i].length()-1);
				//std::cout << rowWidths[i] << "\n";
			}
			wantedHeight = maxHeight / lineWidths.size();
		}
		//std::cout << wantedHeight << "\n";
		float x = 0;
		float y = 0;
		if (center) {
			y = (maxHeight - wantedHeight * lines.size()) / 2;
		}
		int dataIndex = 0;
		//std::cout << lines[0] << "\n";
		if (atChar != -1) { // do marker
			//std::cout << lines.size()<<"\n";
			for (int i = 0; i < lines.size(); i++) {
				if (center)
					x = (maxWidth - lineWidths[i]) / 2;
				else
					x = 0;
				for (int j = 0; j < lines[i].length(); j++) {
					char chr = lines[i][j];

					float wStride = wantedHeight * (font->charWid[chr] / (float)font->charSize);

					if (atChar == dataIndex) {
						break;
					}
					x += wStride + spacing;

					dataIndex++;
				}
				if (atChar == dataIndex)
					break;
				y += wantedHeight;
			}
			if (atChar != dataIndex) {
				if (center)
					x = maxWidth / 2;
				else
					x = 0;
			}
			dataIndex = 0;
			//std::cout << x << "\n";
			float wuv = font->charWid[0] / (float)font->imgSize;
			float u = (0 % 16);
			float v = 15 - (0 / 16);
			float markerW = wantedHeight * (font->charWid[0] / (float)font->charSize);
			Insert4(verts, 16 * dataIndex, x, y, (u) / 16, (v + 1) / 16);
			Insert4(verts, 4 + 16 * dataIndex, x, y + wantedHeight, (u) / 16, (v) / 16);
			Insert4(verts, 8 + 16 * dataIndex, x + markerW, y + wantedHeight, (u) / 16 + wuv, (v) / 16);
			Insert4(verts, 12 + 16 * dataIndex, x + markerW, y, (u) / 16 + wuv, (v + 1) / 16);
			dataIndex = 1;
			if (center) {
				y = (maxHeight - wantedHeight * lines.size()) / 2;
			}
		}
		for (int i = 0; i < lines.size(); i++) {
			//std::cout << "[" << lines[i] << "] " << "\n";
			float x = 0;
			if (center)
				x = (maxWidth - lineWidths[i]) / 2;
			//std::cout << lines[i].length()<<"\n";
			for (int j = 0; j < lines[i].length(); j++) {
				char chr = lines[i][j];
				//if (chr == '\n')
				//	continue;
				float wStride = wantedHeight * (font->charWid[chr] / (float)font->charSize);

				float wuv = font->charWid[chr] / (float)font->imgSize;
				float u = (chr % 16);
				float v = 15 - (chr / 16);

				Insert4(verts, 16 * dataIndex, x, y, (u) / 16, (v+1) / 16);
				Insert4(verts, 4 + 16 * dataIndex, x, y+ wantedHeight, (u) / 16, (v) / 16);
				Insert4(verts, 8 + 16 * dataIndex, x + wStride, y+ wantedHeight, (u) / 16 + wuv, (v) / 16);
				Insert4(verts, 12 + 16 * dataIndex, x + wStride, y, (u) / 16 + wuv, (v+1) / 16);
				x += wStride + spacing;

				if (dataIndex == TEXT_BATCH) {
					textBuffer.ModifyVertices(0, 4 * 4 * TEXT_BATCH, verts);
					textBuffer.Draw();
					dataIndex = 0;
				}
				else {
					dataIndex++;
				}
			}
			y += wantedHeight;
		}

		int charIndex = dataIndex % TEXT_BATCH;
		memset(&verts[16 * charIndex], 0, 16 * (TEXT_BATCH - charIndex) * sizeof(float));

		//memset(&verts[0], 0, 16 * (TEXT_BATCH) * sizeof(float));
		/*
		Insert4(verts,0, 0, 0, 0, 0);
		Insert4(verts,4, 1, 0, 0, 1);
		Insert4(verts,8, 1, 1, 1, 1);
		Insert4(verts,12, 0, 1, 1, 0);

		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				std::cout << verts[i * 4 + j]<<" ";
			}
			std::cout << "\n";
		}*/

		textBuffer.ModifyVertices(0, 16 * TEXT_BATCH, verts);
		textBuffer.Draw();
		//rectBuffer.Draw();
	}
#if gone
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

		//float charWidth = charHeight* (font->charWid[65] / (float)font->charSize)/(Wid()/Hei());

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

		//float charWidth = charHeight* (font->charWid[65] / (float)font->charSize)/(Wid()/Hei());

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
#endif
	void DrawRect() {
		float vertices[16]{
		0,0,0,1,
		0,1,0,0,
		1,1,1,0,
		1,0,1,1
		};
		rectBuffer.ModifyVertices(0, 16, vertices);
		rectBuffer.Draw();
	}
	void DrawRect(Shader* shad, float x,float y,float w,float h,float r,float g,float b,float a)
	{
		float vertices[16]{
		0,0,0,1,
		0,1,0,0,
		1,1,1,0,
		1,0,1,1
		};
		rectBuffer.ModifyVertices(0, 16, vertices);
		shad->SetVec2("uPos", { x,y });
		shad->SetVec2("uSize", { w,h });
		shad->SetVec4("uColor", r,g,b,a);
		rectBuffer.Draw();
	}
	void DrawUVRect(float u, float v, float uw, float vh) {
		float vertices[16]{
			0,0,u,v,
			0,1,u,v+vh,
			1,1,u + uw,v + vh,
			1,0,u+uw,v
		};
		rectBuffer.ModifyVertices(0, 16, vertices);
		rectBuffer.Draw();
	}
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
	void BindLights(Shader* shader, glm::vec3 objectPos) {
		if (shader != nullptr) {
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
				PassLight(shader,dir);
				lightCount.x++;
			}

			for (int i = 0; i < N_POINTLIGHTS; i++) {
				if (points[i] != nullptr) {
					PassLight(shader,i, points[i]);
					lightCount.y++;
				} else
					break;
			}
			for (int i = 0; i < N_SPOTLIGHTS; i++) {
				if (spots[i] != nullptr) {
					PassLight(shader,i, spots[i]);
					lightCount.z++;
				} else
					break;
			}
			shader->SetIVec3("uLightCount", lightCount);
		}
	}
	std::vector<Light*>& GetLights() {
		return lights;
	}
	void PassLight(Shader* shader, DirLight* light) {
		if (light != nullptr) {
			shader->SetVec3("uDirLight.ambient", light->ambient);
			shader->SetVec3("uDirLight.diffuse", light->diffuse);
			shader->SetVec3("uDirLight.specular", light->specular);
			shader->SetVec3("uDirLight.direction", light->direction);
		}
	}
	void PassLight(Shader* shader, int index, PointLight* light) {
		std::string u = "uPointLights[" + index + (std::string)"].";
		if (light != nullptr) {
			shader->SetVec3(u + "ambient", light->ambient);
			shader->SetVec3(u + "diffuse", light->diffuse);
			shader->SetVec3(u + "specular", light->specular);
			shader->SetVec3(u + "position", light->position);
			shader->SetFloat(u + "constant", light->constant);
			shader->SetFloat(u + "linear", light->linear);
			shader->SetFloat(u + "quadratic", light->quadratic);
		}
	}
	void PassLight(Shader* shader, int index, SpotLight* light) {
		std::string u = "uSpotLights[" + index + (std::string)"].";
		if (light!=nullptr) {
			shader->SetVec3(u + "ambient", light->ambient);
			shader->SetVec3(u + "diffuse", light->diffuse);
			shader->SetVec3(u + "specular", light->specular);
			shader->SetVec3(u + "position", light->position);
			shader->SetVec3(u + "direction", light->direction);
			shader->SetFloat(u + "cutOff", light->cutOff);
			shader->SetFloat(u + "outerCutOff", light->outerCutOff);
		}
	}
	void PassMaterial(Shader* shader, int index, MaterialAsset* material) {
		if (shader != nullptr&&material!=nullptr) {
			if (material->diffuse_map!=nullptr) {

				material->diffuse_map->Bind(index+1);
				//BindTexture(index + 1, material->diffuse_map);// + 1 because of shadow_map on 0
				//std::cout << "PassMaterial - texture not bound!\n";
				shader->SetInt("uMaterials[" + std::to_string(index) + "].diffuse_map", index + 1);
				shader->SetInt("uMaterials[" + std::to_string(index) + "].useMap", 1);
			}else{
				shader->SetInt("uMaterials[" + std::to_string(index) + "].useMap", 0);
			}
			shader->SetVec3("uMaterials[" + std::to_string(index) + "].diffuse_color", material->diffuse_color);
			shader->SetVec3("uMaterials[" + std::to_string(index) + "].specular", material->specular);
			shader->SetFloat("uMaterials[" + std::to_string(index) + "].shininess", material->shininess);
		}else{
			//std::cout << "shader or material is nullptr in Passmaterial\n";
		}
	}
}
//#endif