
#include "Engone/RenderModule.h"

#include "Engone/Window.h"
#include "Engone/Utilities/Utilities.h"
#include "Engone/EventModule.h"

#include "Engone/Engone.h"

static const char* uiPipelineGLSL = {
#include "Engone/Shaders/uiPipeline.glsl"
};
static const char* rendererGLSL = {
#include "Engone/Shaders/renderer.glsl"
};
static const char* linesGLSL = {
#include "Engone/Shaders/lines3d.glsl"
};
static const char* guiShaderSource = {
#include "Engone/Shaders/gui.glsl"
};
static const char* objectSource = {
#include "Engone/Shaders/object.glsl"
};
static const char* armatureSource = {
#include "Engone/Shaders/armature.glsl"
};
static const char* particleGLSL= {
#include "Engone/Shaders/particle.glsl"
};

namespace engone {

#ifndef PIPE3_LINE_BATCH_LIMIT
#define PIPE3_LINE_BATCH_LIMIT 500
#endif
#ifndef PIPE3_LINE_RESERVE
#define PIPE3_LINE_RESERVE PIPE3_LINE_BATCH_LIMIT
#endif
	static Renderer* g_activeRenderer = nullptr;
	void Renderer::init() {
		if (m_initialized) return;
		m_initialized = true;
		m_parent = GetActiveWindow();

		//m_parent = GetActiveWindow();
		uint32_t indes[TEXT_BATCH * 6];
		for (int i = 0; i < TEXT_BATCH; i++) {
			indes[0 + 6 * i] = 0 + 4 * i;
			indes[1 + 6 * i] = 1 + 4 * i;
			indes[2 + 6 * i] = 2 + 4 * i;
			indes[3 + 6 * i] = 2 + 4 * i;
			indes[4 + 6 * i] = 3 + 4 * i;
			indes[5 + 6 * i] = 0 + 4 * i;
		}
		textIBO.setData(6 * TEXT_BATCH*sizeof(float), indes);
		textVBO.setData(16 * TEXT_BATCH * sizeof(float), nullptr);
		textVAO.addAttribute(4, &textVBO);

		Assets* assets = m_parent->getAssets();

		// pipeline

		assets->set<Shader>("uiPipeline", new Shader(uiPipelineGLSL));

		boxVBO.setData(4 * VERTEX_SIZE * MAX_BOX_BATCH * sizeof(float), nullptr);

		uint32_t intArray[6 * MAX_BOX_BATCH];
		for (int i = 0; i < MAX_BOX_BATCH; i++) {
			intArray[i * 6] = i * 4 + 0;
			intArray[i * 6 + 1] = i * 4 + 1;
			intArray[i * 6 + 2] = i * 4 + 2;
			intArray[i * 6 + 3] = i * 4 + 2;
			intArray[i * 6 + 4] = i * 4 + 3;
			intArray[i * 6 + 5] = i * 4 + 0;
		}
		boxIBO.setData(6 * MAX_BOX_BATCH * sizeof(float), intArray);

		boxVAO.addAttribute(2); // pos
		boxVAO.addAttribute(2); // uv
		boxVAO.addAttribute(4); // color
		boxVAO.addAttribute(1, &boxVBO); // texture

		float near_plane = 1.f, far_plane = 20.5f;
		lightProjection = glm::ortho(-10.f, 10.f, -10.f, 10.f, near_plane, far_plane);

		setProjection(GetWidth()/GetHeight());

		// ISSUE: how to deal with tracking of assets, when window is destroyed, free assets.
		//	What if you have added an asset twice with different names
		//  Freeing assets not allocate with tracker would give negative memoray allocations.
		//  Either the user needs to define some flag?
		//  Maybe track with addMemory event if assets are added twice it would be fine?
		//  maybe use tracker inside constructors, You can't have inside constructors because a stack allocated class would be tracked.

		// Line pipeline
		assets->set<Shader>("lines3d", new Shader(linesGLSL));
		pipe3lines.reserve(PIPE3_LINE_RESERVE*6);
		pipe3lineVB.setData(PIPE3_LINE_BATCH_LIMIT*6 * sizeof(float), nullptr);
		uint32_t indLine[PIPE3_LINE_BATCH_LIMIT*2];
		for (int i = 0; i < PIPE3_LINE_BATCH_LIMIT * 2; i++) {
			indLine[i] = i;
		}
		pipe3lineIB.setData(PIPE3_LINE_BATCH_LIMIT*2 * sizeof(float), indLine);
		pipe3lineVA.addAttribute(3, &pipe3lineVB);

		assets->set<Shader>("renderer", new Shader(rendererGLSL));
		float cubeVertices[48]{
			0,0,0, 0,0,0,
			1,0,0, 0,0,-1,
			0,1,0, -1,0,0,
			1,1,0, 0,1,0,

			0,0,1, 0,-1,0,
			1,0,1, 1,0,0,
			0,1,1, 0,0,1,
			1,1,1, 0,0,0,
		};
		for (int i = 0; i < 24; i++)
			cubeVertices[i % 3 + (i / 3) * 6] -= 0.5;
		cubeVBO.setData(48*sizeof(float), cubeVertices);
		uint32_t cubeIndices[36]{
			0, 2, 1, 2, 3, 1,
			0, 1, 4, 1, 5, 4,
			0, 4, 2, 4, 6, 2,
			2, 6, 3, 6, 7, 3,
			1, 3, 5, 3, 7, 5,
			4, 5, 6, 5, 7, 6,
		};
		cubeIBO.setData(36 * sizeof(uint32_t), cubeIndices);

		cubeInstanceVBO.setData(MAX_BOX_BATCH * sizeof(Cube), nullptr);
		cubeVAO.addAttribute(3);
		cubeVAO.addAttribute(3, &cubeVBO);
		cubeVAO.addAttribute(4, 1);
		cubeVAO.addAttribute(4, 1);
		cubeVAO.addAttribute(4, 1);
		cubeVAO.addAttribute(4, 1);
		cubeVAO.addAttribute(3, 1, &cubeInstanceVBO);

		float simpleQuad[]{
			0,1,0,1,
			0,0,0,0,
			1,1,1,1,
			1,0,1,0,
			1,1,1,1,
			0,0,0,0,

			//0,0,0,1,
			//0,1,0,0,
			//1,1,1,0,
			//1,0,1,1,
		};
		//uint32_t simpleInd[]{
		//	0, 1, 2, 2, 3, 0,
		//};
		quadVB.setData(sizeof(simpleQuad), simpleQuad);
		//quadIB.setData(sizeof(uint32_t)*6, simpleInd);
		quadVA.addAttribute(4,&quadVB);

		instanceBuffer.setData(INSTANCE_BATCH * sizeof(glm::mat4), nullptr);

		assets->set<Shader>("gui", new Shader(guiShaderSource));
		
		assets->set<Shader>("object", new Shader(objectSource));
		assets->set<Shader>("armature", new Shader(armatureSource));
		MaterialAsset* mat = new MaterialAsset();
		mat->setBaseName("defaultMaterial");
		assets->set<MaterialAsset>("defaultMaterial", mat);

		//instanceBuffer.setData(INSTANCE_LIMIT * 16, nullptr);
		//depthBuffer.init();

		//AddListener(new Listener(EventType::Resize, 9999, DrawOnResize));
		setActiveRenderer();
	}
	void Renderer::DrawQuad(RenderInfo& info) {
		//Assets* assets = info.window->getAssets();
		//Shader* guiShader = assets->get<Shader>("gui");

		//guiShader->bind();
		//guiShader->setVec2("uWindow", { GetWidth(), GetHeight() });
		//guiShader->setVec2("uPos", { x,y });
		//guiShader->setVec2("uSize", {w,h});
		//guiShader->setVec4("uColor", 1,1,1,1);
		//guiShader->setInt("uTextures",0);
		//guiShader->setInt("uColorMode",texture?1:0);

		//quadVA.draw(&quadIB);
		quadVA.drawTriangleArray(6);
	}
	void TerminateRenderer() {
		// destroy buffers
		//colliderBuffer.Uninit();
		//lineBuffer.Uninit();
	}
	void EnableBlend() {
		glEnable(GL_BLEND);
		glDisable(GL_DEPTH_TEST);
	}
	void EnableDepth() {
		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
	}
	Camera* Renderer::getCamera() {
		return &camera;
	}
	glm::mat4& Renderer::getLightProj() {
		return lightProjection;
	}
	void Renderer::setProjection(float ratio) {
		if (ratio == -NAN) log::out << "Renderer::SetProjection ratio was bad\n";
		projMatrix = glm::perspective(fov, ratio, zNear, zFar);
	}
	// Should be done once in the render loop, currently done in Engone::render
	//void Renderer::updateViewMatrix(double lag) {
	//	viewMatrix = glm::inverse(
	//		glm::translate(glm::mat4(1.0f), getCamera()->position) *
	//		glm::rotate(getCamera()->rotation.y, glm::vec3(0, 1, 0)) *
	//		glm::rotate(getCamera()->rotation.x, glm::vec3(1, 0, 0))
	//	);
	//}
	void Renderer::updateProjection(Shader* shader) {
		if (shader != nullptr)
			shader->setMat4("uProj", projMatrix * camera.getViewMatrix());
	}
	static void Insert4(float* ar, int ind, float f0, float f1, float f2, float f3) {
		ar[ind] = f0;
		ar[ind + 1] = f1;
		ar[ind + 2] = f2;
		ar[ind + 3] = f3;
	}
	void Renderer::DrawString(Font* font, const std::string& text, bool center, float wantedHeight, float maxWidth, float maxHeight, int atChar) {
		if (text.length() == 0 && atChar == -1)
			return;
		if (font == nullptr) {
			std::cout << "DrawString: Font is null\n";
			return;
		} else if (font->texture.error) {
			return;
		} else {
			Shader* guiShader = m_parent->getAssets()->get<Shader>("gui");
			if (guiShader != nullptr)
				guiShader->setInt("uColorMode", 1);
			//guiShader->setInt("uTextures", 0);
			font->texture.bind();
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
		for (uint32_t i = 0; i < text.length(); ++i) {
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
			for (uint32_t i = 0; i < lineWidths.size(); ++i) {
				//lineWidths[i] -= spacing * (lines[i].length() - 1);
				lineWidths[i] *= maxWidth / reachedWidth;
				//lineWidths[i] += spacing*(lines[i].length()-1);
				//std::cout << rowWidths[i] << "\n";
			}
		}
		if (lineWidths.size() * wantedHeight > maxHeight) {
			for (uint32_t i = 0; i < lineWidths.size(); ++i) {
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
		uint32_t dataIndex = 0;
		//std::cout << lines[0] << "\n";
		if (atChar != -1) { // do marker
			//std::cout << lines.size()<<"\n";
			for (uint32_t i = 0; i < lines.size(); ++i) {
				if (center)
					x = (maxWidth - lineWidths[i]) / 2;
				else
					x = 0;
				for (uint32_t j = 0; j < lines[i].length(); ++j) {
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
		for (uint32_t i = 0; i < lines.size(); ++i) {
			//std::cout << "[" << lines[i] << "] " << "\n";
			float x = 0;
			if (center)
				x = (maxWidth - lineWidths[i]) / 2;
			//std::cout << lines[i].length()<<"\n";
			for (uint32_t j = 0; j < lines[i].length(); ++j) {
				char chr = lines[i][j];
				//if (chr == '\n')
				//	continue;
				float wStride = wantedHeight * (font->charWid[chr] / (float)font->charSize);

				float wuv = font->charWid[chr] / (float)font->imgSize;
				float u = (float)(chr % 16);
				float v = (float)(15 - (chr / 16));

				Insert4(verts, 16 * dataIndex, x, y, (u) / 16, (v + 1) / 16);
				Insert4(verts, 4 + 16 * dataIndex, x, y + wantedHeight, (u) / 16, (v) / 16);
				Insert4(verts, 8 + 16 * dataIndex, x + wStride, y + wantedHeight, (u) / 16 + wuv, (v) / 16);
				Insert4(verts, 12 + 16 * dataIndex, x + wStride, y, (u) / 16 + wuv, (v + 1) / 16);
				x += wStride + spacing;

				if (dataIndex + 1 == TEXT_BATCH) {
					textVBO.setData(16 * TEXT_BATCH, verts);
					textVAO.draw(&textIBO);
					//textBuffer.ModifyVertices(0, 4 * 4 * TEXT_BATCH, verts);
					//textBuffer.Draw();
					dataIndex = 0;
				} else {
					dataIndex++;
				}
			}
			y += wantedHeight;
		}

		uint32_t charIndex = dataIndex % TEXT_BATCH;
		memset(verts+16 * charIndex, 0, 16 * (TEXT_BATCH - charIndex) * sizeof(float));

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

		textVBO.setData(16 * TEXT_BATCH * sizeof(float), verts);
		textVAO.draw(&textIBO);

		//textBuffer.ModifyVertices(0, 16 * TEXT_BATCH, verts);
		//textBuffer.Draw();
		//rectBuffer.Draw();
	}
	void Renderer::DrawCube(glm::mat4 matrix, glm::vec3 scale, glm::vec3 color) {
		cubeObjects.push_back({ glm::scale(matrix,scale),color });
	}
	void Renderer::DrawNetCube(glm::mat4 matrix, glm::vec3 scale, glm::vec3 color) {
		auto p = [&](float x, float y, float z) {
			return (matrix * glm::translate(glm::vec3(scale.x * (x - .5), scale.y * (y - .5), scale.z * (z - .5))))[3];
		};
		DrawLine(p(0, 0, 0), p(1, 0, 0));
		DrawLine(p(0, 0, 0), p(0, 0, 1));
		DrawLine(p(1, 0, 1), p(1, 0, 0));
		DrawLine(p(1, 0, 1), p(0, 0, 1));

		DrawLine(p(0, 0, 0), p(0, 1, 0));
		DrawLine(p(1, 0, 0), p(1, 1, 0));
		DrawLine(p(1, 0, 1), p(1, 1, 1));
		DrawLine(p(0, 0, 1), p(0, 1, 1));

		DrawLine(p(0, 1, 0), p(1, 1, 0));
		DrawLine(p(0, 1, 0), p(0, 1, 1));
		DrawLine(p(1, 1, 1), p(1, 1, 0));
		DrawLine(p(1, 1, 1), p(0, 1, 1));
	}
	constexpr float pis() {
		return glm::pi<float>();
	}
	void Renderer::DrawSphere(glm::vec3 position, float radius, glm::vec3 color) {
	/*	std::vector<glm::vec3> points;
		std::vector<tri> tris;
		std::vector<line> lines;*/

		constexpr float pi = pis();
		//DrawBegin();
		//AddVertex(0, 1*radius, 0);
		//AddVertex(0,-1* radius, 0);
		//for (int i = 0; i < 5; i++) {
		//	AddVertex( cosf(2 * pi * (i / 5.f))*radius,.5f* radius,sinf(2 * pi * (i / 5.f)) * radius);
		//	AddVertex( cosf(2 * pi * (1 / 10.f + i / 5.f))* radius,-.5f* radius,sinf(2 * pi * (1 / 10.f + i / 5.f)) * radius);

		//	AddIndex( 2 + i * 2,2 + i * 2 + 1 );
		//	AddIndex( 2 + ((i + 1) % 5) * 2,2 + i * 2 + 1 );
		//	AddIndex( 2 + i * 2, 2 + ((i + 1) % 5) * 2 );
		//	AddIndex( 2 + i * 2 + 1, 2 + ((i + 1) % 5) * 2 + 1 );
		//	
		//	AddIndex( 0,2 + i * 2 );
		//	AddIndex( 1,2 + i * 2 + 1 );
		//}
		//DrawBuffer();

	}
	//struct line {
	//	int a, b;
	//};
	//struct tri {
	//	int a, b, c;
	//};
	//void DrawSphere(float radius) {
	//	std::vector<glm::vec3> points;
	//	std::vector<tri> tris;
	//	std::vector<line> lines;

	//	float pi = glm::pi<float>();
	//	points.push_back({ 0,1,0 });
	//	points.push_back({ 0,-1,0 });
	//	for (int i = 0; i < 5; i++) {
	//		points.push_back({ cosf(2 * pi * (i / 5.f)),.5,sinf(2 * pi * (i / 5.f)) });
	//		points.push_back({ cosf(2 * pi * (1 / 10.f + i / 5.f)),-.5,sinf(2 * pi * (1 / 10.f + i / 5.f)) });

	//		lines.push_back({ 2 + i * 2,2 + i * 2 + 1 });
	//		lines.push_back({ 2 + ((i + 1) % 5) * 2,2 + i * 2 + 1 });
	//		lines.push_back({ 2 + i * 2, 2 + ((i + 1) % 5) * 2 });
	//		lines.push_back({ 2 + i * 2 + 1, 2 + ((i + 1) % 5) * 2 + 1 });

	//		lines.push_back({ 0,2 + i * 2 });
	//		lines.push_back({ 1,2 + i * 2 + 1 });
	//	}

	//	for (glm::vec3& p : points)
	//		p *= radius;

	//	{
	//		std::vector<glm::vec3> oldPoints = points;
	//		std::vector<line> oldLines = lines;
	//		std::vector<tri> oldTris = tris;
	//	}

	//	/*colliderBuffer.ModifyVertices(0, points.size() * 3, points.data());
	//	colliderBuffer.ModifyIndices(0, lines.size() * 2, lines.data());
	//	colliderBuffer.Draw();*/
	//}
	void Renderer::DrawLine(glm::vec3 a, glm::vec3 b) {
		pipe3lines.push_back(a.x);
		pipe3lines.push_back(a.y);
		pipe3lines.push_back(a.z);

		pipe3lines.push_back(b.x);
		pipe3lines.push_back(b.y);
		pipe3lines.push_back(b.z);
	}
	void Renderer::DrawTriangle(glm::vec3 a, glm::vec3 b, glm::vec3 c) {
		DrawLine(a,b);
		DrawLine(b,c);
		DrawLine(c,a);
	}

	// global ui pipeline
	namespace ui {
		struct PipeTextBox {
			//TextBox(const std::string& text, float x, float y, float h) : text() {}
			uint32_t text_index=0;
			float x = 0, y = 0, h = 20;
			Font* font = nullptr;
			Color rgba;
			uint32_t at = -1;
		};
		void Draw(Box box) {
			if (!GetActiveRenderer()) return;
			if (box.x + box.w < 0 || box.x>GetWidth()||box.y+box.h<0||box.y>GetHeight())
				return; // out of bounds
			GetActiveRenderer()->uiObjects.writeMemory<Box>('B', &box);
		}
		void Draw(TexturedBox box) {
			if (box.texture == nullptr) {
				return;
			}
			if (!GetActiveRenderer()) return;
			if (box.x + box.w < 0 || box.x>GetWidth() || box.y + box.h<0 || box.y>GetHeight())
				return; // out of bounds
			GetActiveRenderer()->uiObjects.writeMemory<TexturedBox>('T', &box);
		}
		void Draw(TextBox& box) {
			if (!box.editing) {
				box.at = -1;
			}
			if (!GetActiveRenderer()) return;
			// ISSUE: width of str is used.  ( box.x+box.w<0 )
			if (box.x>GetWidth() || box.y + box.h<0 || box.y>GetHeight())
				return; // out of bounds
			PipeTextBox a;
			GetActiveRenderer()->uiStrings.push_back(box.text);
			a.text_index = GetActiveRenderer()->uiStrings.size()-1;
			a.x = box.x;
			a.y = box.y;
			a.h = box.h;
			a.font = box.font;
			a.rgba = box.rgba;
			a.at = box.at;

			GetActiveRenderer()->uiObjects.writeMemory<PipeTextBox>('S', &a);
		}

		void Edit(std::string& str, int& at,bool& editing, bool stopEditWithEnter) {
			if (at < 0) at = str.length();
			if (at > str.length()) at = str.length();
			uint32_t chr=0;
			if (IsKeyDown(GLFW_KEY_LEFT_CONTROL) && IsKeyPressed(GLFW_KEY_V)) {
				std::string tmp = PollClipboard();
				//str.insert(str.begin() + at, tmp);
				str.insert(at, tmp);
				at += (int)tmp.length();
				return;
			}
			while (chr = PollChar()) {
				if (chr == GLFW_KEY_BACKSPACE) {
					if (str.length() > 0 && at > 0) {
						str = str.substr(0, at - 1) + str.substr(at);
						at--;
					}
				} else if (chr == GLFW_KEY_ENTER) {
					if (stopEditWithEnter) {
						editing = false;
					} else {
						str.insert(str.begin() + at, '\n');
						at++;
					}
				} else if (chr == GLFW_KEY_DELETE) {
					if ((int)str.length() > at) {
						str = str.substr(0, at) + str.substr(at + 1);
					}
				} else if (chr == GLFW_KEY_LEFT) {
					if (at > 0)
						at--;
				} else if (chr == GLFW_KEY_RIGHT) {
					if (at < (int)str.length())
						at++;
				} else {
					str.insert(str.begin() + at, chr);
					at++;
				}
			}
		}
		void Edit(std::string& str) {
			int at = (int)str.length();
			bool editing=false;
			Edit(str, at,editing,true);
		}
		// will only edit if box->editing is true.
		// the TextBox need to store it's editing value for next frame/update.
		// either make your own isEditing variable and then box->editing=isEditing.
		// or have TextBox in a class or as a static global variable. It's up to you.
		void Edit(TextBox* box,bool stopEditWithEnter) {
			if (box->editing) {
				Edit(box->text, box->at,box->editing, stopEditWithEnter);
			}
		}
		static bool inside(Box box, float x, float y) {
			return box.x<x&& box.x + box.w>x && box.y<y&& box.y + box.h>y;
		}
		static bool inside(TextBox box, float x, float y) {
			float w = box.h * box.text.length();
			if (box.font != nullptr) {
				w = box.font->getWidth(box.text, box.h);
			}
			return box.x<x&& box.x + w>x && box.y<y&& box.y + box.h>y;
		}
		bool Clicked(Box& box) {
			if (IsKeyPressed(GLFW_MOUSE_BUTTON_1)) {
				float x = GetMouseX();
				float y = GetMouseY();
				if (inside(box, x, y)) {
					return true;
				}
			}
			return false;
		}
		bool Clicked(TextBox& box) {
			if (IsKeyPressed(GLFW_MOUSE_BUTTON_1)) {
				float x = GetMouseX();
				float y = GetMouseY();
				if (inside(box, x, y)) {
					return true;
				}
			}
			return false;
		}
		bool Hover(Box& box) {
			float x = GetMouseX();
			float y = GetMouseY();
			if (inside(box, x, y)) {
				return true;
			}
			return false;
		}
		bool Hover(TextBox& box) {
			float x = GetMouseX();
			float y = GetMouseY();
			if (inside(box, x, y)) {
				return true;
			}
			return false;
		}
	}
	struct PIPE_VERTEX {
		float data[Renderer::VERTEX_SIZE];
	};
	static int min(int a, int b) {
		return a < b ? a : b;
	}
	Renderer* GetActiveRenderer() {
		return g_activeRenderer;
	}
	void Renderer::setActiveRenderer() {
		g_activeRenderer = this;
	}
	void Renderer::render(RenderInfo& info) {
		GLFWwindow* match = glfwGetCurrentContext();
		//log::out << "Win:"<<info.window << "\n";
		if (!m_parent) {
			log::out << log::RED << "Renderer window was nullptr\n";
			return;
		}
		if (match != m_parent->glfw()) {
			log::out << "NOT MATCH\n";
		}
		EnableDepth();

		Shader* shad = m_parent->getAssets()->get<Shader>("renderer");
		if (shad) {
			shad->bind();
			updateProjection(shad);
			shad->setVec3("uCamera", camera.getPosition());
			uint32_t drawnCubes = 0;
			uint32_t cubeCount = 0;

			while (drawnCubes < cubeObjects.size()) {
				cubeCount = min(MAX_BOX_BATCH, cubeObjects.size() - drawnCubes);

				// Cube only consists of floats which is important. may want to zero memory the last bit of the instance buffer.
				cubeInstanceVBO.setData(cubeCount * sizeof(Cube), (cubeObjects.data() + drawnCubes));
				cubeVAO.selectBuffer(2, &cubeInstanceVBO);
				cubeVAO.draw(&cubeIBO, cubeCount);
				drawnCubes += cubeCount;
			}
			cubeObjects.clear();
		}
		shad = m_parent->getAssets()->get<Shader>("lines3d");
		if (shad) {
			shad->bind();
			updateProjection(shad);
			glLineWidth(1.f);
			//shad->setVec3("uColor", { 0.3,0.8,0.3 });
			shad->setVec3("uColor", { 0.9,0.2,0.2 });

			uint32_t drawnLines = 0;

			while (drawnLines * 6 < pipe3lines.size()) {
				if ((drawnLines + PIPE3_LINE_BATCH_LIMIT) * 6 > pipe3lines.size()) {
					pipe3lines.resize((drawnLines + PIPE3_LINE_BATCH_LIMIT) * 6, 0);
				}
				pipe3lineVB.setData(PIPE3_LINE_BATCH_LIMIT * 6 * sizeof(float), pipe3lines.data() + drawnLines * 6);
				pipe3lineVA.drawLines(&pipe3lineIB);
				drawnLines += PIPE3_LINE_BATCH_LIMIT;
			}
			pipe3lines.clear();
		}

		EnableBlend();

		if (!uiObjects.empty()) {
			if (info.window != m_parent)
				log::out << log::YELLOW << "Renderer and active context is different!\n";
		}
		// setup
		Shader* guiShad = m_parent->getAssets()->get<Shader>("gui");
		if (!guiShad) return;

		guiShad->bind();
		guiShad->setVec2("uWindow", { GetWidth(), GetHeight() });

		Shader* pipeShad = m_parent->getAssets()->get<Shader>("uiPipeline");
		if (!pipeShad) return;

		pipeShad->bind();
		pipeShad->setVec2("uWindow", { GetWidth(), GetHeight() });

		// needs to be done once
		for (int i = 0; i < 8; i++) {
			pipeShad->setInt("uSampler[" + std::to_string(i) + std::string("]"), i);
		}
	
		// Method one
		int floatIndex = 0;
		std::unordered_map<Texture*, int> boundTextures;
		char lastShader = 0;
		while (true) {
			char type = uiObjects.readType();
			if (type == 'B') {
				ui::Box* box = uiObjects.readItem<ui::Box>();

				((PIPE_VERTEX*)&floatArray)[floatIndex * 4 + 0] = { box->x, box->y, 0, 0, box->rgba.r, box->rgba.g, box->rgba.b, box->rgba.a, -1 };
				((PIPE_VERTEX*)&floatArray)[floatIndex * 4 + 1] = { box->x, box->y + box->h, 0, 0, box->rgba.r, box->rgba.g, box->rgba.b, box->rgba.a, -1 };
				((PIPE_VERTEX*)&floatArray)[floatIndex * 4 + 2] = { box->x + box->w, box->y + box->h, 0, 0, box->rgba.r, box->rgba.g, box->rgba.b, box->rgba.a, -1 };
				((PIPE_VERTEX*)&floatArray)[floatIndex * 4 + 3] = { box->x + box->w, box->y, 0, 0, box->rgba.r, box->rgba.g, box->rgba.b, box->rgba.a, -1 };

				floatIndex++;

			} else if (type == 'T') {
				ui::TexturedBox* box = uiObjects.readItem<ui::TexturedBox>();

				int slot = boundTextures.size();
				if (boundTextures.count(box->texture)) {
					boundTextures[box->texture] = boundTextures.size();
				} else {
					slot = boundTextures[box->texture];
				}

				((PIPE_VERTEX*)&floatArray)[floatIndex * 4 + 0] = { box->x,			box->y,			box->u,			box->v + box->vh,				box->rgba.r, box->rgba.g, box->rgba.b, box->rgba.a, (float)slot };
				((PIPE_VERTEX*)&floatArray)[floatIndex * 4 + 1] = { box->x,			box->y + box->h,	box->u,			box->v,				box->rgba.r, box->rgba.g, box->rgba.b, box->rgba.a, (float)slot };
				((PIPE_VERTEX*)&floatArray)[floatIndex * 4 + 2] = { box->x + box->w, box->y + box->h,	box->u + box->uw,	box->v,				box->rgba.r, box->rgba.g, box->rgba.b, box->rgba.a, (float)slot };
				((PIPE_VERTEX*)&floatArray)[floatIndex * 4 + 3] = { box->x + box->w, box->y,			box->u + box->uw,	box->v + box->vh,				box->rgba.r, box->rgba.g, box->rgba.b, box->rgba.a, (float)slot };

				floatIndex++;

			} else if (type == 'S') {
				// Is done below
			}

			if (floatIndex == MAX_BOX_BATCH || boundTextures.size() >= 7 || (type == 0 && floatIndex != 0)||(type=='S'&&lastShader!='P'&&lastShader!=0)) {

				for (auto [texture, index] : boundTextures) {
					texture->bind(index);
				}

				if (floatIndex != MAX_BOX_BATCH)
					ZeroMemory(floatArray + floatIndex * 4 * VERTEX_SIZE, (MAX_BOX_BATCH - floatIndex) * 4 * VERTEX_SIZE * sizeof(float));

				if (lastShader != 'P')
					pipeShad->bind();
				lastShader = 'P';

				boxVBO.setData(MAX_BOX_BATCH * 4 * VERTEX_SIZE * sizeof(float), floatArray);
				boxVAO.draw(&boxIBO);

				floatIndex = 0;
				boundTextures.clear();
			}
			if (type == 'S') {
				ui::PipeTextBox* box = uiObjects.readItem<ui::PipeTextBox>();

				if (lastShader != 'G')
					guiShad->bind();
				lastShader = 'G';
				guiShad->setVec2("uPos", { box->x, box->y });
				guiShad->setVec2("uSize", { 1, 1 });
				guiShad->setVec4("uColor", box->rgba.r, box->rgba.g, box->rgba.b, box->rgba.a);
				DrawString(box->font, uiStrings[box->text_index], false, box->h, 9999, 9999, box->at);

				// don't continue with other stuff
				continue;
			}
			lastShader = 1;
			if (type == 0)
				break;
		}
		uiObjects.clear();
		uiStrings.clear();
	}
}